#ifndef LAZYMEDIAPLAYER_H_
#define LAZYMEDIAPLAYER_H_

#include <QObject>
#include <QSize>
#include <QVariantMap>

#include <bb/multimedia/MediaError>
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
	Q_PROPERTY(bool repeat READ repeat WRITE setRepeat NOTIFY repeatChanged)
	Q_PROPERTY(bool multiThreaded READ multiThreaded WRITE setMultiThreaded FINAL)

	QString m_name;
	MediaPlayer* m_mp;
	NowPlayingConnection* m_npc;
	QString m_videoWindowId;
	bool m_repeat;
	double m_volume;
	bool m_multiThreaded;

	void doPlayback(QUrl const& uri);

private slots:
    void error(bb::multimedia::MediaError::Type mediaError, unsigned int position);
	void mediaStateChanged(bb::multimedia::MediaState::Type mediaState);
    void playNow();
	void trackChanged(unsigned int track);

signals:
    void activeChanged();
    void currentIndexChanged(int index);
    void durationChanged(unsigned int duration);
    void error(QString const& message);
    void metaDataChanged(QVariantMap const& metaData);
    void playbackCompleted();
    void playingChanged();
	void positionChanged(unsigned int position);
    void repeatChanged();
	void videoDimensionsChanged(const QSize &videoDimensions);

public:
	LazyMediaPlayer(bool multiThreaded=false, QObject* parent=NULL);
	virtual ~LazyMediaPlayer();

    bool active() const;
    bool playing() const;
    bool repeat() const;
    int count() const;
    int currentIndex() const;
    MediaPlayer* mediaPlayer();
    QSize videoDimensions() const;
    QVariant currentPosition() const;
    QVariantMap metaData() const;
    void setRepeat(bool value);
    double volume();
    bool multiThreaded() const;

    Q_INVOKABLE QString videoWindowId();
    Q_INVOKABLE void jump(int secs);
    Q_INVOKABLE void play(QUrl const& uri);
    Q_INVOKABLE void seek(unsigned int position);
    Q_INVOKABLE void setName(QString const& name);
    Q_INVOKABLE void setVideoWindowId(QString const& windowId);
    Q_INVOKABLE void setVolume(double volume);
    Q_INVOKABLE void skip(int n);
    Q_SLOT void pause();
    Q_SLOT void stop();
    Q_SLOT void togglePlayback();
    Q_SLOT void toggleVideo();
    void setMultiThreaded(bool threaded);
};

} /* namespace canadainc */
#endif /* LazyMediaPlayer_H_ */
