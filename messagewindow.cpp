#include "messagewindow.h"
#include "ui_messagewindow.h"

MessageWindow::MessageWindow(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::MessageWindow)
{
    ui_->setupUi(this);
    QWidget::connect(ui_->okButton, &QAbstractButton::pressed, this, &MessageWindow::Ok);
}

MessageWindow::~MessageWindow()
{
    delete ui_;
}

void MessageWindow::Init()
{
}

void MessageWindow::DisplayText(QString message)
{
    ui_->messageText->setText(message);
}
