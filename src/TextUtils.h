#ifndef TEXTUTILS_H_
#define TEXTUTILS_H_

#include <QObject>
#include <QString>

namespace canadainc {

class TextUtils : public QObject
{
	Q_OBJECT

public:
	TextUtils(QObject* parent=NULL);
	virtual ~TextUtils();

    Q_INVOKABLE static bool isEmail(QString const& input);
    Q_INVOKABLE static bool isPhoneNumber(QString const& input);
    Q_INVOKABLE static bool isSimilar(QString a, QString b, qreal percentage=80, int n=2, Qt::CaseSensitivity caseSense=Qt::CaseInsensitive);
    Q_INVOKABLE static bool isUrl(QString const& input);
    Q_INVOKABLE static int randInt(int low, int high);
    Q_INVOKABLE static QString bytesToSize(qint64 bytes);
    Q_INVOKABLE static QString sanitize(QString const& original);
    Q_INVOKABLE static QString zeroFill(int input, int numZeroes=3);
};

} /* namespace canadainc */
#endif /* TEXTUTILS_H_ */
