#include "c5systempreference.h"
#include <Windows.h>
#include <QLocale>
#include <QMessageBox>

C5SystemPreference::C5SystemPreference(QObject *parent) : QObject(parent)
{
}

bool C5SystemPreference::checkDecimalPointAndSeparator()
{
    // QLocale l;
    // if (l.decimalPoint() != '.' || l.groupSeparator() != __c5config.getValue(param_system_thouthand_separator)) {
    //     if (QMessageBox::warning(nullptr, "Locale", QString("Program using '.' as decimal point and '%1' as group separator.\r\nYou need to change your system locale or quit.\r\nChange your system locale?").arg(__c5config.getValue(param_system_thouthand_separator)), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
    //         LPWSTR decimal = const_cast<LPWSTR>(TEXT("."));
    //         LPWSTR group = const_cast<LPWSTR>((const wchar_t*) __c5config.getValue(param_system_thouthand_separator).utf16());
    //         SetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, decimal);
    //         SetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, group);
    //         QMessageBox::information(nullptr, "Locale", "Please, restart your application to make changes.");
    //         return true;
    //     } else {
    //         return false;
    //     }
    // }
    // return true;
    QLocale l;

    if(l.decimalPoint() != '.' || l.groupSeparator() != ',') {
        if(QMessageBox::warning(nullptr, "Locale", QString("Program using '.' as decimal point and '%1' as group separator.\r\nYou need to change your system locale or quit.\r\nChange your system locale?").arg(","), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
            LPWSTR decimal = const_cast<LPWSTR>(TEXT("."));
            LPWSTR group = const_cast<LPWSTR>((const wchar_t*) ",");
            SetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, decimal);
            SetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, group);
            QMessageBox::information(nullptr, "Locale", "Please, restart your application to make changes.");
            return true;
        } else {
            return false;
        }
    }

    return true;
}
