#include "ReverseGeolocator.h"
#include "Logger.h"

#include <QStringList>

#include <QtLocationSubset/QGeoServiceProvider>
#include <QtLocationSubset/QGeoSearchManager>
#include <QtLocationSubset/QGeoSearchReply>

namespace canadainc {

using namespace QtMobilitySubset;

ReverseGeolocator::ReverseGeolocator(QObject* parent) : QObject(parent), m_serviceProvider(NULL)
{
	connect( &m_reader, SIGNAL( coordinatesChanged() ), this, SLOT( coordinatesChanged() ) );
}


void ReverseGeolocator::coordinatesChanged()
{
	LOGGER("========= New coordinates" << m_reader.coordinates() );

	QStringList serviceProviders = QGeoServiceProvider::availableServiceProviders();

	if ( serviceProviders.size() )
	{
		LOGGER("======== AVAILABLE!!!!!!!" << serviceProviders.size() );

		if (!m_serviceProvider) {
			m_serviceProvider = new QGeoServiceProvider( serviceProviders.first() );
		}

	    QGeoSearchManager* searchManager = m_serviceProvider->searchManager();

	    // Request reverse geocoding
	    QGeoSearchReply* reply = searchManager->reverseGeocode( m_reader.position() );
	    connect( reply, SIGNAL( finished() ), this, SLOT( readReverseGeocode() ) );
	} else {
		LOGGER("======== NOTHING AVAILABLE!!!!!!!");
	}
}


bool ReverseGeolocator::locate()
{
	LOGGER("===== LOCATE");
	return m_reader.requestUpdate();
}


void ReverseGeolocator::readReverseGeocode()
{
	LOGGER("========= Ready for reversal");
	QGeoSearchReply* reply = static_cast<QGeoSearchReply*>( sender() );

	QList<QGeoPlace> places = reply->places();
	LOGGER("====== PLACES" << places.size());

	if ( !places.isEmpty() )
	{
		QGeoAddress add = places.first().address();
		LOGGER("======= DETAILS" << add.text());

		emit finished( add.text(), m_reader.coordinates() );
	} else {
		emit finished( tr("No matches found for phone coordinates..."), m_reader.coordinates(), true );
	}

	delete reply;
}


ReverseGeolocator::~ReverseGeolocator() {
	delete m_serviceProvider;
}

} /* namespace canadainc */
