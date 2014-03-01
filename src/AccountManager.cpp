#include "AccountManager.h"
#include "Persistance.h"

namespace {

QString encrypt(QString input) {
	return QString( QCryptographicHash::hash( input.toLocal8Bit(), QCryptographicHash::Md5).toHex() );
}

}

namespace canadainc {

AccountManager::AccountManager(Persistance* persistance) : m_persistance(persistance), m_authenticated(false)
{
}

bool AccountManager::login(QString password)
{
	password = encrypt(password);

	QString actual = m_persistance->getValueFor("password").toString();
	bool prevAuthenticated = m_authenticated;

	m_authenticated = actual == password;

	if (prevAuthenticated != m_authenticated) {
		emit authenticatedChanged();
	}

	return m_authenticated;
}


bool AccountManager::accountCreated() const {
	return m_persistance->contains("password");
}


bool AccountManager::authenticated() const {
	return m_authenticated;
}


void AccountManager::savePassword(QString password)
{
	password = encrypt(password);
	m_persistance->saveValueFor("password", password);
}

} /* namespace canadainc */
