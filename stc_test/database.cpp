#include "database.h"

DataBase::DataBase(QObject *parent)
{

}

DataBase::~DataBase()
{

}

void DataBase::connectToDataBase()
{
    if (!QFile(DATABASE_NAME).exists()) {
        this->restoreDataBase();
    } else {
        this->openDataBase();
    }
}

void DataBase::disconnectFromDataBase()
{
    closeDataBase();
}

void DataBase::insertIntoTable(const Texteditor &editor)
{
    if (openDataBase()) {
        QSqlQuery qry;
        qry.prepare("insert into " TABLE " ( " TABLE_TEXTEDITOR ", " TABLE_FILEFORMATS ", " TABLE_ENCODING ", "
                    TABLE_HASINTELLISENSE ", " TABLE_HASPLUGINS ", " TABLE_CANCOMPILE ")"
                    "values (:texteditor, :fileformats, :encoding, :hasintellisense, :hasplugins, :cancompile);");


        qry.bindValue(":texteditor",      editor.texteditor);
        qry.bindValue(":fileformats",     editor.fileformats);
        qry.bindValue(":encoding",        editor.encoding);
        qry.bindValue(":hasintellisense", (editor.hasintellisense == "true") ? 1 : 0);
        qry.bindValue(":hasplugins",      (editor.hasplugins == "true") ? 1 : 0);
        qry.bindValue(":cancompile",      (editor.cancompile == "true") ? 1 : 0);

        if (!qry.exec()) {
            qDebug() << "error insert into " << TABLE;
            qDebug() << qry.lastError().text();
        }
        
        //te_db.close();
        closeDataBase();
        qDebug() << "Base closed after insert";
   }
}

void DataBase::deleteFromTable(const QString &texteditor)
{
    if (te_db.open()) {
        QSqlQuery query;

        query.prepare("DELETE FROM " TABLE " WHERE " TABLE_TEXTEDITOR " = :texteditor;");

        query.bindValue(":texteditor", texteditor);

        if (!query.exec()) {
            qDebug() << "error delete from " << TABLE;
            qDebug() << query.lastError().text();
        }
    }

    te_db.close();
    qDebug() << "Base closed after delete";
}

void DataBase::clearTable()
{
    if (te_db.open()) {
        QSqlQuery query;

        query.prepare("delete from " TABLE);

        if (!query.exec()) {
            qDebug() << "error clear table " << TABLE;
            qDebug() << query.lastError().text();
        }
    }

    te_db.close();
}

void DataBase::updateTable(const QString& oldTextEditor, const Texteditor &editor)
{
    if (openDataBase()) {
        QSqlQuery qry;
        qry.prepare("update " TABLE
                    " set " TABLE_TEXTEDITOR " = :texteditor, " TABLE_FILEFORMATS " = :fileformats, " TABLE_ENCODING " = :encoding, "
                    TABLE_HASINTELLISENSE "= :hasintellisense, " TABLE_HASPLUGINS " = :hasplugins, " TABLE_CANCOMPILE " = :cancompile "
                    "where " TABLE_TEXTEDITOR " = :oldtexteditor;");

        qry.bindValue(":oldtexteditor",   oldTextEditor);
        qry.bindValue(":texteditor",      editor.texteditor);
        qry.bindValue(":fileformats",     editor.fileformats);
        qry.bindValue(":encoding",        editor.encoding);
        qry.bindValue(":hasintellisense", (editor.hasintellisense == "true") ? 1 : 0);
        qry.bindValue(":hasplugins",      (editor.hasplugins == "true") ? 1 : 0);
        qry.bindValue(":cancompile",      (editor.cancompile == "true") ? 1 : 0);
        if (!qry.exec()) {
            qDebug() << "error insert into " << TABLE;
            qDebug() << qry.lastError().text();
        }
    }

    te_db.close();
    qDebug() << "Base closed after update";
}

QVector<Texteditor> DataBase::getTexteditorsFromDB()
{
    if (te_db.open()) {
        QVector <Texteditor> texteditors;
        Texteditor editor;

        QSqlQuery query;
        query.exec("select * from texteditors");

        while (query.next()) {
            editor.texteditor      = query.value(0).toString();
            editor.fileformats     = query.value(1).toString();
            editor.encoding        = query.value(2).toString();
            editor.hasintellisense = (query.value(3) == 1) ? "true" : "false";
            editor.hasplugins      = (query.value(4) == 1) ? "true" : "false";
            editor.cancompile      = (query.value(5) == 1) ? "true" : "false";

            texteditors.push_back(editor);
        }

        te_db.close();
        qDebug() << "Base closed get Texteditors";

        return texteditors;
    }
}

bool DataBase::openDataBase()
{
    te_db = QSqlDatabase::addDatabase("QSQLITE");
    te_db.setHostName(DATABASE_HOSTNAME);
    te_db.setDatabaseName(DATABASE_NAME);
    if(te_db.open()){
        //te_db.close();
        return true;
    } else {
        return false;
    }
}

void DataBase::closeDataBase()
{
    te_db.close();
}

bool DataBase::restoreDataBase()
{
    if(te_db.open()){
        if(!this->createTable()){
            return false;
        } else {
            //te_db.close();
            qDebug() << "Base closed after create table";
            return true;
        }
    } else {
        qDebug() << "Failed to restore database";
        return false;
    }
    return false;
}

bool DataBase::createTable()
{
    te_db.open();

    QSqlQuery query;

    if (!query.exec("create table texteditors "
                    "(texteditor varchar(20) primary key, "
                    "fileformats varchar(20), "
                    "encoding varchar(30), "
                    "hasintellisense integer,"
                    "hasplugins integer,"
                    "cancompile integer)")) {
        qDebug() << "DataBase: error of create " << TABLE;
        qDebug() << query.lastError().text();
        return false;
        } else {
        return true;
    }

    te_db.close();
    return false;

}
