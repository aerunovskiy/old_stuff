#ifndef IMPORTWINDOW_H
#define IMPORTWINDOW_H

#include <QWidget>
#include <QDebug>

namespace Ui {
class ImportWindow;
}

class ImportWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ImportWindow(QWidget *parent = nullptr);
    ~ImportWindow();
public slots:
    void getMaxPB(const int &max);
    void incProgressBarValue();
    void getLog(const QString &log);

signals:
    void finished();

private:
    Ui::ImportWindow *ui;
};

#endif // IMPORTWINDOW_H
