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

	m_cached = QPointF( update.coordinate().latitude(), update.coordinate().longitude() );
    LOGGER( "latitude,longitude" << m_cached );
	emit coordinatesChanged();
}


QPointF GeoLocationReader::coordinates()
{
    if ( m_cached.isNull() && m_src )
    {
        QGeoCoordinate coordinate = position();
        m_cached = QPointF( coordinate.latitude(), coordinate.longitude() );
    }

    return m_cached;
}


QGeoCoordinate GeoLocationReader::position() const {
	return m_src ? m_src->lastKnownPosition().coordinate() : QGeoCoordinate();
}

} /* namespace oct10 */
