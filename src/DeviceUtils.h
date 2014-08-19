#ifndef DEVICEUTILS_H_
#define DEVICEUTILS_H_

#include <QObject>
#include <QMap>

#include <bb/cascades/ScrollPosition>

namespace bb {
    namespace device {
        class HardwareInfo;
    }
}

namespace canadainc {

    using namespace bb::cascades;
using namespace bb::device;

class DeviceUtils : public QObject
{
    Q_OBJECT

    DeviceUtils(QObject* parent);
    HardwareInfo* m_hw;
    QMap<QObject*, QObject*> m_actionToList;

    void lazyInit();
    void processDirection(ScrollPosition::Type);

private slots:
    void onBottomTriggered();
    void onDestroyed(QObject* obj);
    void onTopTriggered();

public:
    static DeviceUtils* create(QObject* parent=NULL);
    virtual ~DeviceUtils();

    Q_INVOKABLE void attachTopBottomKeys(QObject* page, QObject* listView, bool onBar=false);
};

} /* namespace canadainc */

#endif /* DEVICEUTILS_H_ */
