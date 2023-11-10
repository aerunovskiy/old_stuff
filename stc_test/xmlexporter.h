#ifndef XMLEXPORTER_H
#define XMLEXPORTER_H

#include <QObject>
#include <QFile>
#include <QtXml>
#include "tablemodel.h"

class XmlExporter : public QObject
{
    Q_OBJECT
public:
    explicit XmlExporter(QObject *parent = nullptr);
    static QString filename;
    static Texteditor editor;

signals:
    void finished();

public slots:
    void exportFile();

};

#endif // XMLEXPORTER_H
