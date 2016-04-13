#ifndef SEARCHDECORATOR_H_
#define SEARCHDECORATOR_H_

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

struct SearchDecorator
{
    static SimilarReference decorateResults(QVariantList input, bb::cascades::ArrayDataModel* adm, QVariantList const& additional, QString const& textKey);
    static SimilarReference decorateSimilar(QVariantList input, bb::cascades::ArrayDataModel* adm, bb::cascades::AbstractTextControl* atc, QString body);
    static QString longestCommonSubstring(QString const& str1, QString const& str2);
    static void onResultsDecorated(SimilarReference const& result);
};

} /* namespace canadainc */

#endif /* SEARCHDECORATOR_H_ */
