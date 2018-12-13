#include "c5order.h"
#include "c5sockethandler.h"
#include "c5socketmessage.h"
#include "c5utils.h"
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
    QJsonObject o = fItems[index].toObject();
    o[name.toLower()] = value;
    fItems[index] = o;
    fSaved = false;
}

QString C5Order::headerValue(const QString &name)
{
    return fHeader[name.toLower()].toString();
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

void C5Order::setHeaderValue(const QString &name, double value)
{
    setHeaderValue(name, QString::number(value, 'f', 2));
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

void C5Order::countTotal()
{
    double total = 0;
    double totalService = 0;
    double totalDiscount = 0;
    for (int i = 0, count = fItems.count(); i < count; i++) {
        setItemValue(i, "f_discount", headerValue("f_discountfactor"));
        setItemValue(i, "f_service", headerValue("f_servicefactor"));
        QJsonObject o = fItems[i].toObject();
        if (o["f_state"].toString().toInt() != DISH_STATE_OK) {
            continue;
        }
        double price = o["f_price"].toString().toDouble();
        double service = o["f_service"].toString().toDouble();
        double discount = o["f_discount"].toString().toDouble();
        double itemTotal = price * o["f_qty2"].toString().toDouble();
        double serviceAmount = (itemTotal * service);
        itemTotal += serviceAmount;
        double discountAmount = (itemTotal * discount);
        itemTotal -= discountAmount;
        total += itemTotal;
        totalService += serviceAmount;
        totalDiscount += discountAmount;
        o["f_total"] = QString::number(itemTotal, 'f', 2);
        fItems[i] = o;
    }
    setHeaderValue("f_amounttotal", total);
    setHeaderValue("f_amountservice", totalService);
    setHeaderValue("f_amountdiscount", totalDiscount);
    double acash = headerValue("f_amountcash").toDouble(),
            acard = headerValue("f_amountcard").toDouble(),
            abank = headerValue("f_amountbank").toDouble(),
            aother = headerValue("f_amountother").toDouble();
    double adiff = total - (acash + acard + abank + aother);
    if (adiff < 0.001) {
        setHeaderValue("f_amountother", 0);
        setHeaderValue("f_amountbank", 0);
        setHeaderValue("f_amountcard", 0);
        setHeaderValue("f_amountcash", headerValue("f_amounttotal"));
    }
}
