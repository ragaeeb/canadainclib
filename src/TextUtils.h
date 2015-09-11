#ifndef TEXTUTILS_H_
#define TEXTUTILS_H_

#include <QObject>

namespace canadainc {

class TextUtils : public QObject
{
	Q_OBJECT

public:
	TextUtils(QObject* parent=NULL);
	virtual ~TextUtils();

	Q_INVOKABLE static QString zeroFill(int input, int numZeroes=3);

    /*
     * Utility function to strip html-formatting.
     * @param htmlString The string that is going to be stripped.
     */
    Q_INVOKABLE static QString plainText(QString const& htmlString);

    /**
     * @param A duration in milliseconds.
     * @return 15:12:04
     */
    Q_INVOKABLE static QString formatTime(unsigned int duration);

    Q_INVOKABLE static QString sanitize(QString const& original);

    Q_INVOKABLE static QString bytesToSize(qint64 bytes);

    Q_INVOKABLE static QString longestCommonSubstring(QString const& str1, QString const& str2);

    Q_INVOKABLE static int randInt(int low, int high);

    Q_INVOKABLE static QString toTitleCase(QString const& s);
    Q_INVOKABLE static QString removeBrackets(QString& input);
    Q_INVOKABLE static QString optimize(QString input);
    Q_INVOKABLE static bool isSimilar(QString a, QString b, qreal percentage=80, int n=2, Qt::CaseSensitivity caseSense=Qt::CaseInsensitive);

    Q_INVOKABLE static bool isEmail(QString const& input);
    Q_INVOKABLE static bool isPhoneNumber(QString const& input);
    Q_INVOKABLE static bool isUrl(QString const& input);
};

} /* namespace canadainc */
#endif /* TEXTUTILS_H_ */
