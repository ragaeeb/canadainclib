#include "CardUtils.h"

#include <bb/cascades/Application>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Page>

namespace canadainc {

using namespace bb::cascades;

QObject* CardUtils::initAppropriate(QString const& qmlDoc, QMap<QString, QObject*> const& context, QObject* parent)
{
    QmlDocument* qml = QmlDocument::create( QString("asset:///%1").arg(qmlDoc) ).parent(parent);

    foreach ( QString const& key, context.keys() ) {
        qml->setContextProperty( key, context.value(key) );
    }

    AbstractPane* root = qml->createRootObject<AbstractPane>();

    if (qmlDoc != "main.qml")
    {
        Page* page = qobject_cast<Page*>(root);

        if (page) {
            NavigationPane* np = NavigationPane::create().backButtons(true);
            np->push(page);

            Application::instance()->setScene(np);
        } else {
            Application::instance()->setScene(root);
        }
    } else {
        Application::instance()->setScene(root);
    }

    QObject::connect( parent, SIGNAL( initialize() ), parent, SLOT( lazyInit() ), Qt::QueuedConnection ); // async startup

    return root;
}

} /* namespace canadainc */
