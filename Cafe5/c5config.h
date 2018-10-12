#ifndef C5CONFIG_H
#define C5CONFIG_H

#include <QString>
#include <QWidget>

class C5Config
{
public:
    C5Config();
    static QString fAppHomePath;
    static QString fAppLogFile;
    static QString fSettingsName;
    static QWidget *fParentWidget;
    static QString fLastUsername;
};

extern C5Config __c5config;
extern int __userid;
extern int __usergroup;
extern QString __username;
extern QStringList __databases;

#endif // C5CONFIG_H
