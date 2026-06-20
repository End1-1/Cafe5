#ifndef SELFBOARDSETTINGS_H
#define SELFBOARDSETTINGS_H

#include <QSettings>

class SelfBoardSettings
{
public:
    static void configureStorage();
    static QSettings store();
    static void flush();
};

#endif // SELFBOARDSETTINGS_H
