#include "TextUtils.h"

#include <QRegExp>
#include <QStringList>

#include <math.h>
#include <stdint.h>
#include <sys/types.h>

#include <iostream>
#include <string>
#include <set>
#include <cstdlib>

#define BYTES_PER_KB 1024

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

using namespace std;

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

    for (int i = 0; i < original.length(); i++)
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


QString TextUtils::longestCommonSubstring(QString const& s1, QString const& s2)
{
    string str1 = s1.toStdString();
    string str2 = s2.toStdString();

    set<char *> res;
    string res_str;
    int longest = 0;

    int **n = (int **) calloc (str1.length() + 1,  sizeof(int *));
    for(int i = 0; i <= str1.length(); i++) {
        n[i] = (int *) calloc (str2.length() + 1, sizeof(int));
    }

    for(int i = 0; i < str1.length(); i ++) {
        for(int j = 0; j < str2.length(); j++) {
            if( toupper(str1[i]) == toupper(str2[j]) )
            {
                n[i+1][j+1] = n[i][j] + 1;
                if(n[i+1][j+1] > longest) {
                    longest = n[i+1][j+1];
                    res.clear();
                }
                if(n[i+1][j+1] == longest)
                    for(int it = i-longest+1; it <= i; it++){
                        res.insert((char *) &str1[it]);
                    }
            }
        }

    }
    for(set<char *>::const_iterator it = res.begin(); it != res.end(); it ++)
    {
        res_str.append(1,**it);
    }
    for(int i = 0; i <= str1.length(); i++)
        free(n[i]);
    free(n);

    return QString::fromStdString(res_str);
}


QString TextUtils::bytesToSize(qint64 bytes)
{
    static QStringList sizes = QStringList() << tr("Bytes") << tr("KB") << tr("MB") << tr("GB") << tr("TB");

    if (bytes == 0) {
        return tr("0 Bytes");
    }

    int i = floor( log(bytes) / log(BYTES_PER_KB) );
    double rounded = bytes / pow(BYTES_PER_KB, i);

    return QString("%1 %2").arg( QString::number(rounded, 'f', 2) ).arg( sizes[i] );
}


int TextUtils::randInt(int low, int high) {
    return qrand() % ((high + 1) - low) + low;
}


QString TextUtils::getPlaceHolders(int n, bool multi, QString const& symbol)
{
    QStringList placeHolders;

    for (int i = 0; i < n; i++) {
        placeHolders << symbol;
    }

    return placeHolders.join(multi ? "),(" : ",");
}


TextUtils::~TextUtils()
{
}

} /* namespace canadainc */
