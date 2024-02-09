#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connectionstatus.h"
#include "controller.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void Init();

private slots:
    void connect();

private:
    Ui::MainWindow *ui_;
    ConnectionStatus *statusWindow_;
    Controller *controller_;
    QString ipAddress_;
    qint16 port_;
};
#endif // MAINWINDOW_H
