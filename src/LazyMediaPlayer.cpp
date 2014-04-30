#include "LazyMediaPlayer.h"
#include "Logger.h"

#include <bb/Application>

#include <bb/multimedia/MediaPlayer>
#include <bb/multimedia/NowPlayingConnection>

namespace canadainc {

using namespace bb::multimedia;

LazyMediaPlayer::LazyMediaPlayer(bool fastShutdown, QObject* parent) :
		QObject(parent), m_mp(NULL), m_npc(NULL), m_repeat(false), m_fastShutDown(true)
{
    if (m_fastShutDown) {
        connect( bb::Application::instance(), SIGNAL( aboutToQuit() ), this, SLOT( pause() ) );
    }
}


void LazyMediaPlayer::setName(QString const& name) {
	m_name = name;
}


void LazyMediaPlayer::play(QUrl const& uri)
{
	LOGGER("Play" << uri);

	if (m_npc == NULL)
	{
	    QObject* parent = m_fastShutDown ? NULL : this;
		m_mp = new MediaPlayer(parent);
		m_npc = new NowPlayingConnection(m_name, parent);

		setRepeat(m_repeat);

		connect( m_npc, SIGNAL( pause() ), m_mp, SLOT( pause() ) );
		connect( m_npc, SIGNAL( acquired() ), m_mp, SLOT( play() ) );
		connect( m_npc, SIGNAL( play() ), m_mp, SLOT( play() ) );
		connect( m_npc, SIGNAL( revoked() ), m_mp, SLOT( pause() ) );
		connect( m_mp, SIGNAL( durationChanged(unsigned int) ), this, SIGNAL( durationChanged(unsigned int) ) );
        connect( m_mp, SIGNAL( mediaStateChanged(bb::multimedia::MediaState::Type) ), this, SLOT( mediaStateChanged(bb::multimedia::MediaState::Type) ) );
		connect( m_mp, SIGNAL( metaDataChanged(QVariantMap const&) ), this, SIGNAL( metaDataChanged(QVariantMap const&) ) );
		connect( m_mp, SIGNAL( playbackCompleted() ), this, SIGNAL( playbackCompleted() ) );
        connect( m_mp, SIGNAL( positionChanged(unsigned int) ), this, SIGNAL( positionChanged(unsigned int) ) );
        connect( m_mp, SIGNAL( trackChanged(unsigned int) ), this, SLOT( trackChanged(unsigned int) ) );
		connect( m_mp, SIGNAL( videoDimensionsChanged(QSize const&) ), this, SIGNAL( videoDimensionsChanged(QSize const&) ) );

		if ( !m_videoWindowId.isNull() ) {
			m_mp->setWindowId(m_videoWindowId);
			m_mp->setVideoOutput(VideoOutput::PrimaryDisplay);
		}
	} else {
		m_mp->reset();
	}

	m_mp->setSourceUrl(uri);

    if ( m_npc->isAcquired() ) {
        m_mp->play();
    } else {
        m_npc->acquire();
    }
}


void LazyMediaPlayer::trackChanged(unsigned int track)
{
    int converted = track-1;
    emit currentIndexChanged(converted);
}


void LazyMediaPlayer::mediaStateChanged(bb::multimedia::MediaState::Type mediaState)
{
    if (mediaState == MediaState::Started) {
        trackChanged( m_mp->track() );
        emit activeChanged();
    }

    emit playingChanged();
}


void LazyMediaPlayer::skip(int n)
{
	if (!m_mp) {
	    return;
	}

	m_mp->seekTrack( m_mp->track() + n );

    if ( m_npc->isAcquired() ) {
        m_mp->play();
    } else {
        m_npc->acquire();
    }
}


QVariant LazyMediaPlayer::currentPosition() const {
	return m_mp ? m_mp->position() : 0;
}


void LazyMediaPlayer::seek(unsigned int position)
{
	if (m_mp) {
		m_mp->seek( m_mp->track(), position );
	}
}


void LazyMediaPlayer::stop()
{
	if (m_mp) {
		m_mp->stop();
	}
}


void LazyMediaPlayer::pause()
{
	if (m_mp) {
		m_mp->pause();
	}
}


void LazyMediaPlayer::setVideoWindowId(QString const& windowId)
{
	m_videoWindowId = windowId;

	if (m_mp) {
		m_mp->setWindowId(windowId);
	}
}


void LazyMediaPlayer::toggleVideo()
{
	if (m_mp)
	{
		uint position = m_mp->position();

		if ( m_mp->videoOutput() == VideoOutput::None ) {
			m_mp->setVideoOutput(VideoOutput::PrimaryDisplay);
		} else {
			m_mp->setVideoOutput(VideoOutput::None);
		}

		m_mp->play();
		m_mp->seekTime(position);
	}
}


void LazyMediaPlayer::jump(int secs)
{
	if (m_mp) {
		m_mp->seekTime( m_mp->position() + secs );
	}
}


bool LazyMediaPlayer::playing() const {
	return m_mp && m_mp->mediaState() == MediaState::Started;
}

bool LazyMediaPlayer::active() const {
	return m_mp && ( m_mp->mediaState() == MediaState::Started || m_mp->mediaState() == MediaState::Paused );
}


int LazyMediaPlayer::currentIndex() const {
	return m_mp ? m_mp->track()-1 : 0;
}


int LazyMediaPlayer::count() const {
	return m_mp ? m_mp->trackCount() : 0;
}


QSize LazyMediaPlayer::videoDimensions() const {
	return m_mp ? m_mp->videoDimensions() : QSize();
}


QString LazyMediaPlayer::videoWindowId() {
	return m_mp && m_mp->videoOutput() == VideoOutput::PrimaryDisplay ? m_mp->windowId() : QString();
}


void LazyMediaPlayer::togglePlayback()
{
	if ( playing() ) {
		m_mp->pause();
	} else if (m_mp) {
		m_mp->play();
	}
}


bool LazyMediaPlayer::repeat() const {
    return m_repeat;
}


void LazyMediaPlayer::setRepeat(bool value)
{
    m_repeat = value;

    if (m_mp) {
        m_mp->setRepeatMode(value ? RepeatMode::All : RepeatMode::None);
    }
}


QVariantMap LazyMediaPlayer::metaData() const {
	return m_mp ? m_mp->metaData() : QVariantMap();
}


MediaPlayer* LazyMediaPlayer::mediaPlayer() {
	return m_mp;
}


LazyMediaPlayer::~LazyMediaPlayer()
{
	if (m_npc != NULL) {
		m_npc->revoke();
	}
}

} /* namespace salat */
