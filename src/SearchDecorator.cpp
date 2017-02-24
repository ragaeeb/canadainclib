#include "SearchDecorator.h"

#include <bb/cascades/AbstractTextControl>
#include <bb/cascades/ArrayDataModel>

#include "Logger.h"

#include <string>
#include <set>

#define DECORATE(body, common) body.replace( common, "<span style='font-style:italic;font-weight:bold;color:lightgreen'>"+common+"</span>", Qt::CaseInsensitive )
#define HTMLIZE(input) "<html>"+input+"</html>"

using namespace bb::cascades;
using namespace std;

namespace {

QString longestCommonSubstring(QString const& s1, QString const& s2)
{
    string str1 = s1.toStdString();
    string str2 = s2.toStdString();

    set<char *> res;
    string res_str;
    int longest = 0;

    int **n = (int **) calloc (str1.length() + 1,  sizeof(int *));
    for(uint i = 0; i <= str1.length(); i++) {
        n[i] = (int *) calloc (str2.length() + 1, sizeof(int));
    }

    for(uint i = 0; i < str1.length(); i ++) {
        for(uint j = 0; j < str2.length(); j++) {
            if( toupper(str1[i]) == toupper(str2[j]) )
            {
                n[i+1][j+1] = n[i][j] + 1;
                if(n[i+1][j+1] > longest) {
                    longest = n[i+1][j+1];
                    res.clear();
                }
                if(n[i+1][j+1] == longest)
                    for(uint it = i-longest+1; it <= i; it++){
                        res.insert((char *) &str1[it]);
                    }
            }
        }

    }
    for(set<char *>::const_iterator it = res.begin(); it != res.end(); it ++) {
        res_str.append(1,**it);
    }

    for(uint i = 0; i <= str1.length(); i++)
        free(n[i]);
    free(n);

    return QString::fromStdString(res_str);
}

QVariantMap decorateElement(QVariantMap current, QRegExp const& regex, QString const& textKey)
{
    QString text = canadainc::SearchDecorator::toHtmlEscaped( current.value(textKey).toString() );
    text.replace( regex, "<span style='font-style:italic;font-weight:bold;color:lightgreen'>\\1</span>" );

    current[textKey] = HTMLIZE(text);

    return current;
}

void searchAndDecorate(QVariantList input, ArrayDataModel* adm, QVariantList const& queries, QString const& textKey, int index)
{
    QStringList searches;

    foreach (QVariant const& q, queries) {
        searches << q.toString();
    }

    QRegExp regex = QRegExp( QString("(%1)").arg( searches.join("|") ) );
    regex.setCaseSensitivity(Qt::CaseInsensitive);

    if (index < 0)
    {
        for (int i = input.size()-1; i >= 0; i--)
        {
            QVariantMap current = input[i].toMap();
            current = decorateElement(current, regex, textKey);
            adm->replace(i, current);
        }
    } else {
        QVariantMap current = input[index].toMap();
        current = decorateElement(current, regex, textKey);
        adm->replace(index, current);
    }
}

void groupAndDecorate(QVariantList const& input, ArrayDataModel* adm, AbstractTextControl* atc, QString const& textKey)
{
    QString body = canadainc::SearchDecorator::toHtmlEscaped( atc->text() );

    for (int i = input.size()-1; i >= 0; i--)
    {
        QVariantMap current = input[i].toMap();
        QString text = canadainc::SearchDecorator::toHtmlEscaped( current.value(textKey).toString() );
        QString common = longestCommonSubstring(body, text);
        DECORATE(text, common);

        current[textKey] = HTMLIZE(text);
        adm->replace(i, current);

        if (i == 0) {
            DECORATE(body, common);
            atc->setProperty("decorated", HTMLIZE(body) );
        }
    }
}

}

namespace canadainc {

QString SearchDecorator::toHtmlEscaped(QString const& input)
{
    QString rich;
    const int len = input.length();
    rich.reserve(int(len * 1.1));
    for (int i = 0; i < len; ++i) {
        if (input.at(i) == QLatin1Char('<'))
            rich += QLatin1String("&lt;");
        else if (input.at(i) == QLatin1Char('>'))
            rich += QLatin1String("&gt;");
        else if (input.at(i) == QLatin1Char('&'))
            rich += QLatin1String("&amp;");
        else if (input.at(i) == QLatin1Char('"'))
            rich += QLatin1String("&quot;");
        /*else if (input.at(i) == QLatin1Char('\r')) {
            if(i < len - 1 && input.at(i+1) != QLatin1Char('\n')) {
                rich += QLatin1String("<br>");
            }
        } else if (input.at(i) == QLatin1Char('\n'))
            rich += QLatin1String("<br>");
        else if (input.at(i) == QLatin1Char(' ')) {
            if(i < len - 1 && input.at(i+1) == QLatin1Char(' '))
                rich += QLatin1String("&nbsp;");
            else
                rich += input.at(i);
        } */
        else
            rich += input.at(i);
    }
    rich.squeeze();
    return rich;
}


void SearchDecorator::decorateSearchResults(QVariantList const& input, bb::cascades::ArrayDataModel* adm, QVariantList const& queries, QString const& key, int index) {
    QtConcurrent::run(&searchAndDecorate, input, adm, queries, key, index);
}

void SearchDecorator::decorateSimilar(QVariantList const& input, bb::cascades::ArrayDataModel* adm, bb::cascades::AbstractTextControl* atc, QString const& key) {
    QtConcurrent::run(&groupAndDecorate, input, adm, atc, key);
}


SearchDecorator::SearchDecorator(QObject* parent) : QObject(parent)
{
}


SearchDecorator::~SearchDecorator()
{
}

} /* namespace islamiclib */
