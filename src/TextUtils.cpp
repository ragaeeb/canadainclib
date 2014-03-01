#include "TextUtils.h"

#include <QRegExp>
#include <math.h>

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


TextUtils::~TextUtils()
{
}

} /* namespace canadainc */
