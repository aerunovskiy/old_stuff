#ifndef XMLIMPORTER_H
#define XMLIMPORTER_H

#include <QObject>
#include <QFile>
#include <QtXml>
#include "importdialog.h"
#include "tablemodel.h"

class XmlImporter : public QObject
{
    Q_OBJECT
public:
    explicit XmlImporter(QObject *parent = nullptr);
    static QString path;

signals:
    void finished();
    void sendImportedEditor(QString, Texteditor, bool);

    void sendMaxPB(int);
    void incProgressBarValue();
    void sendLog(const QString &log);

public slots:
    void importFiles();
};

#endif // XMLIMPORTER_H
