#ifndef C5ORDER_H
#define C5ORDER_H

#include <QJsonObject>
#include <QJsonArray>

class C5SocketHandler;

class C5Order
{
public:
    C5Order();
    void addItem(const QJsonObject &obj);
    QString itemValue(int index, const QString &name);
    void setItemValue(int index, const QString &name, const QString &value);
    QString headerValue(const QString &name);
    void setHeaderValue(const QString &name, const QString &value);
    void setHeaderValue(const QString &name, int value);
    void setHeaderValue(const QString &name, double value);
    void save(C5SocketHandler *sh);
    void countTotal();
    QJsonArray fItems;
    QJsonObject fHeader;
private:
    bool fSaved;
};

#endif // C5ORDER_H
