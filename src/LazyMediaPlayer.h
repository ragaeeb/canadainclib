#ifndef LAZYMEDIAPLAYER_H_
#define LAZYMEDIAPLAYER_H_

#include <QObject>
#include <QSize>
#include <QVariantMap>

#include <bb/multimedia/MediaState>

namespace bb {
	namespace multimedia {
		class MediaPlayer;
		class NowPlayingConnection;
	}
}

namespace canadainc {

using namespace bb::multimedia;

class LazyMediaPlayer : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int currentIndex READ currentIndex)
	Q_PROPERTY(int count READ count)
	Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)
	Q_PROPERTY(bool active READ active NOTIFY activeChanged)
	Q_PROPERTY(QSize videoDimensions READ videoDimensions)
	Q_PROPERTY(QVariant position READ currentPosition)
	Q_PROPERTY(QVariantMap metaData READ metaData)

	QString m_name;
	MediaPlayer* m_mp;
	NowPlayingConnection* m_npc;
	QStringList m_playlist;
	int m_currentIndex;
	QString m_videoWindowId;

private slots:
	void playbackCompleted();
	void mediaStateChanged(bb::multimedia::MediaState::Type mediaState);

signals:
	void playlistCompleted();
	void positionChanged(unsigned int position);
	void durationChanged(unsigned int duration);
	void videoDimensionsChanged(const QSize &videoDimensions);
	void playingChanged();
	void activeChanged();
	void metaDataChanged(QVariantMap const& metaData);

public:
	LazyMediaPlayer(QObject* parent=NULL);
	virtual ~LazyMediaPlayer();

	bool playing() const;
	bool active() const;
	QVariantMap metaData() const;
	int currentIndex() const;
	int count() const;
	QVariant currentPosition() const;
	QSize videoDimensions() const;
	Q_INVOKABLE void setName(QString const& name);
	Q_INVOKABLE void play(QStringList const& playlist);
	Q_INVOKABLE void play(QString const& uri);
	Q_SLOT void togglePlayback();
	Q_INVOKABLE void seek(unsigned int position);
	Q_INVOKABLE void jump(int secs);
	Q_INVOKABLE void skip(int n);
	Q_SLOT void stop();
	Q_SLOT void pause();
	Q_INVOKABLE QString videoWindowId();
	MediaPlayer* mediaPlayer();

	Q_INVOKABLE void setVideoWindowId(QString const& windowId);
	Q_SLOT void toggleVideo();
};

} /* namespace canadainc */
#endif /* LazyMediaPlayer_H_ */
