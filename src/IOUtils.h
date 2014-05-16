#ifndef IOUTILS_H_
#define IOUTILS_H_

#include <QByteArray>
#include <QRunnable>
#include <QString>

#define NEW_LINE "\r\n"

namespace canadainc {

class IOUtils
{
public:
	static void startThread(QRunnable* qr, bool autoDelete=true);
	static bool writeFile(QString const& filePath, QByteArray const& data=QByteArray(), bool replace=true);

	/**
	 * @param replace If this is <code>false</code> then we will just append to the text file.
	 */
	static bool writeTextFile(QString const& filePath, QString contents, bool replace=true, bool correctNewLines=true);

	static QString readTextFile(QString const& filePath);

	static QString setupOutputDirectory(QString const& sharedFolder, QString const& name);

	static const char* directory_local_shared;
	static const char* directory_sdcard;
	static const char* directory_temp;
};

} /* namespace canadainc */
#endif /* IOUTILS_H_ */
