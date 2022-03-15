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
    RawMessage r(nullptr);
    switch (cmd) {
    case MessageList::srv_connections_count:
        quint32 user, count;
        quint8 state;
        r.readUInt(count, data);
        r.readUInt(user, data);
        r.readUByte(state, data);
        ui->lbConnections->setText(QString::number(count));
        break;
    }
}

void Connections::connectionLost()
{
    ui->lbConnections->setText(tr("Not connected"));
}
