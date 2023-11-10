#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QtXml>
#include <QFile>
#include <QMessageBox>
#include <QAbstractTableModel>
#include <QSqlDatabase>
#include <QStringList>
#include <QFileDialog>
#include <QProgressDialog>
#include "tablemodel.h"
#include "editdialog.h"
#include "database.h"
#include "xmlexporter.h"
#include "xmlimporter.h"
#include "importwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Texteditor *getEditor(const int& row); // Create an object from the selected row of the table

public slots:
    void receiveImportedEditor(const QString &filename, const Texteditor& editor, const bool &isImport);

private slots:
    void on_importButton_clicked(); // Execute import dialog
    void on_clearButton_clicked(); // Execute data deletion dialog

    void updateModels();
    void editRecord();
    void removeRecord();
    void exportRecord();
    void customMenuRequested(QPoint pos);

private:
    Ui::MainWindow *ui;

    TableModel *textEditorsModel = new TableModel();
    EditDialog *editWindow = new EditDialog();

    //ImportDialog *importWindow = new ImportDialog();
    ImportWindow *importWindow = new ImportWindow();
   // QThread *iwThread = new QThread;

    DataBase *te_db = new DataBase();

    XmlImporter *importer = new XmlImporter();
    QThread *importThread = new QThread;

    XmlExporter *exporter = new XmlExporter();
    QThread *exportThread = new QThread;

    void setupModel();

signals:
    void sendLog(const QString &log);

};
#endif // MAINWINDOW_H
