#include "editdialog.h"
#include "ui_editdialog.h"

EditDialog::EditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDialog)
{
    ui->setupUi(this);
}

EditDialog::~EditDialog()
{
    delete ui;
}

Texteditor *EditDialog::returnNewEditor()
{
    Texteditor *newEditor = new Texteditor;

    newEditor->texteditor =  ui->textEditorEdit->text();
    newEditor->fileformats = ui->fileFormatsEdit->text();
    newEditor->encoding = ui->encodingEdit->text();
    newEditor->hasintellisense = (ui->hiComboBox->currentIndex() == 0) ? "true" : "false";
    newEditor->hasplugins = (ui->hpComboBox->currentIndex() == 0) ? "true" : "false";
    newEditor->cancompile = (ui->ccComboBox->currentIndex() == 0) ? "true" : "false";

    return  newEditor;
}

void EditDialog::editTexteditor(const Texteditor &editor)
{
    ui->textEditorEdit->setText(editor.texteditor);
    ui->fileFormatsEdit->setText(editor.fileformats);
    ui->encodingEdit->setText(editor.encoding);
    ui->hiComboBox->setCurrentIndex((editor.hasintellisense == "true") ? 0 : 1);
    ui->hpComboBox->setCurrentIndex((editor.hasplugins == "true") ? 0 : 1);
    ui->ccComboBox->setCurrentIndex((editor.cancompile == "true") ? 0 : 1);
}
