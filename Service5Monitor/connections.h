#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include "socketwidget.h"

namespace Ui {
class Connections;
}

class Connections : public SocketWidget
{
    Q_OBJECT

public:
    explicit Connections(QWidget *parent = nullptr);
    ~Connections();

protected slots:
    virtual void externalDataReceived(quint16 cmd, const QByteArray &data);
    virtual void connectionLost() override;

private:
    Ui::Connections *ui;
};

#endif // CONNECTIONS_H
