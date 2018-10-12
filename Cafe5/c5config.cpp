#include "c5config.h"
#include <QDir>

QString C5Config::fAppHomePath;
QString C5Config::fAppLogFile;
QString C5Config::fSettingsName;
QString C5Config::fLastUsername;
QWidget *C5Config::fParentWidget = 0;
C5Config __c5config;
int __userid;
int __usergroup;
QString __username;
QStringList __databases;

C5Config::C5Config()
{
    QDir d;
    fAppHomePath = d.homePath() + "\\" + _APPLICATION_ + "\\" + _MODULE_;
    if (!d.exists(fAppHomePath)) {
        d.mkpath(fAppHomePath);
    }
    fAppLogFile = fAppHomePath + "\\logs.log";
}
