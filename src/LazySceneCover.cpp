#include "LazySceneCover.h"
#include "Logger.h"

#include <bb/cascades/Application>
#include <bb/cascades/Control>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/SceneCover>

namespace canadainc {

using namespace bb::cascades;

LazySceneCover::LazySceneCover(bool appLaunch, QObject* ui, bool qrc) :
        m_ready(false), m_qrc(qrc)
{
    if (appLaunch)
    {
        Application* app = Application::instance();
        connect( app, SIGNAL( thumbnail() ), this, SLOT( onThumbnail() ) );

        QObject::connect( ui, SIGNAL( lazyInitComplete() ), this, SLOT( onReady() ), Qt::QueuedConnection ); // async startup
    }
}


void LazySceneCover::onReady() {
    m_ready = true;
}


void LazySceneCover::setContext(QString const& key, QObject* value) {
	m_context.insert(key, value);
}


void LazySceneCover::onThumbnail()
{
    if (m_ready)
    {
        LOGGER("Thumbnailed");

        if ( Application::instance()->cover() == NULL )
        {
            LOGGER("CreatingThumbnail");
            QmlDocument* qmlCover = QmlDocument::create(m_qrc ? "qrc:/assets/Cover.qml" : "asset:///Cover.qml").parent(this);

            QStringList keys = m_context.keys();

            for (int i = keys.size()-1; i >= 0; i--) {
                qmlCover->setContextProperty( keys[i], m_context.value(keys[i]) );
            }

            Control* sceneRoot = qmlCover->createRootObject<Control>();
            SceneCover* cover = SceneCover::create().content(sceneRoot);

            Application::instance()->setCover(cover);
        }
    }
}


LazySceneCover::~LazySceneCover()
{
}

} /* namespace canadainc */
