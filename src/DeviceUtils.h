#ifndef DEVICEUTILS_H_
#define DEVICEUTILS_H_

#include <QObject>
#include <QMap>
#include <QSize>

#include <bb/cascades/ScrollPosition>

namespace bb {
    namespace cascades {
        class ListView;
        class Page;
    }

    namespace device {
        class DisplayInfo;
        class HardwareInfo;
    }
}

namespace canadainc {

using namespace bb::cascades;
using namespace bb::device;

class DeviceUtils : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSize pixelSize READ pixelSize FINAL)
    Q_PROPERTY(bool isPhysicalKeyboardDevice READ isPhysicalKeyboardDevice FINAL)

    static DeviceUtils* instance;
    HardwareInfo* m_hw;
    DisplayInfo* m_display;
    QMap<QObject*, QObject*> m_actionToList;
    int m_factor;

    void processDirection(ScrollPosition::Type);

private slots:
    void onBottomTriggered();
    void onDestroyed(QObject* obj);
    void onTopTriggered();

public:
    DeviceUtils(QObject* parent=NULL);
    virtual ~DeviceUtils();

    Q_INVOKABLE void attachTopBottomKeys(bb::cascades::Page* page, bb::cascades::ListView* listView, bool onBar=false);
    QSize pixelSize();
    bool isPhysicalKeyboardDevice();

    /**
     * This method is needed because QML sometimes has issues comparing Page objects since one of them happens to be a QVariant(QMLPage)
     * and the other is an actual Page instance. This should usually be used when comparing popTransitionEnded(page) with an existing page.
     */
    Q_INVOKABLE bool isEqual(bb::cascades::Page* p1, bb::cascades::Page* p2);

    Q_INVOKABLE void cleanUpAndDestroy(QObject* q);

    static void registerTutorialTips(QObject* parent);
    Q_INVOKABLE void log(QVariant const& message, QObject* q=NULL);
    Q_INVOKABLE qreal du(qreal units);
};

} /* namespace canadainc */

#endif /* DEVICEUTILS_H_ */
