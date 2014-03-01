#include <QtLocationSubset/QGeoPositionInfoSource>
#include <QVariant>

#include "GeoLocationReader.h"
#include "Logger.h"

namespace canadainc {

using namespace QtMobilitySubset;

GeoLocationReader::GeoLocationReader(QObject* parent) : QObject(parent), m_src(NULL)
{
}


bool GeoLocationReader::requestUpdate()
{
	if (m_src == NULL)
	{
	    m_src = QGeoPositionInfoSource::createDefaultSource(this);
	    m_src->setPreferredPositioningMethods(QGeoPositionInfoSource::SatellitePositioningMethods);
	    connect( m_src, SIGNAL( positionUpdated(const QGeoPositionInfo&) ), this, SLOT( positionUpdated(const QGeoPositionInfo&) ) );
	}

	m_src->requestUpdate();

	return m_src->property("locationServicesEnabled").toBool();
}


void GeoLocationReader::positionUpdated(const QGeoPositionInfo& update)
{
	Q_UNUSED(update);

	LOGGER( "latitude" << update.coordinate().latitude() );
	LOGGER( "longitude" << update.coordinate().longitude() );

	emit coordinatesChanged();
}


QPointF GeoLocationReader::coordinates() const {
	return m_src ? QPointF( m_src->lastKnownPosition().coordinate().latitude(), m_src->lastKnownPosition().coordinate().longitude() ) : QPointF();
}


QGeoCoordinate GeoLocationReader::position() const {
	return m_src ? m_src->lastKnownPosition().coordinate() : QGeoCoordinate();
}

} /* namespace oct10 */
