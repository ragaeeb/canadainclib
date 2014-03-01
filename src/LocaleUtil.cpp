#include "LocaleUtil.h"
#include "Logger.h"

#include <QDateTime>

#include <bb/utility/i18n/Formatting>
#include <bb/system/LocaleHandler>

namespace canadainc {

LocaleUtil::LocaleUtil(QString const& appName, QObject* parent) : QObject(parent), m_appName(appName)
{
    if ( !connect( &m_pLocaleHandler, SIGNAL( systemLanguageChanged() ), this, SLOT( onSystemLanguageChanged() ) ) ) {
        LOGGER("Recovering from a failed connect()");
    } else {
        onSystemLanguageChanged();
    }
}


void LocaleUtil::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(&m_pTranslator);

    // Initiate, load and install the application translation files.
    QString localeString = QLocale().name();

    if ( m_appName.isNull() ) {
    	m_appName = QCoreApplication::applicationName();
    }

    QString fileName = QString("%1_%2").arg(m_appName).arg(localeString);

    LOGGER("Locale file name: " << fileName);

    if ( m_pTranslator.load(fileName, "app/native/qm") ) {
        QCoreApplication::instance()->installTranslator(&m_pTranslator);
    }
}


LocaleUtil::~LocaleUtil()
{
}


QString LocaleUtil::renderStandardTime(QDateTime const& theTime)
{
    static QString format = bb::utility::i18n::timeFormat(bb::utility::i18n::DateFormat::Short);
    bb::system::LocaleHandler region(bb::system::LocaleType::Region);

    return region.locale().toString(theTime, format);
}

} /* namespace canadainc */
