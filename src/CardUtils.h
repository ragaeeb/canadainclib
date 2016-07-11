#ifndef CARDUTILS_H_
#define CARDUTILS_H_

#include <QMap>
#include <QObject>

#define CARD_KEY "logCard"

namespace canadainc {

class CardUtils
{
public:
    static QObject* initAppropriate(QString const& qml, QMap<QString, QObject*> context, QObject* parent, bool qrc=false);
};

} /* namespace canadainc */

#endif /* CARDUTILS_H_ */
