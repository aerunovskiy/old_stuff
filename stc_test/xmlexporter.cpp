#include "xmlexporter.h"

QString XmlExporter::filename;
Texteditor XmlExporter::editor;

XmlExporter::XmlExporter(QObject *parent) : QObject(parent)
{

}

void XmlExporter::exportFile()
{
    QFile file(filename);
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("root");

    xmlWriter.writeStartElement("texteditor");
    xmlWriter.writeCharacters(editor.texteditor);
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("fileformats");
    xmlWriter.writeCharacters(editor.fileformats);
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("encoding");
    xmlWriter.writeCharacters(editor.encoding);
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("hasintellisense");
    xmlWriter.writeCharacters(editor.hasintellisense);
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("hasplugins");
    xmlWriter.writeCharacters(editor.hasplugins);
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("cancompile");
    xmlWriter.writeCharacters(editor.cancompile);
    xmlWriter.writeEndElement();

    xmlWriter.writeEndElement(); //root

    xmlWriter.writeEndDocument();
    file.close();

    emit finished();
}
