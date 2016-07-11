#include "CardUtils.h"

#include <bb/cascades/Application>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Page>

namespace canadainc {

using namespace bb::cascades;

QObject* CardUtils::initAppropriate(QString const& qmlDoc, QMap<QString, QObject*> context, QObject* parent, bool qrc)
{
    QDeclarativeContext* rootContext = QmlDocument::defaultDeclarativeEngine()->rootContext();
    context["app"] = parent;

    bool invoked = qmlDoc != "main.qml";
    QmlDocument* qml = QmlDocument::create( QString(qrc ? "qrc:/assets/%1": "asset:///%1").arg(qmlDoc) ).parent(parent);

    foreach ( QString const& key, context.keys() ) {
        rootContext->setContextProperty( key, context.value(key) );
    }

    AbstractPane* root = NULL;

    if (invoked)
    {
        NavigationPane* np = NavigationPane::create().backButtons(true);
        qml->setContextProperty("navigationPane", np);

        Page* page = qml->createRootObject<Page>();
        np->push(page);

        root = page;
        Application::instance()->setScene(np);
    } else {
        root = qml->createRootObject<AbstractPane>();
        Application::instance()->setScene(root);
    }

    QObject::connect( parent, SIGNAL( initialize() ), parent, SLOT( lazyInit() ), Qt::QueuedConnection ); // async startup

    return root;
}

} /* namespace canadainc */
