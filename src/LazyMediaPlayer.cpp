#include "LazyMediaPlayer.h"
#include "Logger.h"

#include <QCoreApplication>

#include <bb/multimedia/MediaPlayer>
#include <bb/multimedia/NowPlayingConnection>

namespace canadainc {

using namespace bb::multimedia;

LazyMediaPlayer::LazyMediaPlayer(QObject* parent) :
        QObject(parent), m_mp(NULL), m_npc(NULL), m_repeat(false), m_volume(1)
{
    connect( QCoreApplication::instance(), SIGNAL( aboutToQuit() ), this, SLOT( pause() ) );
}


void LazyMediaPlayer::setName(QString const& name) {
	m_name = name;
}


void LazyMediaPlayer::play(QUrl const& uri)
{
	LOGGER("Play" << uri);

	if (m_npc == NULL)
	{
		m_mp = new MediaPlayer(this);
		m_npc = new NowPlayingConnection(m_name, this);

		setRepeat(m_repeat);
		setVolume(m_volume);

		LOGGER("Using vol" << m_volume << m_mp->volume());

		connect( m_npc, SIGNAL( pause() ), m_mp, SLOT( pause() ) );
		connect( m_npc, SIGNAL( acquired() ), this, SLOT( playNow() ) );
		connect( m_npc, SIGNAL( play() ), this, SLOT( playNow() ) );
		connect( m_npc, SIGNAL( revoked() ), m_mp, SLOT( pause() ) );
		connect( m_mp, SIGNAL( durationChanged(unsigned int) ), this, SIGNAL( durationChanged(unsigned int) ) );
		connect( m_mp, SIGNAL( error(bb::multimedia::MediaError::Type, unsigned int) ), this, SLOT( error(bb::multimedia::MediaError::Type, unsigned int) ) );
        connect( m_mp, SIGNAL( mediaStateChanged(bb::multimedia::MediaState::Type) ), this, SLOT( mediaStateChanged(bb::multimedia::MediaState::Type) ) );
		connect( m_mp, SIGNAL( metaDataChanged(QVariantMap const&) ), this, SLOT( onMetaDataChanged(QVariantMap const&) ) );
		connect( m_mp, SIGNAL( playbackCompleted() ), this, SIGNAL( playbackCompleted() ) );
        connect( m_mp, SIGNAL( positionChanged(unsigned int) ), this, SIGNAL( positionChanged(unsigned int) ) );
        connect( m_mp, SIGNAL( trackChanged(unsigned int) ), this, SLOT( trackChanged(unsigned int) ) );
		connect( m_mp, SIGNAL( videoDimensionsChanged(QSize const&) ), this, SIGNAL( videoDimensionsChanged(QSize const&) ) );

		if ( !m_videoWindowId.isNull() ) {
			m_mp->setWindowId(m_videoWindowId);
			m_mp->setVideoOutput(VideoOutput::PrimaryDisplay);
		}
	}

    if (m_mp) {
        m_mp->reset();
    }

    m_mp->setSourceUrl(uri);

    if ( m_npc->isAcquired() ) {
        playNow();
    } else {
        MediaError::Type errType = m_npc->acquire();
        LOGGER(errType);

        if (errType != MediaError::None) {
            error(errType, 0);
        }
    }
}


void LazyMediaPlayer::error(bb::multimedia::MediaError::Type mediaError, unsigned int position)
{
    LOGGER(mediaError << position);

    QString message = tr("A DRM related error was encountered.");

    switch (mediaError)
    {
        case MediaError::Internal:
            message = tr("An unexpected internal error has occured!");
            break;
        case MediaError::InvalidParameter:
            message = tr("An invalid parameter was specified!");
            break;
        case MediaError::InvalidState:
            message = tr("An illegal operation given the context state.");
            break;
        case MediaError::UnsupportedValue:
            message = tr("An unrecognized input or output type or an out-of-range speed setting encountered.");
            break;
        case MediaError::UnsupportedMediaType:
            message = tr("A data format not recognized by any plugin encountered.");
            break;
        case MediaError::DrmProtected:
            message = tr("A DRM protected input was specified.");
            break;
        case MediaError::UnsupportedOperation:
            message = tr("An illegal operation was encountered by the playback process.");
            break;
        case MediaError::Read:
            message = tr("An I/O error has been encountered at the source!");
            break;
        case MediaError::Write:
            message = tr("An I/O error has been encountered at the sink!");
            break;
        case MediaError::SourceUnavailable:
            message = tr("The source is currently unavailable.");
            break;
        case MediaError::ResourceCorrupted:
            message = tr("The resource was corrupted.");
            break;
        case MediaError::OutputUnavailable:
            message = tr("Cannot open the sink (possibly because no plugin recognizes it).");
            break;
        case MediaError::OutOfMemory:
            message = tr("Insufficient memory to perform the requested operation.");
            break;
        case MediaError::ResourceUnavailable:
            message = tr("A required resource such as an encoder or an output feed is presently unavailable.");
            break;
        case MediaError::Permission:
            message = tr("A playback permission error (e.g., user prohibition, region mismatch) error was encountered.");
            break;
        default:
            break;
    }

    emit error(message);
}


void LazyMediaPlayer::onMetaDataChanged(QVariantMap const& metaData)
{
    m_npc->setMetaData(metaData);
    emit metaDataChanged(metaData);
}


void LazyMediaPlayer::playNow()
{
    MediaError::Type errType = m_mp->play();
    LOGGER(errType);

    if (errType != MediaError::None) {
        error(errType, 0);
    }
}


void LazyMediaPlayer::trackChanged(unsigned int track)
{
    int converted = track-1;
    emit currentIndexChanged(converted);
}


void LazyMediaPlayer::mediaStateChanged(bb::multimedia::MediaState::Type mediaState)
{
    m_npc->setMediaState(mediaState);

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
    bool changed = m_repeat != value;
    m_repeat = value;

    if (m_mp) {
        m_mp->setRepeatMode(value ? RepeatMode::All : RepeatMode::None);
    }

    if (changed) {
        emit repeatChanged();
    }
}


QVariantMap LazyMediaPlayer::metaData() const {
	return m_mp ? m_mp->metaData() : QVariantMap();
}


MediaPlayer* LazyMediaPlayer::mediaPlayer() {
	return m_mp;
}


void LazyMediaPlayer::setVolume(double volume)
{
    m_volume = volume;
    LOGGER("setVol" << m_volume);

    if (m_mp) {
        m_mp->setVolume(volume);
    }
}


double LazyMediaPlayer::volume() {
    return m_mp ? m_mp->volume() : 0;
}


int LazyMediaPlayer::equalizer() const {
    return m_mp ? m_mp->property("equalizerPreset").toInt() : 0;
}


void LazyMediaPlayer::setEqualizer(int preset)
{
    LOGGER(preset);
    m_mp->setProperty("equalizerPreset", preset);
}


LazyMediaPlayer::~LazyMediaPlayer()
{
	if (m_npc != NULL) {
		m_npc->revoke();
	}
}

} /* namespace canadainc */
