#include "outgoingmsg.h"

OutgoingMsg::OutgoingMsg(QObject *parent)
    : QObject{parent}
{}

OutgoingMsg::~OutgoingMsg()
{
}

void OutgoingMsg::Start()
{
    message_->setType(spb::remote::MsgTypeGadget::CONNECT);
}

void OutgoingMsg::Send()
{

}
