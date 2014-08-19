#include "DeviceUtils.h"
#include "Logger.h"

#include <bb/cascades/ActionItem>
#include <bb/cascades/ListView>
#include <bb/cascades/Page>
#include <bb/cascades/QmlDocument>

#include <bb/device/HardwareInfo>

namespace canadainc {

using namespace bb::cascades;
using namespace bb::device;

DeviceUtils::DeviceUtils(QObject* parent) : QObject(parent), m_hw(NULL)
{
    QDeclarativeContext* rootContext = QmlDocument::defaultDeclarativeEngine()->rootContext();
    rootContext->setContextProperty("deviceUtils", this);
}


void DeviceUtils::lazyInit()
{
    if (!m_hw) {
        m_hw = new HardwareInfo(this);
    }
}


void DeviceUtils::attachTopBottomKeys(QObject* p, QObject* listView, bool onBar)
{
    lazyInit();

    if ( !m_hw->isPhysicalKeyboardDevice() )
    {
        AbstractActionItem* top = ActionItem::create().title( tr("Top") ).imageSource( QUrl("asset:///images/menu/ic_top.png") ).onTriggered( this, SLOT( submitUpdates() ) );
        connect( top, SIGNAL( destroyed(QObject*) ), this, SLOT( onDestroyed(QObject*) ) );

        AbstractActionItem* bottom = ActionItem::create().title( tr("Bottom") ).imageSource( QUrl("asset:///images/menu/ic_bottom.png") ).onTriggered( this, SLOT( uploadUpdates() ) );
        connect( bottom, SIGNAL( destroyed(QObject*) ), this, SLOT( onDestroyed(QObject*) ) );

        Page* page = static_cast<Page*>(p);
        page->addAction(top, onBar ? ActionBarPlacement::OnBar : ActionBarPlacement::Default);
        page->addAction(bottom, onBar ? ActionBarPlacement::OnBar : ActionBarPlacement::Default);

        m_actionToList[top] = listView;
        m_actionToList[bottom] = listView;

        connect( bottom, SIGNAL( triggered() ), this, SLOT( onBottomTriggered() ) );
        connect( top, SIGNAL( triggered() ), this, SLOT( onTopTriggered() ) );
    }
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
}


void DeviceUtils::onTopTriggered()
{
    LOGGER("UserEvent: TopTriggered");
    processDirection(ScrollPosition::Beginning);
}


DeviceUtils* DeviceUtils::create(QObject* parent) {
    return new DeviceUtils(parent);
}


DeviceUtils::~DeviceUtils()
{
}

} /* namespace canadainc */
