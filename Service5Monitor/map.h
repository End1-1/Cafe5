#ifndef MAP_H
#define MAP_H

#include "socketwidget.h"

namespace Ui {
class Map;
}

class Map : public SocketWidget
{
    Q_OBJECT

public:
    explicit Map(QWidget *parent = nullptr);
    ~Map();

protected slots:
    virtual void externalDataReceived(quint16 cmd, const QByteArray &data);

private:
    Ui::Map *ui;
    QObject *fMapObject;
};

#endif // MAP_H
