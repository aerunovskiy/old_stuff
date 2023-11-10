#include "xmlimporter.h"

QString XmlImporter::path;

XmlImporter::XmlImporter(QObject *parent) : QObject(parent)
{

}

void XmlImporter::importFiles()
{
    QDir dir (path);

    QStringList fileList = dir.entryList(QStringList()<<"*.xml");

    emit sendMaxPB(fileList.size());

    for (auto& f : fileList) {
        bool isImport = true;
        emit incProgressBarValue();
        QFile file(dir.path() +'/' + f);
        Texteditor editor;

        if (file.open(QIODevice::ReadOnly)) {
            QXmlStreamReader sr(&file);

                while (!sr.atEnd()) {
                    QXmlStreamReader::TokenType token = sr.readNext();

                    if (token == QXmlStreamReader::StartElement) {

                        if (sr.name() == "texteditor") {
                            sr.readNext();
                            editor.texteditor = sr.text().toString();
                        }

                        if (sr.name() == "fileformats") {
                            sr.readNext();
                            editor.fileformats = sr.text().toString();
                        }

                        if (sr.name() == "encoding") {
                            sr.readNext();
                            editor.encoding = sr.text().toString();
                        }

                        if (sr.name() == "hasintellisense") {
                            sr.readNext();
                            editor.hasintellisense = sr.text().toString();
                        }
                        if (sr.name() == "hasplugins") {
                            sr.readNext();
                            editor.hasplugins = sr.text().toString();
                        }

                        if (sr.name() == "cancompile") {
                            sr.readNext();
                            editor.cancompile = sr.text().toString();
                        }
                    }
                }

                file.close();

                if (sr.hasError()) {
                    emit sendLog(tr("<FONT COLOR=#FF0000>Error: %1</FONT>").arg(sr.errorString()));
                    isImport = false;
                }

                emit sendImportedEditor(f, editor, isImport);

                isImport = true;
        }

    }

    emit finished();
}
