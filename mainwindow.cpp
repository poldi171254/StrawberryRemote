#include <QDebug>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      controller_(new Controller)
{
    ui_->setupUi(this);
    //controller_ = new Controller();
    QWidget::connect(ui_->connectButton, &QAbstractButton::pressed, this, &MainWindow::connect);
    QWidget::connect(ui_->finishButton, &QAbstractButton::pressed, this, &MainWindow::Finish);
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

void MainWindow::Finish()
{
    QMainWindow::close();
}

void MainWindow::connect()
{
    ipAddress_ = ui_->ipAddress->text();
    port_ = ui_->port->value();
    ui_->connectButton->setEnabled(false);
    controller_->Init(ipAddress_, port_);
    Finish();
}
