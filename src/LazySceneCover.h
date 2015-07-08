#ifndef LAZYSCENECOVER_H_
#define LAZYSCENECOVER_H_

#include <QHash>
#include <QObject>
#include <QString>

namespace canadainc {

class LazySceneCover : public QObject
{
	Q_OBJECT

	QHash<QString, QObject*> m_context;
	bool m_ready;

private slots:
    void onReady();
	void onThumbnail();

public:
	LazySceneCover(bool appLaunch, QObject* ui);
	virtual ~LazySceneCover();

	void setContext(QString const& key, QObject* value);
};

} /* namespace canadainc */
#endif /* LAZYSCENECOVER_H_ */
