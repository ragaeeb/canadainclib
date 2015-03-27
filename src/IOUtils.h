#ifndef IOUTILS_H_
#define IOUTILS_H_

#include <QByteArray>
#include <QRunnable>
#include <QString>

#define NEW_LINE "\r\n"
#define directory_local_shared "/accounts/1000/shared"
#define directory_sdcard "/accounts/1000/removable/sdcard"
#define directory_temp "/var/tmp"

namespace canadainc {

class IOUtils
{
public:
	static void startThread(QRunnable* qr, bool autoDelete=true);
	static bool writeFile(QString const& filePath, QByteArray const& data=QByteArray(), bool replace=true);

	/**
	 * @param replace If this is <code>false</code> then we will just append to the text file.
	 */
	static bool writeTextFile(QString const& filePath, QString contents, bool replace=true, bool correctNewLines=true, bool log=true);

	static QString readTextFile(QString const& filePath);

	static QString setupOutputDirectory(QString const& sharedFolder, QString const& name);

	static QStringList executeCommand(QString const& command);

	static void preventIndexing(QString const& dirPath);

	static void clearAllCache();

	static bool removeDir(QString const& dirName);

	static QString getMd5(QByteArray const& input);
};

} /* namespace canadainc */
#endif /* IOUTILS_H_ */
