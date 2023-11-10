#ifndef QUERYTHREAD_H
#define QUERYTHREAD_H

#include <QThread>
#include <QtSql>

class QueryThread : public QThread
{
    Q_OBJECT
public:
    QueryThread(QObject *parent = nullptr);

signals:
    void queryFinished(const QList<QSqlRecord> &records);
    void queue(const QString &query);

public slots:
    void slotExecQuery(const QString &query);
    void run();
};

#endif // QUERYTHREAD_H
