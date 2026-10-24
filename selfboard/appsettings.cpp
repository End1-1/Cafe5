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
    cfg.m_orderStatusUrlBase = s.value(
                                    QStringLiteral("orderStatusUrlBase"),
                                    QStringLiteral("https://www.ararix.com/get-order-status/"))
                                   .toString();
    cfg.m_trackingRestaurantId = s.value(QStringLiteral("trackingRestaurantId")).toString();
    cfg.m_trackingLocationId = s.value(QStringLiteral("trackingLocationId")).toString();
    cfg.m_trackingEtaMinutes = s.value(QStringLiteral("trackingEtaMinutes"), 15).toInt();
    if (cfg.m_trackingEtaMinutes < 0) {
        cfg.m_trackingEtaMinutes = 0;
    }
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
    s.setValue(QStringLiteral("orderStatusUrlBase"), cfg.m_orderStatusUrlBase);
    s.setValue(QStringLiteral("trackingRestaurantId"), cfg.m_trackingRestaurantId);
    s.setValue(QStringLiteral("trackingLocationId"), cfg.m_trackingLocationId);
    s.setValue(QStringLiteral("trackingEtaMinutes"), cfg.m_trackingEtaMinutes);
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

QString AppSettings::orderStatusUrlBase()
{
    return instance().m_orderStatusUrlBase;
}

void AppSettings::setOrderStatusUrlBase(const QString &url)
{
    instance().m_orderStatusUrlBase = url.trimmed();
}

QString AppSettings::trackingRestaurantId()
{
    return instance().m_trackingRestaurantId;
}

void AppSettings::setTrackingRestaurantId(const QString &id)
{
    instance().m_trackingRestaurantId = id.trimmed();
}

QString AppSettings::trackingLocationId()
{
    return instance().m_trackingLocationId;
}

void AppSettings::setTrackingLocationId(const QString &id)
{
    instance().m_trackingLocationId = id.trimmed();
}

int AppSettings::trackingEtaMinutes()
{
    return instance().m_trackingEtaMinutes;
}

void AppSettings::setTrackingEtaMinutes(int minutes)
{
    instance().m_trackingEtaMinutes = minutes < 0 ? 0 : minutes;
}
