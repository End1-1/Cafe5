#include "c5config.h"
#include <QDir>

QString C5Config::fAppHomePath;
QString C5Config::fAppLogFile;
QString C5Config::fSettingsName;
QWidget *C5Config::fParentWidget = 0;
C5Config __c5config;

C5Config::C5Config()
{
    QDir d;
    fAppHomePath = d.homePath() + "\\" + _APPLICATION_ + "\\" + _MODULE_;
    if (!d.exists(fAppHomePath)) {
        d.mkpath(fAppHomePath);
    }
    fAppLogFile = fAppHomePath + "\\logs.log";
}
