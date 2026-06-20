#include "appsettings.h"

#include "selfboardsettings.h"

#include <QSettings>

AppSettings &AppSettings::instance()
{
    static AppSettings cfg;
    return cfg;
}

void AppSettings::loadFromSettings()
{
    QSettings s = SelfBoardSettings::store();
    auto &cfg = instance();
    cfg.m_paymentTestStub = s.value(QStringLiteral("paymentTestStub"), false).toBool();
    cfg.m_tableTakeAway = s.value(QStringLiteral("tableTakeAway"), 0).toInt();
    cfg.m_tableDineIn = s.value(QStringLiteral("tableDineIn"), 0).toInt();
    cfg.m_cashboxId = s.value(QStringLiteral("cashboxId"), 0).toInt();
    cfg.m_serviceFactor = s.value(QStringLiteral("serviceFactor"), 0.0).toDouble();
}

void AppSettings::saveToSettings()
{
    const auto &cfg = instance();
    QSettings s = SelfBoardSettings::store();
    s.setValue(QStringLiteral("paymentTestStub"), cfg.m_paymentTestStub);
    s.setValue(QStringLiteral("tableTakeAway"), cfg.m_tableTakeAway);
    s.setValue(QStringLiteral("tableDineIn"), cfg.m_tableDineIn);
    s.setValue(QStringLiteral("cashboxId"), cfg.m_cashboxId);
    s.setValue(QStringLiteral("serviceFactor"), cfg.m_serviceFactor);
    SelfBoardSettings::flush();
}

bool AppSettings::paymentTestStub()
{
    return instance().m_paymentTestStub;
}

void AppSettings::setPaymentTestStub(bool enabled)
{
    instance().m_paymentTestStub = enabled;
}

int AppSettings::tableTakeAway()
{
    return instance().m_tableTakeAway;
}

void AppSettings::setTableTakeAway(int tableId)
{
    instance().m_tableTakeAway = tableId;
}

int AppSettings::tableDineIn()
{
    return instance().m_tableDineIn;
}

void AppSettings::setTableDineIn(int tableId)
{
    instance().m_tableDineIn = tableId;
}

int AppSettings::cashboxId()
{
    return instance().m_cashboxId;
}

void AppSettings::setCashboxId(int cashboxId)
{
    instance().m_cashboxId = cashboxId;
}

double AppSettings::serviceFactor()
{
    return instance().m_serviceFactor;
}

void AppSettings::setServiceFactor(double factor)
{
    instance().m_serviceFactor = factor;
}
