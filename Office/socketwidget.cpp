#include "socketwidget.h"
#include "socketconnection.h"

SocketWidget::SocketWidget(QWidget *parent) :
    QWidget(parent)
{
    connect(SocketConnection::instance(), &SocketConnection::externalDataReady, this, &SocketWidget::externalDataReceived);
    connect(SocketConnection::instance(), &SocketConnection::connectionLost, this, &SocketWidget::connectionLost);
    connect(this, &SocketWidget::dataReady, SocketConnection::instance(), &SocketConnection::sendData);
}

void SocketWidget::connectionLost()
{

}
