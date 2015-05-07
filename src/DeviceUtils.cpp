#include "DeviceUtils.h"
#include "AppLogFetcher.h"
#include "Logger.h"

#include <bb/cascades/ActionItem>
#include <bb/cascades/ListView>
#include <bb/cascades/Page>
#include <bb/cascades/QmlDocument>

#include <bb/device/DisplayInfo>
#include <bb/device/HardwareInfo>

namespace canadainc {

using namespace bb::cascades;
using namespace bb::device;

DeviceUtils* DeviceUtils::instance = NULL;

DeviceUtils::DeviceUtils(QObject* parent) :
        QObject(parent), m_hw(NULL), m_display(NULL)
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


void DeviceUtils::attachTopBottomKeys(bb::cascades::Page* page, bb::cascades::ListView* listView, bool onBar)
{
    if ( !isPhysicalKeyboardDevice() )
    {
        AbstractActionItem* top = ActionItem::create().title( tr("Top") ).imageSource( QUrl("asset:///images/menu/ic_top.png") ).onTriggered( this, SLOT( onTopTriggered() ) );
        connect( top, SIGNAL( destroyed(QObject*) ), this, SLOT( onDestroyed(QObject*) ) );

        AbstractActionItem* bottom = ActionItem::create().title( tr("Bottom") ).imageSource( QUrl("asset:///images/menu/ic_bottom.png") ).onTriggered( this, SLOT( onBottomTriggered() ) );
        connect( bottom, SIGNAL( destroyed(QObject*) ), this, SLOT( onDestroyed(QObject*) ) );

        page->addAction(top, onBar ? ActionBarPlacement::OnBar : ActionBarPlacement::Default);
        page->addAction(bottom, onBar ? ActionBarPlacement::OnBar : ActionBarPlacement::Default);

        m_actionToList[top] = listView;
        m_actionToList[bottom] = listView;
    }

    listView->setScrollRole(ScrollRole::Main);
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


DeviceUtils* DeviceUtils::create(QObject* parent)
{
    if (!instance) {
        instance = new DeviceUtils(parent);
    }

    return instance;
}


bool DeviceUtils::isEqual(bb::cascades::Page* p1, bb::cascades::Page* p2) {
    return p1 == p2;
}


DeviceUtils::~DeviceUtils()
{
}

} /* namespace canadainc */
