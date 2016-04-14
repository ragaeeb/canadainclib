#include "SearchDecorator.h"

#include <bb/cascades/AbstractTextControl>
#include <bb/cascades/ArrayDataModel>

#include "Logger.h"

#include <string>
#include <set>

#define DECORATE(text, searchText) text.replace(searchText, "<span style='font-style:italic;font-weight:bold;color:lightgreen'>"+searchText+"</span>", Qt::CaseInsensitive)
#define HTMLIZE(input) "<html>"+input+"</html>"

using namespace bb::cascades;
using namespace std;

namespace {

void searchAndDecorate(QVariantList input, ArrayDataModel* adm, QVariantList const& queries, QString const& textKey)
{
    QStringList searches;

    foreach (QVariant const& q, queries) {
        searches << q.toString();
    }

    QRegExp regex = QRegExp( QString("(%1)").arg( searches.join("|") ) );
    regex.setCaseSensitivity(Qt::CaseInsensitive);

    for (int i = input.size()-1; i >= 0; i--)
    {
        QVariantMap current = input[i].toMap();
        QString text = canadainc::SearchDecorator::toHtmlEscaped( current.value(textKey).toString() );
        text.replace( regex, "<span style='font-style:italic;font-weight:bold;color:lightgreen'>\\1</span>" );

        current[textKey] = HTMLIZE(text);
        input[i] = current;

        adm->replace(i, current);
    }
}

}

namespace canadainc {

SimilarReference::SimilarReference() : adm(NULL), textControl(NULL)
{
}


SimilarReference SearchDecorator::decorateSimilar(QVariantList input, ArrayDataModel* adm, AbstractTextControl* atc, QString body)
{
    SimilarReference s;
    s.adm = adm;
    s.textControl = atc;

    int n = input.size();

    if (n > 0)
    {
        QString common = longestCommonSubstring( body, input[0].toMap().value("content").toString() );
        DECORATE(body, common);
        s.body = HTMLIZE(body);
    }

    for (int i = 0; i < n; i++)
    {
        QVariantMap current = input[i].toMap();
        QString text = current.value("content").toString();
        QString common = longestCommonSubstring(text, body);

        DECORATE(text, common);
        s.body = HTMLIZE(text);
        input[i] = current;
    }

    s.input = input;

    return s;
}


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
        else if (input.at(i) == QLatin1Char('\r')) {
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
        }
        else
            rich += input.at(i);
    }
    rich.squeeze();
    return rich;
}

QString SearchDecorator::longestCommonSubstring(QString const& s1, QString const& s2)
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


void SearchDecorator::decorateSearchResults(QVariantList const& input, bb::cascades::ArrayDataModel* adm, QVariantList const& queries, QString const& key) {
    QtConcurrent::run(&searchAndDecorate, input, adm, queries, key);
}


SearchDecorator::SearchDecorator(QObject* parent) : QObject(parent)
{
}


SearchDecorator::~SearchDecorator()
{
}


void SearchDecorator::onResultsDecorated(SimilarReference const& result)
{
    QVariantList data = result.input;

    if (result.adm)
    {
        ArrayDataModel* adm = result.adm;

        for (int i = data.size()-1; i >= 0; i--) {
            adm->replace(i, data[i]);
        }
    }

    if (result.textControl) {
        result.textControl->setText(result.body);
    }
}

} /* namespace islamiclib */
