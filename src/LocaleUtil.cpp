#include "LocaleUtil.h"
#include "Logger.h"

#include <bb/system/LocaleHandler>

namespace canadainc {

LocaleUtil::LocaleUtil(QString appName, QStringList libs)
{
    if ( appName.isNull() ) {
        appName = QCoreApplication::applicationName();
    }

    libs << "canadainc";
    libs << appName;

    foreach (QString const& lib, libs) {
        m_translators[lib] = new QTranslator();
    }

    connect( &m_pLocaleHandler, SIGNAL( systemLanguageChanged() ), this, SLOT( onSystemLanguageChanged() ) );
    onSystemLanguageChanged();
}


void LocaleUtil::onSystemLanguageChanged()
{
    QStringList keys = m_translators.keys();

    m_currentLocale = QLocale().name(); // Initiate, load and install the application translation files.

    foreach (QString const& key, keys)
    {
        QTranslator* q = m_translators.value(key);
        QCoreApplication::instance()->removeTranslator(q);

        QString fileName = QString("%1_%2").arg(key).arg(m_currentLocale);

        if ( q->load(fileName, "app/native/qm") ) {
            QCoreApplication::instance()->installTranslator(q);
        } else {
            LOGGER("LoadFailed" << fileName);
        }
    }
}


QString LocaleUtil::locale() const {
    return m_currentLocale;
}


LocaleUtil::~LocaleUtil()
{
}


} /* namespace canadainc */
