#ifndef ACCOUNTIMPORTER_H_
#define ACCOUNTIMPORTER_H_

#include <QObject>
#include <QRunnable>
#include <QVariantList>

#include <bb/pim/account/Service>

namespace canadainc {

using namespace bb::pim::account;

class AccountImporter : public QObject, public QRunnable
{
	Q_OBJECT

	Service::Type m_type;

signals:
	void importCompleted(QVariantList const& qvl);

public:
	AccountImporter(Service::Type type=Service::Messages);
	virtual ~AccountImporter();
	void run();
};

} /* namespace canadainc */
#endif /* ACCOUNTIMPORTER_H_ */
