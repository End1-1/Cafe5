#include "connections.h"
#include "ui_connections.h"
#include "messagelist.h"
#include "rawmessage.h"
#include <QDebug>

Connections::Connections(QWidget *parent) :
    SocketWidget(parent),
    ui(new Ui::Connections)
{
    ui->setupUi(this);
}

Connections::~Connections()
{
    delete ui;
}

void Connections::externalDataReceived(quint16 cmd, const QByteArray &data)
{
    RawMessage r(nullptr, data);
    switch (cmd) {
    case MessageList::srv_connections_count:
        ui->lbConnections->setText(QString::number(r.readUInt()));
        break;
    }
}

void Connections::connectionLost()
{
    ui->lbConnections->setText(tr("Not connected"));
}
