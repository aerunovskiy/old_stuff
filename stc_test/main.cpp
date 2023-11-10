#include <QApplication>
#include <QLockFile>
#include <QDir>
#include <QMessageBox>

#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QLockFile lockFile(QDir::temp().absoluteFilePath("b.lock"));

    if (!lockFile.tryLock(100)) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("The application is already running.\n"
                       "Only one instance of the application is allowed to run.");
        msgBox.exec();
        return 1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
