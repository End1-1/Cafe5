#ifndef APPSETTINGS_H
#define APPSETTINGS_H

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

private:
    static AppSettings &instance();

    bool m_paymentTestStub = false;
    int m_tableTakeAway = 0;
    int m_tableDineIn = 0;
    int m_cashboxId = 0;
    double m_serviceFactor = 0.0;
};

#endif // APPSETTINGS_H
