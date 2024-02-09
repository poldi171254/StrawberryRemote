#include "mainwindow.h"

#include <QApplication>
#include "controller.h"

int main(int argc, char *argv[])
{
    QString ipAddr_;
    qint16 port_;
    QApplication a(argc, argv);
    MainWindow w;
    w.Init();
    w.show();

    return a.exec();
}
