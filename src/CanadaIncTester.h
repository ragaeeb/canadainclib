#ifndef CANADINCTESTER_H_
#define CANADINCTESTER_H_

#include <bb/cascades/ActionItem>
#include <bb/cascades/ArrayDataModel>
#include <bb/cascades/ListItemProvider>
#include <bb/cascades/ListView>
#include <bb/cascades/Page>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Sheet>
#include <bb/cascades/StandardListItem>
#include <bb/cascades/TitleBar>

#include "Logger.h"

#define KEY_ELAPSED_TIME "t"
#define KEY_EXPIRED "expired"
#define KEY_FAILURE_CONTEXT "reason"
#define KEY_PASSED "passed"
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
            h->setStatus(passed ? "Passed" : QString("Failed: %1").arg( data.value(KEY_FAILURE_CONTEXT).toString() ) );
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

        if ( data.contains(KEY_ELAPSED_TIME) ) {
            h->setDescription( QString("%1 ms").arg( data.value(KEY_ELAPSED_TIME).toLongLong() ) );
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
    ListView* m_lv;

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

        m_lv = ListView::create();
        m_lv->setListItemProvider( new TestResultListItemProvider() );
        m_lv->setDataModel(m_adm);

        Sheet* s = Sheet::create().parent(this);

        Page* p = Page::create().titleBar( TitleBar::create().dismissAction( ActionItem::create().title("Close").imageSource( QUrl("asset:///images/bugs/ic_bugs_cancel.png") ).onTriggered( s, SLOT( close() ) ) ).acceptAction( ActionItem::create().title("Run").imageSource( QUrl("asset:///images/bugs/ic_bugs_submit.png") ).onTriggered( this, SLOT( run() ) ) ) );
        p->setContent(m_lv);
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
        QVariantMap current = m_adm->value(m_currentIndex).toMap();
        current[KEY_EXPIRED] = true;
        m_adm->replace(m_currentIndex, current);

        assert(q, false);
    }


    Q_SLOT void run()
    {
        QObjectList all = m_root->children();

        if ( m_currentIndex < all.size() )
        {
            QObject* q = all[m_currentIndex];
            attachTimer(q);
            QMetaObject::invokeMethod(q, "run", Qt::QueuedConnection);

            m_lv->scrollToItem( QVariantList() << m_currentIndex, ScrollAnimation::Smooth );
        }
    }

    void applyResult(QObject* q, bool passed, QString const& reason=QString())
    {
        m_monitor.stop();

        QString name = q->objectName();
        int i = m_testToIndex[name];

        QVariantMap current = m_adm->value(i).toMap();
        current[KEY_PASSED] = passed;

        if ( m_timers.contains(name) )
        {
            QElapsedTimer* qet = m_timers.value(name);
            current[KEY_ELAPSED_TIME] = qet->elapsed();

            delete qet;
            m_timers.remove(name);
        }

        if (!passed) {
            current[KEY_FAILURE_CONTEXT] = reason;
        }

        m_adm->replace(i, current);
        ++m_currentIndex;

        run();
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


    Q_INVOKABLE void dump(QVariant const& q) {
        LOGGER(q);
    }


    Q_INVOKABLE void assert(QObject* q, QVariant const& expected, QVariant const& actual=QVariant())
    {
        bool passed = actual.isNull() ? expected.toBool() : expected == actual;

        if (!passed) {
            LOGGER( q->objectName() << expected.toString() << actual.toString() );
        }

        applyResult( q, passed, QString("%1 != %2").arg( expected.toString() ).arg( actual.toString() ) );
    }


    Q_INVOKABLE void assert(QObject* q, QVariantList const& pairs)
    {
        if ( pairs.size()%2 == 0 )
        {
            bool passed = false;
            QStringList reasons;
            int i = 0;

            for (i = 0; i < pairs.size(); i += 2)
            {
                QVariant expected = pairs[i];
                QVariant actual = pairs[i+1];

                if (expected != actual) {
                    reasons << QString("%1 != %2").arg( expected.toString() ).arg( actual.toString() );
                }
            }

            if ( i == pairs.size() ) { // went through entire loop
                passed = true;
            }

            applyResult( q, passed, reasons.join("; ") );
        } else {
            applyResult(q, false, QString("OddPair!: %1").arg( pairs.size() ) );
        }
    }


    static void create(QMap<QString, QObject*> context) {
        new CanadaIncTester(context);
    }

    virtual ~CanadaIncTester() {}
};

} /* namespace canadainc */

#endif /* CANADINCTESTER_H_ */
