#include "LazySceneCover.h"
#include "Logger.h"

#include <bb/cascades/Application>
#include <bb/cascades/Control>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/SceneCover>

namespace canadainc {

using namespace bb::cascades;

LazySceneCover::LazySceneCover(QString const& sceneCoverQml, bool cycle)
{
	setProperty("qml", sceneCoverQml);

	Application* app = Application::instance();

	connect( app, SIGNAL( thumbnail() ), this, SLOT( onThumbnail() ) );

	if (cycle)
	{
		connect( app, SIGNAL( fullscreen() ), this, SLOT( tearDown() ) );
		connect( app, SIGNAL( invisible() ), this, SLOT( tearDown() ) );
	}

}


void LazySceneCover::tearDown()
{
	LOGGER("Tearing down scene cover");
	QObject* cover = Application::instance()->cover();

	if (cover)
	{
		cover->deleteLater();
		Application::instance()->resetCover();
	}
}


void LazySceneCover::setContext(QString const& key, QObject* value) {
	m_context.insert(key, value);
}


void LazySceneCover::onThumbnail()
{
	LOGGER("Thumbnailed");

	if ( Application::instance()->cover() == NULL )
	{
		LOGGER("Creating thumbnail scene cover for first time!");

		QmlDocument* qmlCover = QmlDocument::create( "asset:///"+property("qml").toString() );

		QStringList keys = m_context.keys();

		for (int i = keys.size()-1; i >= 0; i--) {
			qmlCover->setContextProperty( keys[i], m_context.value(keys[i]) );
		}

		Control* sceneRoot = qmlCover->createRootObject<Control>();
		SceneCover* cover = SceneCover::create().content(sceneRoot);
		qmlCover->setParent(cover);
		Application::instance()->setCover(cover);
	}
}


LazySceneCover::~LazySceneCover()
{
}

} /* namespace canadainc */
