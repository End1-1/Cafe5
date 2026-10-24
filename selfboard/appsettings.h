#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>

class AppSettings
{
public:
    static void loadFromSettings();
    static void saveToSettings();

    static bool paymentTestStub();
    static void setPaymentTestStub(bool enabled);

    static int tableTakeAway();
    static void setTableTakeAway(int tableId);

    static int tableDineIn();
    static void setTableDineIn(int tableId);

    static int cashboxId();
    static void setCashboxId(int cashboxId);

    static double serviceFactor();
    static void setServiceFactor(double factor);

    /** Base URL for guest order-status QR. Path form ends with /, query form ends with token=. */
    static QString orderStatusUrlBase();
    static void setOrderStatusUrlBase(const QString &url);

    /** QRNeto business id stored on the order at close. */
    static QString trackingRestaurantId();
    static void setTrackingRestaurantId(const QString &id);

    /** QRNeto location id stored on the order at close. */
    static QString trackingLocationId();
    static void setTrackingLocationId(const QString &id);

    /** Minutes added to open time for estimated_ready_at. */
    static int trackingEtaMinutes();
    static void setTrackingEtaMinutes(int minutes);

private:
    static AppSettings &instance();

    bool m_paymentTestStub = false;
    int m_tableTakeAway = 0;
    int m_tableDineIn = 0;
    int m_cashboxId = 0;
    double m_serviceFactor = 0.0;
    QString m_orderStatusUrlBase = QStringLiteral("https://www.ararix.com/get-order-status/");
    QString m_trackingRestaurantId;
    QString m_trackingLocationId;
    int m_trackingEtaMinutes = 15;
};

#endif // APPSETTINGS_H
