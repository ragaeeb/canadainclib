#include "IOUtils.h"
#include "Logger.h"

#include <QDir>
#include <QTextStream>
#include <QThreadPool>

namespace canadainc {

const char* IOUtils::directory_local_shared = "/accounts/1000/shared";
const char* IOUtils::directory_sdcard = "/accounts/1000/removable/sdcard";
const char* IOUtils::directory_temp = "/var/tmp";

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


bool IOUtils::writeTextFile(QString const& filePath, QString contents, bool replace, bool correctNewLines)
{
	LOGGER(filePath << contents << replace);

	QFile outputFile(filePath);
	bool opened = outputFile.open(replace ? QIODevice::WriteOnly : QIODevice::WriteOnly | QIODevice::Append);

	if (opened)
	{
		QTextStream stream(&outputFile);

		if ( outputFile.exists() && !replace ) {
			LOGGER("Appending to file");
			stream << QString("%1%1").arg(NEW_LINE);
		}

		if (correctNewLines) {
			stream << contents.replace("\n", NEW_LINE);
		} else {
			stream << contents;
		}

		outputFile.close();
	} else {
		LOGGER("Could not open " << filePath << "for writing!");
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
		LOGGER("Could not open " << filePath << "for writing!");
		result = QString();
	}

	outputFile.close();
	return result;
}


} /* namespace canadainc */
