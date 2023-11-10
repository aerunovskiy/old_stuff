#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupModel();

    ui->textEditorsView->setModel(textEditorsModel);
    ui->textEditorsView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->textEditorsView->show();

    ui->textEditorsView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect (ui->textEditorsView, SIGNAL (doubleClicked(QModelIndex)), this, SLOT (editRecord()));
    connect (ui->textEditorsView, SIGNAL (customContextMenuRequested(QPoint)), this, SLOT(customMenuRequested(QPoint)));

   // connect(this, SIGNAL(sendLog(QString)), importWindow, SLOT(getLog(QString)), Qt::UniqueConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

Texteditor *MainWindow::getEditor(const int& row)
{
    // Creating an object in which we write the current field values
    Texteditor *editor = new Texteditor;

    // Value record
    QModelIndex texteditorIndex = textEditorsModel->index(row, 0, QModelIndex());
    QVariant varTexteditor = textEditorsModel->data(texteditorIndex, Qt::DisplayRole);
    editor->texteditor = varTexteditor.toString();

    QModelIndex fileformatsIndex = textEditorsModel->index(row, 1, QModelIndex());
    QVariant varFileformats = textEditorsModel->data(fileformatsIndex, Qt::DisplayRole);
    editor->fileformats = varFileformats.toString();

    QModelIndex encodingIndex = textEditorsModel->index(row, 2, QModelIndex());
    QVariant varEncoding = textEditorsModel->data(encodingIndex, Qt::DisplayRole);
    editor->encoding = varEncoding.toString();

    QModelIndex hasintellisenseIndex = textEditorsModel->index(row, 3, QModelIndex());
    QVariant varHasintellisense = textEditorsModel->data(hasintellisenseIndex, Qt::DisplayRole);
    editor->hasintellisense = varHasintellisense.toString();

    QModelIndex haspluginsIndex = textEditorsModel->index(row, 4, QModelIndex());
    QVariant varHasplugins = textEditorsModel->data(haspluginsIndex, Qt::DisplayRole);
    editor->hasplugins = varHasplugins.toString();

    QModelIndex cancompileIndex = textEditorsModel->index(row, 5, QModelIndex());
    QVariant varCancompile = textEditorsModel->data(cancompileIndex, Qt::DisplayRole);
    editor->cancompile = varCancompile.toString();

    return editor;
}


void MainWindow::on_importButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Value record"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);

    if (path != "") {

    importThread = new QThread;

    importer = new XmlImporter();
    importer->path = path;
    importer->moveToThread(importThread);

    connect (importThread, SIGNAL(started()),  importer, SLOT(importFiles()));
    connect (importer, SIGNAL(finished()),     importThread, SLOT(terminate()));
    connect (importer, SIGNAL(sendImportedEditor(QString, Texteditor, bool)), this, SLOT(receiveImportedEditor(QString, Texteditor, bool)), Qt::DirectConnection);

    importWindow = new ImportWindow();

    //connect(importThread, SIGNAL(started()),      importWindow, SLOT(show()));

    // SLOTS for send data between importer and importWindow
    connect(importer, SIGNAL(sendMaxPB(int)),        importWindow, SLOT(getMaxPB(int)));
    connect(importer, SIGNAL(incProgressBarValue()), importWindow, SLOT(incProgressBarValue()), Qt::UniqueConnection);
    connect(importer, SIGNAL(sendLog(QString)),      importWindow, SLOT(getLog(QString)), Qt::UniqueConnection);

    importThread->start(); 
    importWindow->show();

    }
}

void MainWindow::on_clearButton_clicked()
{
    QMessageBox clearMsg;
    clearMsg.setWindowTitle("Clear table");
    clearMsg.setText("Data will also be deleted from the database. Are you sure?");

    QPushButton *okBtn = clearMsg.addButton(QMessageBox::Ok);
    clearMsg.addButton(tr("Cancel"), QMessageBox::RejectRole);

    clearMsg.exec();

    if (clearMsg.clickedButton() == okBtn) {
        textEditorsModel->removeRows(0, textEditorsModel->rowCount(QModelIndex()), QModelIndex());

        te_db->clearTable();
    }

}


void MainWindow::updateModels()
{
    ui->textEditorsView->resizeColumnsToContents();
}

void MainWindow::editRecord()
{
    // Getting the number of the selected line
    int row = ui->textEditorsView->selectionModel()->currentIndex().row();

    // Creating an object in which we write the current field values
    Texteditor *oldEditor = getEditor(row);

    // Create an edit window
    editWindow = new EditDialog ();
    editWindow->setWindowTitle("Edit");

    // Transfer current values ​​to the editing window
    editWindow->editTexteditor(*oldEditor);

    if (editWindow->exec()) {
        // Retrieving modified values
        Texteditor *newEditor = editWindow->returnNewEditor();

        // Checking the uniqueness of the editor’s name OR the name has not changed
        if (!textEditorsModel->getTexteditors().contains(*newEditor) || newEditor->texteditor == oldEditor->texteditor) {
            // Changes only different fields
            if (newEditor->texteditor != oldEditor->texteditor) {
                const QModelIndex index = textEditorsModel->index(row, 0, QModelIndex());
                textEditorsModel->setData(index, newEditor->texteditor, Qt::EditRole);
            }

            if (newEditor->fileformats != oldEditor->fileformats) {
                const QModelIndex index = textEditorsModel->index(row, 1, QModelIndex());
                textEditorsModel->setData(index, newEditor->fileformats, Qt::EditRole);
            }

            if (newEditor->encoding != oldEditor->encoding) {
                const QModelIndex index = textEditorsModel->index(row, 2, QModelIndex());
                textEditorsModel->setData(index, newEditor->encoding, Qt::EditRole);
            }

            if (newEditor->hasintellisense != oldEditor->hasintellisense) {
                const QModelIndex index = textEditorsModel->index(row, 3, QModelIndex());
                textEditorsModel->setData(index, newEditor->hasintellisense, Qt::EditRole);
            }

            if (newEditor->hasplugins != oldEditor->hasplugins) {
                const QModelIndex index = textEditorsModel->index(row, 4, QModelIndex());
                textEditorsModel->setData(index, newEditor->hasplugins, Qt::EditRole);
            }

            if (newEditor->cancompile != oldEditor->cancompile) {
                const QModelIndex index = textEditorsModel->index(row, 5, QModelIndex());
                textEditorsModel->setData(index, newEditor->cancompile, Qt::EditRole);
            }

            te_db->updateTable(oldEditor->texteditor, *newEditor);
        } else {
            QMessageBox::information(this, tr("Double!"),
                                     tr("Editor \"%1\" already in the table.").arg(newEditor->texteditor) );
        }
    }
}

void MainWindow::removeRecord()
{
    int row = ui->textEditorsView->selectionModel()->currentIndex().row();
    QString record = getEditor(row)->texteditor;

    if (row >= 0) {
        if (QMessageBox::warning(this,
                                 tr("Delete record"),
                                 tr("Are you sure you want to delete this entry?"),
                                 QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            if(!textEditorsModel->removeRows(row, 1, QModelIndex())) {
                QMessageBox::warning(this, tr("Notification"),
                                     tr("Failed to delete entry"));
            } else {
                te_db->deleteFromTable(record);
            }

            ui->textEditorsView->setCurrentIndex(textEditorsModel->index(-1,-1));
        }
    }
}

void MainWindow::exportRecord()
{
    // Getting the number of the selected line
    int row = ui->textEditorsView->selectionModel()->currentIndex().row();

    // Creating an object in which we write the current field values
    Texteditor *editor = getEditor(row);

    QString filename = QFileDialog::getSaveFileName(this, tr("Export"), tr("%1.xml").arg(editor->texteditor));

    exporter = new XmlExporter();

    exporter->filename = filename;
    exporter->editor   = *editor;

    exportThread = new QThread;
    exporter->moveToThread(exportThread);

    connect(exportThread, SIGNAL(started()), exporter, SLOT(exportFile()));
    connect(exporter, SIGNAL(finished()), exportThread, SLOT(terminate()));

    exportThread->start();
}

// Creating popup-menu
void MainWindow::customMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);

    QAction *editDevice   = new QAction(tr("Edit"), this);
    QAction *removeDevice = new QAction(tr("Delete"), this);
    QAction *exportDevice = new QAction(tr("Export"), this);

    connect(editDevice,   SIGNAL(triggered()), this, SLOT(editRecord()));
    connect(removeDevice, SIGNAL(triggered()), this, SLOT(removeRecord()));
    connect(exportDevice, SIGNAL(triggered()), this, SLOT(exportRecord()));

    menu->addAction(editDevice);
    menu->addAction(removeDevice);
    menu->addAction(exportDevice);

    menu->popup(ui->textEditorsView->viewport()->mapToGlobal(pos));
}

// Receive editor from import thread
void MainWindow::receiveImportedEditor(const QString &filename, const Texteditor &editor, const bool &isImport)
{
    connect(this, SIGNAL(sendLog(QString)), importWindow, SLOT(getLog(QString)), Qt::UniqueConnection);

    if (!textEditorsModel->getTexteditors().contains(editor) && isImport == true) {
            textEditorsModel->insertRows(0, 1, QModelIndex());

            textEditorsModel->setFields(editor);

            te_db->insertIntoTable(editor);

            emit sendLog(tr("File %1 successfully imported").arg(filename));
        } else {
            //emit sendLog(tr("Double! Editor %1 already in the table. ").arg(editor.texteditor));
            emit sendLog("<FONT COLOR=#FF0000>File " + filename + " is not imported!</FONT>");
    }
}

// Filling a model with data from a database
void MainWindow::setupModel()
{
   te_db = new DataBase();

   te_db->connectToDataBase();

   textEditorsModel = new TableModel();

   for (auto &editor : te_db->getTexteditorsFromDB()) {
       textEditorsModel->insertRows(0, 1, QModelIndex());

       textEditorsModel->setFields(editor);
   }

}
