#ifndef CANADINCTESTER_H_
#define CANADINCTESTER_H_

#include <bb/cascades/ListItemProvider>
#include <bb/cascades/ListView>
#include <bb/cascades/Page>
#include <bb/cascades/Sheet>
#include <bb/cascades/StandardListItem>
#include <bb/cascades/TitleBar>

#include "Logger.h"

#define KEY_PASSED "passed"
#define KEY_EXPIRED "expired"
#define KEY_RUNNING "running"

namespace canadainc {

using namespace bb::cascades;

class TestResultListItemProvider : public ListItemProvider
{
    VisualNode* createItem(ListView* list, QString const& type)
    {
        Q_UNUSED(list);
        Q_UNUSED(type);

        return new StandardListItem();
    }


    void updateItem(ListView* list, VisualNode* control, QString const& type, QVariantList const& indexPath, QVariant const& d)
    {
        Q_UNUSED(list);
        Q_UNUSED(indexPath);
        Q_UNUSED(type);

        QVariantMap data = d.toMap();

        StandardListItem* h = static_cast<StandardListItem*>(control);
        h->setTitle( data.value("title").toString() );

        if ( data.contains(KEY_PASSED) )
        {
            bool passed = data.value(KEY_PASSED).toBool();
            bool expired = data.value(KEY_EXPIRED).toBool();
            h->setImageSource(passed ? QUrl("asset:///images/bugs/ic_bugs_submit.png") : expired ? QUrl("asset:///images/bugs/ic_bugs_info.png") : QUrl("asset:///images/bugs/ic_bugs_cancel.png"));
            h->setStatus(passed ? "Passed" : "Failed!");
        } else {
            bool running = data.value(KEY_RUNNING).toBool();

            if (running)
            {
                h->setImageSource( QUrl("asset:///images/menu/ic_help.png") );
                h->setStatus("Running...");
            } else {
                h->setImageSource( QUrl("asset:///images/menu/ic_help.png") );
                h->resetStatus();
            }
        }

        if ( data.contains("t") ) {
            h->setDescription( QString("%1 ms").arg( data.value("t").toLongLong() ) );
        } else {
            h->resetDescription();
        }
    }
};

class CanadaIncTester : public QObject
{
    Q_OBJECT

    QMap<QString, int> m_testToIndex;
    ArrayDataModel* m_adm;
    QMap<QString, QElapsedTimer*> m_timers;
    QObject* m_root;
    int m_currentIndex;
    QTimer m_monitor;

    CanadaIncTester(QMap<QString, QObject*> context) : m_adm( new ArrayDataModel() ), m_currentIndex(0)
    {
        QmlDocument* qml = QmlDocument::create("asset:///AllTests.qml").parent(this);
        qml->setContextProperty("harness", this);

        foreach ( QString const& key, context.keys() ) {
            qml->setContextProperty( key, context.value(key) );
        }

        m_root = qml->createRootObject<QObject>();
        QObjectList all = m_root->children();

        for (int i = 0; i < all.size(); i++)
        {
            QObject* q = all[i];

            QString name = q->objectName();
            m_testToIndex[name] = i;

            QVariantMap current;
            current["title"] = name;

            m_adm->append(current);
        }

        ListView* lv = ListView::create();
        lv->setListItemProvider( new TestResultListItemProvider() );
        lv->setDataModel(m_adm);

        Sheet* s = Sheet::create().parent(this);

        Page* p = Page::create().titleBar( TitleBar::create().dismissAction( ActionItem::create().title("Close").onTriggered( s, SLOT( close() ) ) ).acceptAction( ActionItem::create().title("Run").onTriggered( this, SLOT( run() ) ) ) );
        p->setContent(lv);
        s->setContent(p);

        s->open();

        m_monitor.setInterval(6000); // max of 6 seconds of delay
        m_monitor.setSingleShot(true);
        connect( &m_monitor, SIGNAL( timeout() ), this, SLOT( onTimeout() ) );
    }


    Q_SLOT void onTimeout()
    {
        QObjectList all = m_root->children();

        QObject* q = all[m_currentIndex];
        update(q, false, true);
    }


    Q_SLOT void run()
    {
        QObjectList all = m_root->children();

        if ( m_currentIndex < all.size() )
        {
            QObject* q = all[m_currentIndex];
            attachTimer(q);
            QMetaObject::invokeMethod(q, "run", Qt::QueuedConnection);
        }
    }

public:
    Q_INVOKABLE void attachTimer(QObject* q)
    {
        QElapsedTimer* qet = new QElapsedTimer();
        qet->start();

        m_timers[ q->objectName() ] = qet;
        m_monitor.start();

        QVariantMap current = m_adm->value(m_currentIndex).toMap();
        current[KEY_RUNNING] = true;
        m_adm->replace(m_currentIndex, current);
    }


    Q_INVOKABLE void update(QObject* q, bool passed, bool timedOut=false)
    {
        m_monitor.stop();

        QString name = q->objectName();
        int i = m_testToIndex[name];

        QVariantMap current = m_adm->value(i).toMap();
        current[KEY_PASSED] = passed;
        current[KEY_EXPIRED] = timedOut;

        if ( m_timers.contains(name) )
        {
            QElapsedTimer* qet = m_timers.value(name);
            current["t"] = qet->elapsed();

            delete qet;
            m_timers.remove(name);
        }

        m_adm->replace(i, current);
        ++m_currentIndex;

        run();
    }


    static void create(QMap<QString, QObject*> context) {
        new CanadaIncTester(context);
    }

    virtual ~CanadaIncTester() {}
};

} /* namespace canadainc */

#endif /* CANADINCTESTER_H_ */
