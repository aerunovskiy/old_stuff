#include "importwindow.h"
#include "ui_importwindow.h"

ImportWindow::ImportWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImportWindow)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_QuitOnClose, false);
    this->setAttribute(Qt::WA_DeleteOnClose, true);

    qDebug() << "Import Window create";
}

ImportWindow::~ImportWindow()
{
    delete ui;

    qDebug() << "Import Window destroy";
}

void ImportWindow::getMaxPB(const int &max)
{
    ui->iwProgressBar->setMaximum(max);
}

void ImportWindow::incProgressBarValue()
{
    ui->iwProgressBar->setValue(ui->iwProgressBar->value() + 1);
}

void ImportWindow::getLog(const QString &log)
{
    ui->iwLogsEdit->append(log);
}
