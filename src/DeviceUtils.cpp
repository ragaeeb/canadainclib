#include "bbndk.h"
#include "DeviceUtils.h"
#include "AppLogFetcher.h"
#include "Logger.h"

#include <bb/cascades/ActionItem>
#include <bb/cascades/Application>
#include <bb/cascades/ListView>
#include <bb/cascades/Page>
#include <bb/cascades/QmlDocument>

#include <bb/device/DisplayInfo>
#include <bb/device/HardwareInfo>

#define CLEANUP_FUNC "cleanUp"

namespace canadainc {

using namespace bb::cascades;
using namespace bb::device;

DeviceUtils::DeviceUtils(QObject* parent) :
        QObject(parent), m_hw(NULL), m_display(NULL), m_factor(0)
{
    QDeclarativeContext* rootContext = QmlDocument::defaultDeclarativeEngine()->rootContext();
    rootContext->setContextProperty("deviceUtils", this);
}


QSize DeviceUtils::pixelSize()
{
    if (!m_display) {
        m_display = new DisplayInfo(this);
    }

    return m_display->pixelSize();
}


void DeviceUtils::attachTopBottomKeys(QObject* p, QObject* listView, bool onBar)
{
    if ( !isPhysicalKeyboardDevice() )
    {
        AbstractActionItem* top = ActionItem::create().title( tr("Top") ).imageSource( QUrl("asset:///images/common/ic_top.png") ).onTriggered( this, SLOT( onTopTriggered() ) );
        connect( top, SIGNAL( destroyed(QObject*) ), this, SLOT( onDestroyed(QObject*) ) );

        AbstractActionItem* bottom = ActionItem::create().title( tr("Bottom") ).imageSource( QUrl("asset:///images/common/ic_bottom.png") ).onTriggered( this, SLOT( onBottomTriggered() ) );
        connect( bottom, SIGNAL( destroyed(QObject*) ), this, SLOT( onDestroyed(QObject*) ) );

        Page* page = static_cast<Page*>(p);
        page->addAction(top, onBar ? ActionBarPlacement::OnBar : ActionBarPlacement::Default);
        page->addAction(bottom, onBar ? ActionBarPlacement::OnBar : ActionBarPlacement::Default);

        m_actionToList[top] = listView;
        m_actionToList[bottom] = listView;
    }

    static_cast<ListView*>(listView)->setScrollRole(ScrollRole::Main);
}


bool DeviceUtils::isPhysicalKeyboardDevice()
{
    if (!m_hw) {
        m_hw = new HardwareInfo(this);
    }

    return m_hw->isPhysicalKeyboardDevice();
}


void DeviceUtils::onDestroyed(QObject* obj) {
    m_actionToList.remove(obj);
}


void DeviceUtils::processDirection(ScrollPosition::Type p)
{
    QObject* l = m_actionToList.value( sender() );

    if (l)
    {
        ListView* listView = static_cast<ListView*>(l);
        listView->scrollToPosition(p, ScrollAnimation::None);
    }
}


void DeviceUtils::onBottomTriggered()
{
    LOGGER("UserEvent: BottomTriggered");
    processDirection(ScrollPosition::End);

    AppLogFetcher::getInstance()->record("BottomTriggered");
}


void DeviceUtils::onTopTriggered()
{
    LOGGER("UserEvent: TopTriggered");
    processDirection(ScrollPosition::Beginning);

    AppLogFetcher::getInstance()->record("TopTriggered");
}


bool DeviceUtils::isEqual(bb::cascades::Page* p1, bb::cascades::Page* p2) {
    return p1 == p2;
}


void DeviceUtils::cleanUpAndDestroy(QObject* q)
{
    QMetaObject::invokeMethod(q, CLEANUP_FUNC, Qt::QueuedConnection);
    q->deleteLater();
}


void DeviceUtils::registerTutorialTips(QObject* parent)
{
    QmlDocument* qml = QmlDocument::create("asset:///TutorialTip.qml").parent(parent);
    QmlDocument::defaultDeclarativeEngine()->rootContext()->setContextProperty( "tutorial", qml->createRootObject<QObject>() );
}


void DeviceUtils::log(QVariant const& message, QObject* q)
{
    if (!q) {
        if ( message.type() == QVariant::String ) {
            LOGGER( message.toString() );
        } else if ( message.type() == QVariant::Double ) {
            LOGGER( message.toReal() );
        } else if ( message.type() == QVariant::Int ) {
            LOGGER( message.toLongLong() );
        } else {
            LOGGER(message);
        }
    } else {
        if ( message.type() == QVariant::String ) {
            LOGGER( q << message.toString() );
        } else if ( message.type() == QVariant::Double ) {
            LOGGER( q << message.toReal() );
        } else if ( message.type() == QVariant::Int ) {
            LOGGER( q << message.toLongLong() );
        } else {
            LOGGER(q << message);
        }
    }
}


bool DeviceUtils::isValidPhoneNumber(QString const& phone)
{
    QRegExp regex("^(([^<>()[\]\\.,;:\s@\"]+(\.[^<>()[\]\\.,;:\s@\"]+)*)|(\".+\"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$");
    return regex.exactMatch(phone);
}


bool DeviceUtils::isValidEmail(QString const& email)
{
    QRegExp regex("^(?:(?:\(?(?:00|\+)([1-4]\d\d|[1-9]\d?)\)?)?[\-\.\ \\\/]?)?((?:\(?\d{1,}\)?[\-\.\ \\\/]?){0,})(?:[\-\.\ \\\/]?(?:#|ext\.?|extension|x)[\-\.\ \\\/]?(\d+))?$");
    regex.setCaseSensitivity(Qt::CaseInsensitive);

    return regex.exactMatch(email);
}


bool DeviceUtils::isUrl(QString const& input) {
    return input.startsWith("http://") || input.startsWith("https://");
}


DeviceUtils::~DeviceUtils()
{
}

} /* namespace canadainc */
