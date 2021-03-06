#ifndef C5CONFIG_H
#define C5CONFIG_H

#include <QString>
#include <QVariant>

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
#define param_doc_num_digits_input 13
#define param_default_store 14
#define param_tax_use_ext_pos 15
#define param_default_menu_name 16
#define param_hotel_database 17
#define param_cafe_service_mode 18
#define param_date_cash_auto 19
#define param_date_cash 20
#define param_item_code_for_hotel 21
#define param_doc_num_digits_move 22
#define param_doc_num_digits_out 23
#define param_no_cash_doc_store 24
#define param_no_scancode_store 25
#define param_working_date_change_time 26
#define param_tax_print_always_offer 27
#define param_fd_font_size 28
#define param_recipe_widht 29
#define param_car_mode 30
#define param_date_cash_shift 31
#define param_cash_id 32
#define param_frondesk_mode 33
#define param_shop_enterpin 34
#define param_nocash_id 35
#define param_autocash_prefix 36
#define param_autonocash_prefix 37
#define param_autoinput_salecash 38
#define param_tax_cashier_login 39
#define param_tax_cashier_pin 40
#define param_control_shop_qty 41
#define param_rootdb_replica 42
#define param_rootdb_host 43
#define param_rootdb_schema 44
#define param_rootdb_user 45
#define param_rootdb_pass 46
#define param_shop_noF1 47
#define param_shop_noF2 48
#define param_shop_noPriceChange 49
#define param_shop_defferentStaff 50
#define param_waiter_login_after_payment 51
#define param_shop_print_v1 52
#define param_shop_print_v2 53
#define param_shop_autologin_pin1 54
#define param_shop_autologin_pin2 55
#define param_recipe_footer_text 56
#define param_input_doc_fix_price 57

#define FRONTDESK_WAITER 0
#define FRONTDESK_SHOP 1

#define SALE_RETAIL 1
#define SALE_WHOSALE 2
#define SALE_PREORDER 3

class C5Config
{
public:
    C5Config();

    static QString fAppHomePath;

    static QString fAppLogFile;

    static QString fSettingsName;

    static QWidget *fParentWidget;

    static QString fLastUsername;

    static QString fFullScreen;

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

    static QStringList replicaDbParams();

    static int defaultMenu();

    static QString defaultMenuName();

    static QString defaultHall();

    static QString hallList();

    static int defaultTable();

    static int docNumDigitsInput();

    static int docNumDigitsMove();

    static int docNumDigitsOut();

    static int defaultStore();

    static bool autoDateCash();

    static QString dateCash();

    static int dateShift();

    static bool useHotel();

    static QString hotelDatabase();

    static QString logDatabase();

    static QString serverIP();

    static bool noCashDocStore();

    static bool noScanCodeStore();

    static void setServerIP(const QString &ip);

    static void initParamsFromDb();

    static QVariant getRegValue(const QString &key, const QVariant &defaultValue = QVariant());

    static void setRegValue(const QString &key, const QVariant &value);

    static void setValue(int key, const QString &value);

    static bool isAppFullScreen();

    static int fronDeskFontSize();

    static bool alwaysOfferTaxPrint();

    static bool carMode();

    static bool autoCashInput();

    static int cashId();

    static int nocashId();

    static QString cashPrefix();

    static QString nocashPrefix();

    static int frontDeskMode();

    static bool shopEnterPin();

    static QString taxCashier();

    static QString taxPin();

    static bool controlShopQty();

    static bool rdbReplica();

    static QString rdbHost();

    static QString rdbSchema();

    static QString rdbUser();

    static QString rdbPassword();

    static bool shopDenyF1();

    static bool shopDenyF2();

    static bool shopDenyPriceChange();

    static bool shopDifferentStaff();

    static bool waiterLoginAfterPayment();

    static int shopPrintVersion();

    static QString getValue(int key);

    static void setValues(const QMap<int, QString> &values);

private:
    static QMap<int, QString> fSettings;
};

extern QStringList __autologin_store;

extern C5Config __c5config;

extern int __userid;

extern int __usergroup;

extern QString __username;

extern QList<int> __userpermissions;

extern QStringList __databases;


#endif // C5CONFIG_H
