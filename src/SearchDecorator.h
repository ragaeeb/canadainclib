#ifndef SEARCHDECORATOR_H_
#define SEARCHDECORATOR_H_

#include <QObject>
#include <QVariant>

namespace bb {
    namespace cascades {
        class ArrayDataModel;
        class AbstractTextControl;
    }
}

namespace canadainc {

struct SimilarReference
{
    bb::cascades::ArrayDataModel* adm;
    QVariantList input;
    bb::cascades::AbstractTextControl* textControl;
    QString body;

    SimilarReference();
};

struct SearchDecorator : public QObject
{
    Q_OBJECT

public:
    static void decorateResults(QVariantList input, bb::cascades::ArrayDataModel* adm, QVariantList const& additional, QString const& textKey);
    static SimilarReference decorateSimilar(QVariantList input, bb::cascades::ArrayDataModel* adm, bb::cascades::AbstractTextControl* atc, QString body);
    static void onResultsDecorated(SimilarReference const& result);
    static QString toHtmlEscaped(QString const& input);
    static QString longestCommonSubstring(QString const& str1, QString const& str2);
    Q_INVOKABLE void decorateSearchResults(QVariantList const& input, bb::cascades::ArrayDataModel* adm, QVariantList const& queries, QString const& key);

    SearchDecorator(QObject* parent=NULL);
    virtual ~SearchDecorator();
};

} /* namespace canadainc */

#endif /* SEARCHDECORATOR_H_ */
