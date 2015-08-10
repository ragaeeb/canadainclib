#include "IOUtils.h"
#include "Logger.h"

#include <QCryptographicHash>
#include <QDir>
#include <QTextStream>
#include <QThreadPool>

namespace canadainc {

void IOUtils::startThread(QRunnable* qr, bool autoDelete)
{
	qr->setAutoDelete(autoDelete);

	LOGGER("Starting thread");
	QThreadPool *threadPool = QThreadPool::globalInstance();
	threadPool->start(qr);
}


QString IOUtils::setupOutputDirectory(QString const& sharedFolder, QString const& name)
{
	QString sdDirectory = QString("%1/%2").arg(directory_sdcard).arg(sharedFolder);

	if ( !QDir(sdDirectory).exists() ) {
		sdDirectory = QString("%1/%2/%3").arg(directory_local_shared).arg(sharedFolder).arg(name);
	} else {
		sdDirectory = QString("%1/%2").arg(sdDirectory).arg(name);
	}

	QDir dir(sdDirectory);
	if ( !dir.exists() ) {
		bool created = dir.mkdir(sdDirectory);
		LOGGER("Directory didn't exist, creating!" << created);
	}

	return sdDirectory;
}


bool IOUtils::writeFile(QString const& filePath, QByteArray const& data, bool replace)
{
	bool result = false;
	QFile file(filePath);

	if ( file.exists() && replace ) {
		bool removed = file.remove();
		LOGGER("File already existed, removed: " << removed);
	}

	result = file.open(QIODevice::WriteOnly);
	LOGGER( "opened file" << result << file.fileName() );

	if ( !data.isNull() ) {
		file.write(data);
	}

	file.close();

	return result;
}


bool IOUtils::writeTextFile(QString const& filePath, QString contents, bool replace, bool correctNewLines, bool log)
{
    if (log) {
        LOGGER(filePath << contents.length() << replace << correctNewLines);
    }

	QFile outputFile(filePath);
	bool opened = outputFile.open(replace ? QIODevice::WriteOnly : QIODevice::WriteOnly | QIODevice::Append);

	if (opened)
	{
		QTextStream stream(&outputFile);

		if ( outputFile.exists() && !replace )
		{
		    if (log) {
	            LOGGER("Appending to file");
		    }

			stream << QString("%1%1").arg(NEW_LINE);
		}

		if (correctNewLines) {
			stream << contents.replace("\n", NEW_LINE);
		} else {
			stream << contents;
		}

		outputFile.close();
	} else {
	    if (log) {
	        LOGGER("Could not open " << filePath << "for writing!");
	    }

		return false;
	}

	return true;
}


QString IOUtils::readTextFile(QString const& filePath)
{
	QFile outputFile(filePath);
	bool opened = outputFile.open(QIODevice::ReadOnly);
	QString result;

	if (opened) {
		QTextStream stream(&outputFile);
		result = stream.readAll();
	} else {
		LOGGER("Could not open " << filePath << "for reading!");
		result = QString();
	}

	outputFile.close();
	return result;
}


QStringList IOUtils::executeCommand(QString const& command)
{
    QStringList result;
    FILE *fp;
    char path[2048];

    fflush(stdout);

    // Open the command for reading.
    fp = popen( command.toStdString().c_str(), "r" );

    if (fp == NULL) {
        printf("NoDataFound\n");
        fflush(stdout);
        pclose(fp);
    }

    // Read the output a line at a time - output it.
    while ( fgets(path, sizeof(path) - 1, fp) != NULL )
    {
        // printf("%s\n", path);
        // fflush(stdout);
        result.append(path);
    }

    // close
    pclose(fp);

    return result;
}


void IOUtils::preventIndexing(QString const& dirPath)
{
    QDir q(dirPath);

    if ( q.exists() )
    {
        QFile f( QString("%1/.nomedia").arg(dirPath) );

        if ( !f.exists() ) {
            bool written = f.open(QIODevice::WriteOnly);
            f.close();
            LOGGER(f.fileName() << "written" << written);
        } else {
            LOGGER(".nomediaExists");
        }
    } else {
        LOGGER(dirPath << "noexists");
    }
}


void IOUtils::clearAllCache()
{
    QString homePath = QDir::homePath();

    QFile::remove( QString("%1/WebpageIcons.db").arg(homePath) );
    QFile::remove( QString("%1/cookieCollection.db").arg(homePath) );
    QFile::remove( QString("%1/cookieCollection.db-wal").arg(homePath) );
    QFile::remove( QString("%1/storagequota.db").arg(homePath) );

    removeDir( QString("%1/appcache").arg(homePath) );
    removeDir( QString("%1/cache").arg(homePath) );
    removeDir( QString("%1/certificates").arg(homePath) );
    removeDir( QString("%1/downloads").arg(homePath) );
    removeDir( QString("%1/localstorage").arg(homePath) );
}


bool IOUtils::removeDir(QString const& dirName)
{
    bool result = true;
    QDir dir(dirName);

    if ( dir.exists(dirName) )
    {
        foreach( QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst) )
        {
            if ( info.isDir() ) {
                result = removeDir(info.absoluteFilePath());
            } else {
                result = QFile::remove( info.absoluteFilePath() );
            }
        }

        result = dir.rmdir(dirName);
    }

    return result;
}


QString IOUtils::getMd5(QByteArray const& input)
{
    return QString( QCryptographicHash::hash(input, QCryptographicHash::Md5).toHex() );
}


bool IOUtils::validateMd5(QString const& expected, QByteArray const& input) {
    return expected == getMd5(input);
}


bool IOUtils::writeIfValidMd5(QString const& filePath, QString const& expectedMd5, QByteArray const& data, bool replace)
{
    if ( expectedMd5 == getMd5(data) ) {
        return writeFile(filePath, data, replace);
    } else {
        return false;
    }
}


QMap<QString,QString> IOUtils::extractPpsValue(QString const& path, QMap<QString, QString> const& keyPrefix)
{
    QMap<QString,QString> result;

    if ( QFile::exists(path) )
    {
        QStringList data = readTextFile(path).split(LINE_SEPARATOR);

        foreach (QString const& current, data) // go line by line
        {
            QStringList keys = keyPrefix.keys();

            foreach (QString const& key, keys)
            {
                QString prefix = keyPrefix.value(key);

                if ( current.startsWith(prefix) ) {
                    result.insert( key, current.mid( prefix.length() ).trimmed() );
                }
            }
        }
    }

    return result;
}


QString IOUtils::extractPpsValue(QString const& path, QString const& prefix)
{
    QMap<QString, QString> keyPrefix;
    keyPrefix[prefix] = prefix;

    keyPrefix = extractPpsValue(path, keyPrefix);

    return keyPrefix.value(prefix);
}


} /* namespace canadainc */
