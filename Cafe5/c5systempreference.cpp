#include "c5systempreference.h"
#include <Windows.h>
#include <QLocale>
#include <QMessageBox>

C5SystemPreference::C5SystemPreference(QObject *parent) : QObject(parent)
{

}

bool C5SystemPreference::checkDecimalPointAndSeparator()
{
    QLocale l;
    if (l.decimalPoint() != '.' || l.groupSeparator() != ' ') {
        if (QMessageBox::warning(nullptr, "Locale", "Program using '.' as decimal point and white space as group separator.\r\nYou need to change your system locale or quit.\r\nChange your system locale?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
            LPWSTR decimal = const_cast<LPWSTR>(TEXT("."));
            LPWSTR group = const_cast<LPWSTR>(TEXT(" "));
            SetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, decimal);
            SetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, group);
            return true;
        } else {
            return false;
        }
    }
    return true;
}
