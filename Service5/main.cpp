#include "monitor.h"
#include "monitoringwindow.h"
#include "requestmanager.h"
#include "databaseconnectionmanager.h"
#include "configini.h"
#include "storemanager.h"
#include "shopmanager.h"
#include <QApplication>
#include <QStandardPaths>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("NewStartSoft");
    QCoreApplication::setApplicationName("Service5");
    QApplication a(argc, argv);

    QTranslator t;
    t.load(":/Service5.qm");
    a.installTranslator(&t);

    MonitoringWindow::init();
    ConfigIni::init();
    DatabaseConnectionManager::init();
    if (ConfigIni::isTrue("store/init")) {
        StoreManager::init(ConfigIni::value("store/db"));
    }
    if (ConfigIni::isTrue("shop/init")) {
        ShopManager::init(ConfigIni::value("shop/db"));
    }
    RequestManager::init();

    Monitor w;
    w.show();
    return a.exec();
}