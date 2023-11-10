#include "querythread.h"

QueryThread::QueryThread(QObject *parent)
{

}

void QueryThread::run()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "querythread");


}

void QueryThread::slotExecQuery(const QString &query){

}
