#ifndef ADMINUTILS_H_
#define ADMINUTILS_H_

#include <QObject>

namespace canadainc {

struct AdminUtils : public QObject
{
    Q_OBJECT

    QStringList m_buffer;
    QString m_expected;
    bool m_isAdmin;

    AdminUtils();
    virtual ~AdminUtils();
    void initPage(QObject* page);

signals:
    void adminEnabledChanged();

private slots:
    void onKeyReleasedHandler(bb::cascades::KeyEvent* event);
};

} /* namespace canadainc */

#endif /* ADMINUTILS_H_ */
