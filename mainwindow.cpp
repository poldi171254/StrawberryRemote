#include <QDebug>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);
    controller_ = new Controller();
    QWidget::connect(ui_->connectButton, &QAbstractButton::pressed, this, &MainWindow::connect);
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::Init()
{
    ipAddress_ = controller_->GetIpAddress();
    ui_->ipAddress->setText(ipAddress_);
}

void MainWindow::connect()
{
    ipAddress_ = ui_->ipAddress->text();
    port_ = ui_->port->value();

    ui_->connectButton->setEnabled(false);
    // statusWindow_ = new ConnectionStatus();
    // statusWindow_->show();
    // controller_->Init(ipAddress_, port_);
    controller_->Init(ipAddress_, port_);
    qDebug() << "Window popup";
}
