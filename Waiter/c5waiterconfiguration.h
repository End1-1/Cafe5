#pragma once

#include <QJsonObject>

class C5WaiterConfiguration
{
public:
    C5WaiterConfiguration();

    int defaultHall();

    int defaultMenu();

    QString receiptPrinter();

    static C5WaiterConfiguration* instance();

    static void init(const QJsonObject &jo);

private:
    QJsonObject mData;

    static C5WaiterConfiguration* mInstance;
};
