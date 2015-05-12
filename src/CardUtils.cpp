#include "CardUtils.h"

#include <bb/cascades/Application>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Page>

namespace canadainc {

using namespace bb::cascades;

QObject* CardUtils::initAppropriate(QString const& qmlDoc, QMap<QString, QObject*> const& context, QObject* parent)
{
    bool invoked = qmlDoc != "main.qml";
    QmlDocument* qml = QmlDocument::create( QString("asset:///%1").arg(qmlDoc) ).parent(parent);
    qml->setContextProperty("app", parent);

    foreach ( QString const& key, context.keys() ) {
        qml->setContextProperty( key, context.value(key) );
    }

    NavigationPane* np = NavigationPane::create().backButtons(true);
    qml->setContextProperty("navigationPane", np);

    AbstractPane* root = NULL;

    if (invoked)
    {
        NavigationPane* np = NavigationPane::create().backButtons(true);
        qml->setContextProperty("navigationPane", np);

        Page* page = qml->createRootObject<Page>();
        root = page;
        np->push(page);

        Application::instance()->setScene(np);
    } else {
        root = qml->createRootObject<AbstractPane>();
        Application::instance()->setScene(root);
    }

    QObject::connect( parent, SIGNAL( initialize() ), parent, SLOT( lazyInit() ), Qt::QueuedConnection ); // async startup

    return root;
}

} /* namespace canadainc */
