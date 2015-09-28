#include "AnalyticHelper.h"
#include "customsqldatasource.h"
#include "Logger.h"
#include "IOUtils.h"

#include <bb/Application>

#define ANALYTICS_PATH QString("%1/%2.db").arg( QDir::homePath() ).arg(ANALYTIC_DB_NAME)
#define COMMITTING_ANALYTICS -5
#define COMMIT_ANALYTICS -6

namespace canadainc {

AnalyticHelper::AnalyticHelper() {
    connect( QCoreApplication::instance(), SIGNAL( aboutToQuit() ), this, SLOT( onAboutToQuit() ) );
}


void AnalyticHelper::reset()
{
    QtConcurrent::run(QFile::remove, ANALYTICS_PATH);
    /*
    CustomSqlDataSource* sql = initAnalytics();
    sql->setName("analytics2");
    sql->setQuery("DELETE FROM events");
    sql->executeAndWait(QVariant(), COMMITTING_ANALYTICS);
    sql->setQuery("VACUUM");
    sql->executeAndWait(QVariant(), COMMIT_ANALYTICS); */
}


void AnalyticHelper::commitStats()
{
    if ( !m_counters.isEmpty() )
    {
        LOGGER( bb::Application::instance()->extendTerminationTimeout() );

        QList< QPair<QString, QString> > keys = m_counters.keys();

        if ( !QFile::exists(ANALYTICS_PATH) ) {
            IOUtils::writeFile(ANALYTICS_PATH);
        }

        CustomSqlDataSource* sql = new CustomSqlDataSource(this);
        connect( sql, SIGNAL( dataLoaded(int, QVariant const&) ), this, SLOT( onDataLoaded(int, QVariant const&) ) );
        sql->setSource(ANALYTICS_PATH);
        sql->setName("analytics");
        sql->startTransaction(COMMITTING_ANALYTICS);
        sql->setQuery("CREATE TABLE IF NOT EXISTS events (id INTEGER PRIMARY KEY, event TEXT NOT NULL, context TEXT NOT NULL DEFAULT '', count INTEGER DEFAULT 0, UNIQUE(event,context) ON CONFLICT REPLACE CHECK(event <> ''))");
        sql->executeAndWait(QVariant(), COMMITTING_ANALYTICS);

        for (int i = keys.size()-1; i >= 0; i--)
        {
            QPair<QString, QString> pair = keys[i];
            sql->setQuery( QString("INSERT INTO events (event,context,count) VALUES (?,?,COALESCE((SELECT count FROM events WHERE event=? AND context=?)+%1,%1))").arg( m_counters.value(pair) ) );
            sql->executeAndWait( QVariantList() << pair.first << pair.second << pair.first << pair.second, COMMITTING_ANALYTICS );
        }

        sql->endTransaction(COMMIT_ANALYTICS);
        m_counters.clear();
    }
}


void AnalyticHelper::onAboutToQuit()
{
    record( "AppClose", QString::number( QDateTime::currentMSecsSinceEpoch() ) );
    commitStats();
}


void AnalyticHelper::onDataLoaded(int id, QVariant const& data)
{
    Q_UNUSED(data);

    if (id == COMMIT_ANALYTICS) {
        sender()->deleteLater();
    }
}


void AnalyticHelper::record(QString const& event, QString const& context)
{
    QPair<QString, QString> pair = qMakePair<QString, QString>(event, context);
    int count = m_counters.value(pair);
    ++count;

    m_counters[pair] = count;
}


AnalyticHelper::~AnalyticHelper()
{
}

} /* namespace canadainc */
