#include "TextUtils.h"

#include <QStringList>
#include <QTime>

#include <math.h>
#include <stdint.h>
#include <sys/types.h>

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


int TextUtils::randInt(int low, int high)
{
    static bool seeded = false;

    if (!seeded)
    {
        QTime time = QTime::currentTime();
        qsrand( (uint)time.msec() );
        seeded = true;
    }

    return qrand() % ((high + 1) - low) + low;
}


bool TextUtils::isSimilar(QString a, QString b, qreal percentage, int n, Qt::CaseSensitivity caseSense)
{
    if (a.isEmpty()||b.isEmpty()) return false;
    qreal hits=0;
    const QString spaces = QString(" ").repeated(n-1);
    a = spaces + a + spaces;
    b= spaces + b + spaces;

    QString part;
    for (int i = 0; i < a.count()-(n-1); i++)
    {
        part=a.mid(i,n);
        if (b.contains(part,caseSense)) hits++;
    }

    if (a.length()<b.length()) return (percentage < (100*hits/(a.length()-(n-1))));
    else return (percentage < (100*hits/(b.length()-(n-1))));
}


TextUtils::~TextUtils()
{
}

} /* namespace canadainc */
