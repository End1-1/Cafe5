#ifndef C5CONFIG_H
#define C5CONFIG_H

#include <QString>
#include <QWidget>

#define param_local_receipt_printer 1
#define param_service_factor 2
#define param_idram_id 3
#define param_idram_phone 4
#define param_tax_ip 5
#define param_tax_port 6
#define param_tax_password 7
#define param_tax_dept 8
#define param_default_menu 9
#define param_default_hall 11
#define param_default_table 12
#define param_doc_num_digits 13
#define param_default_store 14
#define param_tax_use_ext_pos 15
#define param_default_menu_name 16
#define param_hotel_database 17

class C5Config
{
public:
    C5Config();

    static QString fAppHomePath;

    static QString fAppLogFile;

    static QString fSettingsName;

    static QWidget *fParentWidget;

    static QString fLastUsername;

    static QString fDBHost;

    static QString fDBPath;

    static QString fDBUser;

    static QString fDBPassword;

    static QString localReceiptPrinter();

    static QString serviceFactor();

    static void setServiceFactor(const QString &value);

    static QString idramID();

    static QString idramPhone();

    static QString taxIP();

    static int taxPort();

    static QString taxPassword();

    static QString taxDept();

    static QString taxUseExtPos();

    static QStringList dbParams();

    static int defaultMenu();

    static QString defaultMenuName();

    static QString defaultHall();

    static QString hallList();

    static int defaultTable();

    static int docNumDigits();

    static int defaultStore();

    static bool useHotel();

    static QString hotelDatabase();

    static QString serverIP();

    static void setServerIP(const QString &ip);

    static void initParamsFromDb();

    static QVariant getRegValue(const QString &key);

    static void setRegValue(const QString &key, const QVariant &value);

    static void setValue(int key, const QString &value);

private:
    static QString getValue(int key);

    static QMap<int, QString> fSettings;
};

extern C5Config __c5config;

extern int __userid;

extern int __usergroup;

extern QString __username;

extern QStringList __databases;

#ifdef FRONTDESK
#include "c5mainwindow.h"
extern C5MainWindow *__mainWindow;
#else
extern QDialog *__mainWindow;
#endif

#endif // C5CONFIG_H
