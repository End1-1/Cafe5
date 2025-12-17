#include "c5waiterconfiguration.h"

C5WaiterConfiguration* C5WaiterConfiguration::mInstance = nullptr;

C5WaiterConfiguration::C5WaiterConfiguration()
{
}

int C5WaiterConfiguration::defaultHall()
{
    return mData["default_hall"].toInt();
}

int C5WaiterConfiguration::defaultMenu()
{
    return mData["default_menu"].toInt();
}

QString C5WaiterConfiguration::receiptPrinter()
{
    return mData["receipt_printer"].toString();
}

C5WaiterConfiguration* C5WaiterConfiguration::instance()
{
    Q_ASSERT(mInstance);
    return mInstance;
}

void C5WaiterConfiguration::init(const QJsonObject &jo)
{
    mInstance = new C5WaiterConfiguration();
    mInstance->mData = jo;
}
