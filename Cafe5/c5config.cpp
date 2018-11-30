#include "c5config.h"
#include "c5database.h"
#include <QDir>
#include <QMutex>
#include <QSettings>

QString C5Config::fAppHomePath;
QString C5Config::fAppLogFile;
QString C5Config::fSettingsName;
QString C5Config::fLastUsername;
QWidget *C5Config::fParentWidget = 0;
QString C5Config::fDBHost;
QString C5Config::fDBPath;
QString C5Config::fDBUser;
QString C5Config::fDBPassword;
C5Config __c5config;
int __userid;
int __usergroup;
QString __username;
QStringList __databases;
QMap<int, QString> C5Config::fSettings;
QMutex settingsMutex;

#ifdef FRONTDESK
C5MainWindow *__mainWindow;
#endif

C5Config::C5Config()
{
    QDir d;
    fAppHomePath = d.homePath() + "\\" + _APPLICATION_ + "\\" + _MODULE_;
    if (!d.exists(fAppHomePath)) {
        d.mkpath(fAppHomePath);
    }
    fAppLogFile = fAppHomePath + "\\logs.log";
}

QString C5Config::localReceiptPrinter()
{
    return getValue(param_local_receipt_printer);
}

QString C5Config::idramID()
{
    return getValue(param_idram_id);
}

QString C5Config::idramPhone()
{
    return getValue(param_idram_phone);
}

QString C5Config::taxIP()
{
    return getValue(param_tax_ip);
}

int C5Config::taxPort()
{
    return getValue(param_tax_port).toInt();
}

QString C5Config::taxPassword()
{
    return getValue(param_tax_password);
}

QString C5Config::taxDept()
{
    return getValue(param_tax_dept);
}

QString C5Config::serviceFactor()
{
    return getValue(param_service_factor);
}

QStringList C5Config::dbParams()
{
    QStringList params;
    params << C5Config::fDBHost
             << C5Config::fDBPath
             << C5Config::fDBUser
             << C5Config::fDBPassword;
    return params;
}

QString C5Config::orderPrefix()
{
    return getValue(param_order_prefix);
}

int C5Config::docNumDigits()
{
    return getValue(param_doc_num_digits).toInt();
}

int C5Config::defaultMenu()
{
    return getValue(param_default_menu).toInt();
}

int C5Config::defaultHall()
{
    return getValue(param_default_hall).toInt();
}

int C5Config::defaultTable()
{
    return getValue(param_default_table).toInt();
}

void C5Config::initParamsFromDb()
{
    C5Database db(fDBHost, fDBPath, fDBUser, fDBPassword);
    fSettings.clear();
    db[":f_name"] = fSettingsName;
    db.exec("select f_id from s_settings_names where f_name=:f_name");
    if (db.nextRow()) {
        db[":f_settings"] = db.getInt(0);
        db.exec("select f_key, f_value from s_settings_values where f_settings=:f_settings");
        while (db.nextRow()) {
            fSettings[db.getInt(0)] = db.getString(1);
        }
    }
}

QVariant C5Config::getRegValue(const QString &key)
{
    QSettings s(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);
    return s.value(key);
}

void C5Config::setRegValue(const QString &key, const QVariant &value)
{
    QSettings s(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);
    s.setValue(key, value);
}

QString C5Config::getValue(int key)
{
    QMutexLocker ml(&settingsMutex);
    return fSettings[key];
}
