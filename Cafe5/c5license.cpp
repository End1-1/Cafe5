#include "c5license.h"
#include <QDate>

static const QString settingsName;

C5License::C5License()
{

}

bool C5License::isOK()
{
    if (QDate::currentDate() > QDate::fromString("11.02.2020", "dd.MM.yyyy")) {
        return false;
    }
    return true;    
}
