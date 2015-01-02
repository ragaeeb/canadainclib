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

private slots:
	void onThumbnail();
	void tearDown();

public:
	LazySceneCover(QString const& sceneCoverQml="Cover.qml", bool cycle=false);
	virtual ~LazySceneCover();

	void setContext(QString const& key, QObject* value);
};

} /* namespace canadainc */
#endif /* LAZYSCENECOVER_H_ */
