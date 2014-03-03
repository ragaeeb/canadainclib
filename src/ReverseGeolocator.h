#ifndef REVERSEGEOLOCATOR_H_
#define REVERSEGEOLOCATOR_H_

#include "GeoLocationReader.h"

namespace QtMobilitySubset {
	class QGeoServiceProvider;
}

namespace canadainc {

using namespace QtMobilitySubset;

class ReverseGeolocator : public QObject
{
	Q_OBJECT

	GeoLocationReader m_reader;
	QGeoServiceProvider* m_serviceProvider;

private slots:
	void coordinatesChanged();
	void readReverseGeocode();

Q_SIGNALS:
	void finished(QString const& location, QString const& city, QPointF const& point, bool error=false);

public:
	ReverseGeolocator(QObject* parent=NULL);
	virtual ~ReverseGeolocator();

	Q_SLOT bool locate();
};

} /* namespace canadainc */
#endif /* ReverseGeolocator_H_ */
