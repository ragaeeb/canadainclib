#ifndef ACCOUNTMANAGER_H_
#define ACCOUNTMANAGER_H_

#include <QObject>

namespace canadainc {

class Persistance;

class AccountManager : public QObject
{
	Q_OBJECT

	Q_PROPERTY(bool authenticated READ authenticated NOTIFY authenticatedChanged)

	Persistance* m_persistance;
	bool m_authenticated;

Q_SIGNALS:
	void authenticatedChanged();

public:
	AccountManager(Persistance* persistance);

	Q_INVOKABLE bool login(QString password);
	Q_INVOKABLE void savePassword(QString password);
	Q_INVOKABLE bool accountCreated() const;

	bool authenticated() const;
};

} /* namespace canadainc */
#endif /* ACCOUNTMANAGER_H_ */
