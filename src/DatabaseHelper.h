#ifndef DATABASEHELPER_H_
#define DATABASEHELPER_H_

#include "customsqldatasource.h"

#include <QDir>

#define ATTACH_DATABASE_ID -1
#define INIT_SETUP_ID -2
#define FOREIGN_KEY_SETUP -3
#define DETACH_DATABASE_ID -4
#define COMMITTING_ANALYTICS -5
#define COMMIT_ANALYTICS -6

namespace canadainc {

class DatabaseHelper : public QObject
{
    Q_OBJECT

    CustomSqlDataSource m_sql;
    int m_currentId;
    QMap< QObject*, QMap<int,bool> > m_objectToIds;
    QMap< QPair<QString, QString>, int> m_counters;
    QMap<int, QPair<QObject*,int> > m_idToObjectQueryType;
    QMap<QString, bool> m_attached;

    void stash(QObject* caller, int t);

signals:
    /**
     * No one should really listen to this. This is a global signal for all executions that happened. Listening to this really defeats the purpose of this class
     * but may be necessary in some isolated cases.
     * @id The query ID executed.
     */
    void finished(int id);

private slots:
    void commitStats();
    void dataLoaded(int id, QVariant const& data);
    void onDestroyed(QObject* obj);

public:
    DatabaseHelper(QString const& dbase, QObject* parent=NULL);
    virtual ~DatabaseHelper();

    void attachIfNecessary(QString const& dbase, bool homePath=false, int id=ATTACH_DATABASE_ID);
    void attachIfNecessary(QString const& dbase, QString const& path, int id=ATTACH_DATABASE_ID);
    void detach(QString const& dbase, int id=DETACH_DATABASE_ID);
    void enableForeignKeys(int id=FOREIGN_KEY_SETUP);
    void endTransaction(QObject* caller, int id);
    void executeInternal(QString const& query, int t, QVariantList const& args=QVariantList());
    void executeQuery(QObject* caller, QString const& query, int t, QVariantList const& args=QVariantList());
    void initSetup(QObject* caller, QStringList const& setupStatements, int id=INIT_SETUP_ID);
    void startTransaction(QObject* caller, int id);
    Q_INVOKABLE void record(QString const& event, QString const& context="");
};

} /* namespace canadainc */

#endif /* DATABASEHELPER_H_ */
