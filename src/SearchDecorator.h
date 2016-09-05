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

struct SearchDecorator : public QObject
{
    Q_OBJECT

public:
    SearchDecorator(QObject* parent=NULL);
    virtual ~SearchDecorator();

    static QString toHtmlEscaped(QString const& input);
    Q_INVOKABLE void decorateSimilar(QVariantList const& input, bb::cascades::ArrayDataModel* adm, bb::cascades::AbstractTextControl* atc, QString const& key);
    Q_INVOKABLE void decorateSearchResults(QVariantList const& input, bb::cascades::ArrayDataModel* adm, QVariantList const& queries, QString const& key, int index=-1);
};

} /* namespace canadainc */

#endif /* SEARCHDECORATOR_H_ */
