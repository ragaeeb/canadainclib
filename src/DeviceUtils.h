#ifndef DEVICEUTILS_H_
#define DEVICEUTILS_H_

#include <QObject>
#include <QMap>
#include <QSize>

#include <bb/cascades/ScrollPosition>

namespace bb {
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

    DeviceUtils(QObject* parent);
    void processDirection(ScrollPosition::Type);

private slots:
    void onBottomTriggered();
    void onDestroyed(QObject* obj);
    void onTopTriggered();

public:
    static DeviceUtils* create(QObject* parent=NULL);
    virtual ~DeviceUtils();

    Q_INVOKABLE void attachTopBottomKeys(QObject* page, QObject* listView, bool onBar=false);
    QSize pixelSize();
    bool isPhysicalKeyboardDevice();
};

} /* namespace canadainc */

#endif /* DEVICEUTILS_H_ */
