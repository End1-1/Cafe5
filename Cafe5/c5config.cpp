#include "c5config.h"
#include "c5database.h"
#include <QDir>
#include <QMutex>
#include <QDialog>
#include <QSettings>

QString C5Config::fAppHomePath;
QString C5Config::fAppLogFile;
QString C5Config::fSettingsName;
QString C5Config::fLastUsername;
QWidget *C5Config::fParentWidget = nullptr;
QString C5Config::fDBHost;
QString C5Config::fDBPath;
QString C5Config::fDBUser;
QString C5Config::fDBPassword;
QString C5Config::fFullScreen;
C5Config __c5config;
int __userid;
int __usergroup;
QString __username;
QStringList __databases;
QMap<int, QString> C5Config::fSettings;
static QMutex settingsMutex;

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

QString C5Config::taxUseExtPos()
{
    return getValue(param_tax_use_ext_pos);
}

QString C5Config::serviceFactor()
{
    return getValue(param_service_factor);
}

void C5Config::setServiceFactor(const QString &value)
{
    setValue(param_service_factor, value);
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

int C5Config::docNumDigitsInput()
{
    return getValue(param_doc_num_digits_input).toInt();
}

int C5Config::docNumDigitsMove()
{
    return getValue(param_doc_num_digits_move).toInt();
}

int C5Config::docNumDigitsOut()
{
    return getValue(param_doc_num_digits_out).toInt();
}

int C5Config::defaultStore()
{
    return getValue(param_default_store).toInt();
}

bool C5Config::autoDateCash()
{
    return getValue(param_date_cash_auto).toInt() == 1;
}

QString C5Config::dateCash()
{
    return getValue(param_date_cash);
}

int C5Config::dateShift()
{
    return getValue(param_date_cash_shift).toInt();
}

bool C5Config::useHotel()
{
    return !getValue(param_hotel_database).isEmpty();
}

QString C5Config::hotelDatabase()
{
    return getValue(param_hotel_database);
}

QString C5Config::logDatabase()
{
    return "airlog";
}

QString C5Config::serverIP()
{
    return getRegValue("server_ip").toString();
}

bool C5Config::noCashDocStore()
{
    return getValue(param_no_cash_doc_store) == "1";
}

bool C5Config::noScanCodeStore()
{
    return getValue(param_no_scancode_store) == "1";
}

void C5Config::setServerIP(const QString &ip)
{
    setRegValue("server_ip", ip);
}

int C5Config::defaultMenu()
{
    return getValue(param_default_menu).toInt();
}

QString C5Config::defaultMenuName()
{
    return getValue(param_default_menu_name);
}

QString C5Config::defaultHall()
{
    QStringList halls = getValue(param_default_hall).split(",", QString::SkipEmptyParts);
    if (halls.count() > 0) {
        return halls.at(0);
    } else {
        return "";
    }
}

QString C5Config::hallList()
{
    return getValue(param_default_hall);
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

QVariant C5Config::getRegValue(const QString &key, const QVariant &defaultValue)
{
    QSettings s(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);
    return s.value(key, defaultValue);
}

void C5Config::setRegValue(const QString &key, const QVariant &value)
{
    QSettings s(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);
    s.setValue(key, value);
}

void C5Config::setValue(int key, const QString &value)
{
    QMutexLocker ml(&settingsMutex);
    fSettings[key] = value;
}

bool C5Config::isAppFullScreen()
{
    return fFullScreen == "1";
}

int C5Config::fronDeskFontSize()
{
    int s = getValue(param_fd_font_size).toInt();
    if (s == 0) {
        s = 8;
    }
    return s;
}

bool C5Config::alwaysOfferTaxPrint()
{
    return getValue(param_tax_print_always_offer).toInt() == 1;
}

bool C5Config::carMode()
{
    bool val = getValue(param_car_mode).toInt() == 1;
    return val;
}

int C5Config::cashId()
{
    return getValue(param_cash_id).toInt();
}

QString C5Config::getValue(int key)
{
    QMutexLocker ml(&settingsMutex);
    return fSettings[key];
}
