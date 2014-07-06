#ifndef CARDUTILS_H_
#define CARDUTILS_H_

#include <QMap>
#include <QObject>

namespace canadainc {

class CardUtils
{
public:
    static QObject* initAppropriate(QString const& qml, QMap<QString, QObject*> const& context, QObject* parent);
};

} /* namespace canadainc */

#endif /* CARDUTILS_H_ */
