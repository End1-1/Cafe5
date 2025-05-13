#include "waiter.h"
#include <QWebSocket>

Waiter::Waiter(const QJsonObject &jdoc) :
    mDoc(jdoc)
{
}

QString Waiter::process()
{
    return "";
}
