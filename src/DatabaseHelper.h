#ifndef DATABASEHELPER_H_
#define DATABASEHELPER_H_

#include "customsqldatasource.h"

#include <QDir>

#define ATTACH_DATABASE_ID -1
#define INIT_SETUP_ID -2
#define FOREIGN_KEY_SETUP -3
#define DETACH_DATABASE_ID -4

namespace canadainc {

class DatabaseHelper : public QObject
{
    Q_OBJECT

    CustomSqlDataSource m_sql;
    QMap<int, QPair<QObject*,int> > m_idToObjectQueryType;
    QMap< QObject*, QMap<int,bool> > m_objectToIds;
    int m_currentId;
    QMap<QString, bool> m_attached;

    void stash(QObject* caller, int t);

signals:
    void finished(int id);

private slots:
    void dataLoaded(int id, QVariant const& data);
    void onDestroyed(QObject* obj);

public:
    DatabaseHelper(QString const& dbase, QObject* parent=NULL);
    virtual ~DatabaseHelper();

    void executeQuery(QObject* caller, QString const& query, int t, QVariantList const& args=QVariantList());
    void attachIfNecessary(QString const& dbase, bool homePath=false, int id=ATTACH_DATABASE_ID);
    void attachIfNecessary(QString const& dbase, QString const& path, int id=ATTACH_DATABASE_ID);
    void detach(QString const& dbase, int id=DETACH_DATABASE_ID);
    void initSetup(QObject* caller, QStringList const& setupStatements, int id=INIT_SETUP_ID);
    void enableForeignKeys(int id=FOREIGN_KEY_SETUP);
    void startTransaction(int id);
    void endTransaction(int id);
};

} /* namespace canadainc */

#endif /* DATABASEHELPER_H_ */
