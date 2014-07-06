#ifndef DATABASEHELPER_H_
#define DATABASEHELPER_H_

#include "customsqldatasource.h"

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

private slots:
    void dataLoaded(int id, QVariant const& data);
    void onDestroyed(QObject* obj);

public:
    DatabaseHelper(QString const& dbase, QObject* parent=NULL);
    virtual ~DatabaseHelper();

    void executeQuery(QObject* caller, QString const& query, int t, QVariantList const& args=QVariantList());
    void attachIfNecessary(QString const& dbase);
    void initSetup(QObject* caller, QStringList const& setupStatements, int id=-2);
};

} /* namespace canadainc */

#endif /* DATABASEHELPER_H_ */
