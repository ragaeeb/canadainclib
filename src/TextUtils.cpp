#include "TextUtils.h"

#include <QRegExp>

#include <math.h>
#include <stdint.h>
#include <sys/types.h>

namespace {

/*
*  Refer to Microsoft Knowledge Base Q120138; valid SFN
*  characters are: A-Z 0-9 $ % ' ` - @ { } ~ ! # ( ) & _ ^
*  Embedded spaces and a single . are also allowed.
*/
const uint8_t sfn_chars[] = {
    0x00, 0x00, 0x00, 0x00, 0xFB, 0x23, 0xFF, 0x03,
    0xFF, 0xFF, 0xFF, 0xC7, 0xFF, 0xFF, 0xFF, 0x6F,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

}

namespace canadainc {

TextUtils::TextUtils(QObject* parent) : QObject(parent)
{
}


QString TextUtils::zeroFill(int input, int numZeroes) {
	return QString::number(input).rightJustified(numZeroes,'0');
}


QString TextUtils::plainText(QString const& htmlString)
{
    QString plainString = htmlString;
    plainString.remove(QRegExp("<[^>]*>"));

    return plainString;
}


QString TextUtils::formatTime(unsigned int duration)
{
	unsigned int secs = floor(duration / 1000);
	secs %= 60;
	unsigned int mins = floor( (duration / (1000 * 60) ) % 60);
	unsigned int hrs = floor( (duration / (1000 * 60 * 60) ) % 24);

	QString seconds = QString::number(secs).rightJustified(2,'0');
	QString minutes = QString::number(mins).rightJustified(2,'0');
	QString hours = hrs > 0 ? QString("%1:").arg(hrs) : "";

	return tr("%1%2:%3").arg(hours).arg(minutes).arg(seconds);
}


QString TextUtils::sanitize(QString const& original)
{
    QString result;

    for (int i = original.length()-1; i >= 0; i--)
    {
        QChar c = original.at(i);
        uchar_t mychar = c.toAscii();
        bool valid = sfn_chars[mychar/8] & ( 1 << (mychar % 8) );

        if (valid) {
            result.append(c);
        } else {
            result.append("_");
        }
    }

    return result;
}


TextUtils::~TextUtils()
{
}

} /* namespace canadainc */
