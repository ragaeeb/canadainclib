#ifndef GEOLOCATIONREADER_H_
#define GEOLOCATIONREADER_H_

#include <QObject>
#include <QPointF>

#include <QtLocationSubset/QGeoCoordinate>

namespace QtMobilitySubset {
	class QGeoPositionInfo;
	class QGeoPositionInfoSource;
}

namespace canadainc {

using namespace QtMobilitySubset;

class GeoLocationReader : public QObject
{
	Q_OBJECT
    Q_PROPERTY(QPointF coordinates READ coordinates NOTIFY coordinatesChanged)

	QGeoPositionInfoSource* m_src;

Q_SIGNALS:
	void coordinatesChanged();

private slots:
	void positionUpdated(const QGeoPositionInfo& update);

public:
	GeoLocationReader(QObject* parent=NULL);
	virtual ~GeoLocationReader() {};
	Q_INVOKABLE bool requestUpdate();

	QPointF coordinates() const;
	QGeoCoordinate position() const;
};

} /* namespace oct10 */
#endif /* GEOLOCATIONREADER_H_ */
