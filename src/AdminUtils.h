#ifndef ADMINUTILS_H_
#define ADMINUTILS_H_

#include <QObject>
#include <QStringList>
#include <QUrl>

#define KEY_ADMIN_MODE "adminMode"
#define KEY_ADMIN_PASSWORD "adminPassword"
#define KEY_ADMIN_USERNAME "adminUsername"

namespace bb {
    namespace cascades {
        class KeyEvent;
    }
}

namespace canadainc {

class Persistance;

class AdminUtils : public QObject
{
    Q_OBJECT

    QStringList m_buffer;
    QString m_expected;
    bool m_isAdmin;
    Persistance* m_settings;

signals:
    void adminEnabledChanged();
    void authenticate(QUrl const& q);

private slots:
    void onKeyReleasedHandler(bb::cascades::KeyEvent* event);

public:
    AdminUtils(Persistance* p);
    virtual ~AdminUtils();
    void initPage(QObject* page);
    void processResponse(QString const& result);
    bool isAdmin() const;
};

} /* namespace canadainc */

#endif /* ADMINUTILS_H_ */
