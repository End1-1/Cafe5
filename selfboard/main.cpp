#include <QApplication>

#include "dlgstart.h"
#include "menucache.h"
#include "selfboarddisplay.h"
#include "appsettings.h"
#include "selfboardsettings.h"
#include "serverconfig.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("Jazzve"));
    QCoreApplication::setApplicationName(QStringLiteral("SelfBoard"));
    SelfBoardSettings::configureStorage();

    ServerConfig::loadFromSettings();
    AppSettings::loadFromSettings();
    ServerConfig::applyToNetwork();
    MenuCache::configureNetwork();

    auto *start = new DlgStart();
    start->setAttribute(Qt::WA_DeleteOnClose);
    SelfBoardDisplay::showFullscreen(start);

    return app.exec();
}
