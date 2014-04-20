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
	Q_PROPERTY(int count READ count FINAL)
	Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)
	Q_PROPERTY(bool active READ active NOTIFY activeChanged)
	Q_PROPERTY(QSize videoDimensions READ videoDimensions)
	Q_PROPERTY(QVariant position READ currentPosition)
	Q_PROPERTY(QVariantMap metaData READ metaData)
	Q_PROPERTY(bool repeat READ repeat WRITE setRepeat FINAL)

	QString m_name;
	MediaPlayer* m_mp;
	NowPlayingConnection* m_npc;
	QString m_videoWindowId;
	bool m_repeat;

private slots:
	void mediaStateChanged(bb::multimedia::MediaState::Type mediaState);
	void trackChanged(unsigned int track);

signals:
    void activeChanged();
    void durationChanged(unsigned int duration);
    void metaDataChanged(QVariantMap const& metaData);
    void playingChanged();
	void positionChanged(unsigned int position);
	void currentIndexChanged(int index);
	void playbackCompleted();
	void videoDimensionsChanged(const QSize &videoDimensions);

public:
	LazyMediaPlayer(QObject* parent=NULL);
	virtual ~LazyMediaPlayer();

	bool playing() const;
	bool active() const;
	bool repeat() const;
	QVariantMap metaData() const;
	int currentIndex() const;
	int count() const;
	QVariant currentPosition() const;
	QSize videoDimensions() const;
	Q_INVOKABLE void setName(QString const& name);
	Q_INVOKABLE void play(QUrl const& uri);
	Q_SLOT void togglePlayback();
	Q_INVOKABLE void seek(unsigned int position);
	Q_INVOKABLE void jump(int secs);
	Q_INVOKABLE void skip(int n);
	Q_SLOT void stop();
	Q_SLOT void pause();
	Q_INVOKABLE QString videoWindowId();
	MediaPlayer* mediaPlayer();
	void setRepeat(bool value);

	Q_INVOKABLE void setVideoWindowId(QString const& windowId);
	Q_SLOT void toggleVideo();
};

} /* namespace canadainc */
#endif /* LazyMediaPlayer_H_ */
