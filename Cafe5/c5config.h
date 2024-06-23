#ifndef C5CONFIG_H
#define C5CONFIG_H

#include <QString>
#include <QVariant>
#include <QSslCertificate>

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
#define param_date_cash_shift 31
#define param_cash_id 32
#define param_frondesk_mode 33
#define param_nocash_id 35
#define param_autocash_prefix 36
#define param_autonocash_prefix 37
#define param_autoinput_salecash 38
#define param_tax_cashier_login 39
#define param_tax_cashier_pin 40
#define param_rootdb_replica 42
#define param_rootdb_host 43
#define param_rootdb_schema 44
#define param_rootdb_user 45
#define param_rootdb_pass 46
#define param_shop_noF1 47
#define param_shop_noF2 48
#define param_shop_noPriceChange 49
#define param_shop_defferentStaff 50
#define param_shop_print_v1 52
#define param_shop_print_v2 53
#define param_recipe_footer_text 56
#define param_input_doc_fix_price 57
#define param_shop_print_goods_qty_side_left 58
#define param_shop_print_goods_qty_side_down 59
#define param_print_scancode_with_name 60
#define param_print_paper_width 61
#define param_print_dish_timeorder 62
#define param_printer_paper_width_50mm 63
#define param_printer_parer_width_80mm 64
#define param_http_server_ip 65
#define param_http_server_port 66
#define param_http_server_user 67
#define param_http_server_pass 68
#define param_as_connection_string 69
#define param_hotel_user_Id 70
#define param_hotel_hall_id 71
#define param_hotel_noinvoice 72
#define param_static_qr_text 73
#define param_idram_tips 74
#define param_idram_tips_wallet 75
#define param_idram_name 76
#define param_idram_session_id 77
#define param_no_tax 78
#define param_waiter_closeorder_after_serviceprint 79
#define param_frontdesk_scale_dir 80
#define param_shop_deny_qtychange 81
#define param_shop_print_dontgroup 82
#define param_debuge_mode 83
#define param_idram_server 84
#define param_waiter_close_order_after_precheck 85
#define param_shop_dont_check_qty 86
#define param_vat 87
#define param_shop_report_of_logged_staff 88
#define param_shop_hide_store_qty 89
#define param_shop_config_mobile_client 90
#define param_fd_russian_scale1 91
#define param_shop_print_v3 92
#define param_shop_never_tax 93
#define param_fd_update_a_temp_store 94
#define param_waiter_automatially_storeout 95
#define param_waiter_ask_for_precheck 96
#define param_app_font_family 97
#define param_waiter_receipt_qty 98
#define param_smart_dont_print_receipt 99
#define param_force_use_print_alias 100
#define param_zip_dish_in_order 101
#define param_service_print_font_family 102
#define param_receipt_print_font_family 103
#define param_service_print_font_size 104
#define param_receipt_print_font_size 105
#define param_system_thouthand_separator 106
#define param_tax_print_if_amount_less 107
#define param_print_modificators_on_receipt 108
#define param_waiter_donotclose_empty_order 109
#define param_waiter_dontprint_final_receipt 110
#define param_waiter_dish_after_day_change 111
#define param_waiter_hall_chart 112
#define param_choose_external_pos 113
#define param_mobile_app_connection 114
#define param_shop_scalecode 115
#define param_default_currency 116
#define param_rest_qty_before_add_dish 117
#define param_auto_discount 118
#define param_waiter_delivery_costumer_category 119
#define param_waiter_debt_costumer_category 120

#define FRONTDESK_WAITER 0
#define FRONTDESK_SHOP 1

#define SALE_RETAIL 1
#define SALE_WHOSALE 2
#define SALE_RETURN 4

class C5User;

class C5Config
{
public:
    C5Config();

    static QString fAppHomePath;

    static QString fAppLogFile;

    static QString fSettingsName;

    static QWidget *fParentWidget;

    static QString fLastUsername;

    static bool fFullScreen;

    static QString fDBHost;

    static QString fDBPath;

    static QString fDBUser;

    static QString fDBPassword;

    static int fJsonConfigId;

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

    static int defaultHall();

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

    static bool autoCashInput();

    static int cashId();

    static int nocashId();

    static QString cashPrefix();

    static QString nocashPrefix();

    static int frontDeskMode();

    static QString taxCashier();

    static QString taxPin();

    static bool rdbReplica();

    static QString rdbHost();

    static QString rdbSchema();

    static QString rdbUser();

    static QString rdbPassword();

    static bool shopDenyF1();

    static bool shopDenyF2();

    static bool shopDenyPriceChange();

    static bool shopDifferentStaff();

    static int shopPrintVersion();

    static int receiptParepWidth();

    static int receipPrinterWidth();

    static QString httpServerIP();

    static int httpServerPort();

    static QString httpServerUsername();

    static QString httpServerPassword();

    static QString getValue(int key);

    static void setValues(const QMap<int, QString> &values);

private:
    static QMap<int, QString> fSettings;
};

extern C5Config __c5config;

extern QStringList __databases;

extern C5User *__user;

#endif // C5CONFIG_H
