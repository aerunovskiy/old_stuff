#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QFile>
#include <QDate>
#include <QDebug>
#include <QVector>
#include "tablemodel.h"

/* Directives for table names, table fields, and database */
#define DATABASE_HOSTNAME       "te_db"
#define DATABASE_NAME           "te_db.sqlite"

#define TABLE                   "texteditors"
#define TABLE_TEXTEDITOR        "texteditor"
#define TABLE_FILEFORMATS       "fileformats"
#define TABLE_ENCODING          "encoding"
#define TABLE_HASINTELLISENSE   "hasintellisense"
#define TABLE_HASPLUGINS        "hasplugins"
#define TABLE_CANCOMPILE        "cancompile"


class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = 0);
    ~DataBase();

    void connectToDataBase();
    void disconnectFromDataBase();
    void insertIntoTable(const Texteditor &editor);
    void deleteFromTable(const QString &texteditor);
    void clearTable();
    void updateTable(const QString& oldTextEditor, const Texteditor &editor);
    QVector <Texteditor> getTexteditorsFromDB();

private:
    QSqlDatabase te_db;

    bool openDataBase();
    void closeDataBase();
    bool restoreDataBase();
    bool createTable();
};

#endif // DATABASE_H
