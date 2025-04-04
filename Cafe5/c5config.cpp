#include "c5config.h"
#include "c5database.h"
#include "c5user.h"
#include <QDir>
#include <QMutex>
#include <QDebug>
#include <QJsonDocument>
#include <QSettings>
#include <QApplication>

QString C5Config::fAppHomePath;
QString C5Config::fAppLogFile;
QString C5Config::fSettingsName;
int C5Config::fSettingsId;
QString C5Config::fLastUsername;
QWidget *C5Config::fParentWidget = nullptr;
QString C5Config::fDBName;
QString C5Config::fDBHost;
QString C5Config::fDBPath;
QString C5Config::fDBUser;
QString C5Config::fDBPassword;
int C5Config::fJsonConfigId = 1;
bool C5Config::fFullScreen;
C5Config __c5config;
C5User *__user;
QStringList __databases;
QMap<int, QString> C5Config::fSettings;
QJsonObject C5Config::fMainJson;

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
    QString value = getValue(param_tax_use_ext_pos).toLower();
    return  value;
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

int C5Config::defaultHall()
{
    QStringList halls = getValue(param_default_hall).split(",", Qt::SkipEmptyParts);
    if (halls.count() > 0) {
        return halls.at(0).toInt();
    } else {
        return 0;
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
        fSettingsId = db.getInt(0);
        db[":f_settings"] = db.getInt(0);
        db.exec("select f_key, f_value from s_settings_values where f_settings=:f_settings");
        while (db.nextRow()) {
            fSettings[db.getInt(0)] = db.getString(1);
        }
    }
    db[":f_id"] = __c5config.fSettingsId;
    db.exec("select * from sys_json_config where f_id=:f_id");
    if (db.nextRow()) {
        fMainJson = QJsonDocument::fromJson(db.getString("f_config").toUtf8()).object();
    }
}

QVariant C5Config::getRegValue(const QString &key, const QVariant &defaultValue)
{
    QSettings s(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);
    return s.value(key, defaultValue);
}

void C5Config::setRegValue(const QString &key, const QVariant &value)
{
    QSettings s(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);
    s.setValue(key, value);
}

void C5Config::setValue(int key, const QString &value)
{
    QMutexLocker ml( &settingsMutex);
    fSettings[key] = value;
}

bool C5Config::isAppFullScreen()
{
    return fFullScreen;
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

bool C5Config::autoCashInput()
{
    return getValue(param_autoinput_salecash).toInt() == 1;
}

int C5Config::cashId()
{
    return getValue(param_cash_id).toInt();
}

int C5Config::nocashId()
{
    return getValue(param_nocash_id).toInt();
}

QString C5Config::cashPrefix()
{
    return getValue(param_autocash_prefix);
}

QString C5Config::nocashPrefix()
{
    return getValue(param_autonocash_prefix);
}

int C5Config::frontDeskMode()
{
    return getValue(param_frondesk_mode).toInt();
}

QString C5Config::taxCashier()
{
    return getValue(param_tax_cashier_login);
}

QString C5Config::taxPin()
{
    return getValue(param_tax_cashier_pin);
}

bool C5Config::shopDenyF1()
{
    return getValue(param_shop_noF1).toInt() == 1;
}

bool C5Config::shopDenyF2()
{
    return getValue(param_shop_noF2).toInt() == 1;
}

bool C5Config::shopDenyPriceChange()
{
    return getValue(param_shop_noPriceChange).toInt() == 1;
}

bool C5Config::shopDifferentStaff()
{
    return getValue(param_shop_defferentStaff).toInt() == 1;
}

int C5Config::shopPrintVersion()
{
    /*
     * 0 - no print
     * 1 - v1
     * 2 - v2
     * 3 - v3
     */
    if (getValue(param_shop_print_v1).toInt() == 1) {
        return 1;
    } else if (getValue(param_shop_print_v2).toInt() == 1) {
        return 2;
    } else if (getValue(param_shop_print_v3).toInt() == 1) {
        return 3;
    } else {
        return 0;
    }
}

int C5Config::receiptParepWidth()
{
    int w = getValue(param_print_paper_width).toInt();
    return w == 0 ? 650 : w;
}

QString C5Config::httpServerIP()
{
    return getValue(param_http_server_ip);
}

int C5Config::httpServerPort()
{
    return getValue(param_http_server_port).toInt();
}

QString C5Config::httpServerUsername()
{
    return getValue(param_http_server_user);
}

QString C5Config::httpServerPassword()
{
    return getValue(param_http_server_pass);
}

int C5Config::receipPrinterWidth()
{
    if (getValue(param_printer_paper_width_50mm).toInt() == 1) {
        return 50;
    }
    return 80;
}

QString C5Config::getValue(int key)
{
    QMutexLocker ml( &settingsMutex);
    return fSettings[key];
}

void C5Config::setValues(const QMap<int, QString> &values)
{
    fSettings = values;
}
