#include "connectionstatus.h"
#include "ui_connectionstatus.h"

class Controller;

ConnectionStatus::ConnectionStatus(QWidget *parent)
    : QWidget(parent),
      ui_(new Ui::ConnectionStatus)
{
    ui_->setupUi(this);
    ui_->connectionStatus->setReadOnly(true);
    QWidget::connect(ui_->continueButton, &QAbstractButton::pressed, this, &ConnectionStatus::Continue );
    QWidget::connect(ui_->cancelButton, &QAbstractButton::pressed, this, &ConnectionStatus::Cancel );
}

ConnectionStatus::~ConnectionStatus()
{
    delete ui_;
}


void ConnectionStatus::DisplayText(QString text)
{
    ui_->connectionStatus->setText(text);
}


