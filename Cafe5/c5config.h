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
    static QString idramID();
    static QString idramPhone();
    static QString taxIP();
    static int taxPort();
    static QString taxPassword();
    static QString taxDept();
    static QStringList dbParams();
    static void initParamsFromDb();

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
#endif

#endif // C5CONFIG_H
