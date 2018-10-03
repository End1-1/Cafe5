#include "c5order.h"
#include "c5sockethandler.h"
#include "c5socketmessage.h"
#include <QDebug>

C5Order::C5Order()
{
    fSaved = true;
}

void C5Order::addItem(const QJsonObject &obj)
{
    fItems.append(obj);
    fSaved = false;
}

QString C5Order::itemValue(int index, const QString &name)
{
    return fItems.at(index)[name].toString();
}

void C5Order::setItemValue(int index, const QString &name, const QString &value)
{
    fItems[index].toObject()[name] = value;
    fSaved = false;
}

QString C5Order::headerValue(const QString &name)
{
    return fHeader[name].toString();
}

void C5Order::setHeaderValue(const QString &name, const QString &value)
{
    fHeader[name] = value;
    fSaved = false;
}

void C5Order::setHeaderValue(const QString &name, int value)
{
    setHeaderValue(name, QString::number(value));
}

void C5Order::save(C5SocketHandler *sh)
{
    if (fSaved) {
        return;
    }
    sh->bind("cmd", sm_saveorder);
    QJsonObject o;
    o["header"] = fHeader;
    o["body"] = fItems;
    sh->send(o);
}
