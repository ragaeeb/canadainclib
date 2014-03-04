#include "LazyMediaPlayer.h"
#include "Logger.h"

#include <bb/multimedia/MediaPlayer>
#include <bb/multimedia/NowPlayingConnection>

namespace canadainc {

using namespace bb::multimedia;

LazyMediaPlayer::LazyMediaPlayer(QObject* parent) :
		QObject(parent), m_mp(NULL), m_npc(NULL), m_currentIndex(0)
{
}


void LazyMediaPlayer::setName(QString const& name) {
	m_name = name;
}


void LazyMediaPlayer::play(QStringList const& playlist)
{
	LOGGER("Play" << playlist);

	if (m_npc == NULL) {
		LOGGER("Creating MediaPlayer for first time");
		m_mp = new MediaPlayer(this);
		m_npc = new NowPlayingConnection(m_name, this);

		connect( m_npc, SIGNAL( pause() ), m_mp, SLOT( pause() ) );
		connect( m_npc, SIGNAL( acquired() ), m_mp, SLOT( play() ) );
		connect( m_npc, SIGNAL( play() ), m_mp, SLOT( play() ) );
		connect( m_npc, SIGNAL( revoked() ), m_mp, SLOT( pause() ) );
		connect( m_mp, SIGNAL( positionChanged(unsigned int) ), this, SIGNAL( positionChanged(unsigned int) ) );
		connect( m_mp, SIGNAL( mediaStateChanged(bb::multimedia::MediaState::Type) ), this, SLOT( mediaStateChanged(bb::multimedia::MediaState::Type) ) );
		connect( m_mp, SIGNAL( durationChanged(unsigned int) ), this, SIGNAL( durationChanged(unsigned int) ) );
		connect( m_mp, SIGNAL( metaDataChanged(QVariantMap const&) ), this, SIGNAL( metaDataChanged(QVariantMap const&) ) );
		connect( m_mp, SIGNAL( playbackCompleted() ), this, SLOT( playbackCompleted() ) );
		connect( m_mp, SIGNAL( videoDimensionsChanged(QSize const&) ), this, SIGNAL( videoDimensionsChanged(QSize const&) ) );

		if ( !m_videoWindowId.isNull() ) {
			m_mp->setWindowId(m_videoWindowId);
			m_mp->setVideoOutput(VideoOutput::PrimaryDisplay);
		}
	} else {
		m_mp->reset();
	}

	seek(0);
	m_currentIndex = 0;
	m_playlist = playlist;

	skip(0);
}


void LazyMediaPlayer::mediaStateChanged(bb::multimedia::MediaState::Type mediaState)
{
	Q_UNUSED(mediaState);

	emit activeChanged();
	emit playingChanged();
}

void LazyMediaPlayer::play(QString const& uri) {
	play( QStringList() << uri );
}


void LazyMediaPlayer::skip(int n)
{
	LOGGER("============ SKIP" << n);
	m_currentIndex += n;

	if ( m_currentIndex < m_playlist.size() && m_currentIndex >= 0 )
	{
		QString uri = m_playlist[m_currentIndex];

		if ( m_mp->sourceUrl().toString() != uri ) {
			m_mp->setSourceUrl( QUrl(uri) );
		}

		if ( m_npc->isAcquired() ) {
			LOGGER("Already acquired, playing!");
			m_mp->play();
		} else {
			LOGGER("Acquiring NPC!");
			m_npc->acquire();
		}
	} else {
		emit playlistCompleted();
	}
}


QVariant LazyMediaPlayer::currentPosition() const {
	return m_mp ? m_mp->position() : 0;
}


void LazyMediaPlayer::playbackCompleted() {
	skip(1);
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
	return m_currentIndex;
}


int LazyMediaPlayer::count() const {
	return m_playlist.size();
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
