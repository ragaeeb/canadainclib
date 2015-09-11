#ifndef DATABASEHELPER_H_
#define DATABASEHELPER_H_

#include "customsqldatasource.h"

#include <QDir>
#include <QFutureWatcher>

namespace canadainc {

class InternalQueryId
{
    Q_GADGET
    Q_ENUMS(Type)

public:
    enum Type {
        AttachDatabase = -1,
        ForeignKeySetup = -2,
        DetachDatabase = -3,
        FetchAllIds = -4,
        PendingTransaction = -5,
        UpdateIdWithIndex = -6
    };
};

class DatabaseHelper : public QObject
{
    Q_OBJECT

    CustomSqlDataSource m_sql;
    int m_currentId;
    QMap< QObject*, QSet<int> > m_objectToIds;
    QMap<int, QPair<QObject*,int> > m_idToObjectQueryType;
    QSet<QString> m_attached;
    bool m_enabled;

    void stash(QObject* caller, int t);

signals:
    void error(QString const& message);

    /**
     * No one should really listen to this. This is a global signal for all executions that happened. Listening to this really defeats the purpose of this class
     * but may be necessary in some isolated cases.
     * @id The query ID executed.
     */
    void finished(int id);
    void setupError(QString const& message);

private slots:
    void dataLoaded(int id, QVariant const& data);
    void onDatabaseCreated();
    void onDestroyed(QObject* obj);

public:
    DatabaseHelper(QString const& dbase, QObject* parent=NULL);
    virtual ~DatabaseHelper();

    Q_INVOKABLE void attachIfNecessary(QString const& dbase, bool homePath=false);
    Q_INVOKABLE void attachIfNecessary(QString const& dbase, QString const& path);
    Q_INVOKABLE void detach(QString const& dbase);
    Q_SLOT void enableForeignKeys();
    Q_INVOKABLE void endTransaction(QObject* caller, int id);
    Q_INVOKABLE void executeInternal(QString const& query, int t, QVariantList args=QVariantList());
    Q_INVOKABLE void executeQuery(QObject* caller, QString const& query, int t, QVariantList const& args=QVariantList());
    Q_INVOKABLE void executeDelete(QObject* caller, QString const& table, int type, qint64 id, QString const& idField="id");
    Q_INVOKABLE void executeClear(QObject* caller, QString const& table, int type);
    Q_INVOKABLE qint64 executeInsert(QString const& table, QVariantMap const& keyValues);
    Q_INVOKABLE void executeUpdate(QObject* caller, QString const& table, QVariantMap const& keyValues, int type, qint64 id, QString const& idField="id");
    Q_INVOKABLE void fetchAllIds(QObject* caller, QString const& table);
    Q_INVOKABLE void setIndexAsId(QObject* caller, QVariantList const& q, QVariantList const& intersection=QVariantList());
    Q_INVOKABLE void startTransaction(QObject* caller, int id);
    void createDatabaseIfNotExists(bool sameThread=false, QStringList const& setupStatements=QStringList()) const;
    void setEnabled(bool enabled);
    void setVerboseLogging(bool enabled=true);

    Q_INVOKABLE static QString getPlaceHolders(int n, bool multi=true, QString const& symbol="?");
};

} /* namespace canadainc */

#endif /* DATABASEHELPER_H_ */
