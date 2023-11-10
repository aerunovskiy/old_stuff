#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include "tablemodel.h"

namespace Ui {
class EditDialog;
}

class EditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditDialog(QWidget *parent = nullptr);
    ~EditDialog();

    Texteditor *returnNewEditor();
    void editTexteditor (const Texteditor &editor);

private:
    Ui::EditDialog *ui;
};

#endif // EDITDIALOG_H
