#include "c5mainwindow.h"
#include "c5systempreference.h"
#include "c5login.h"
#include "logwriter.h"
#include "c5servername.h"
#include "dlgserverconnection.h"
#include <QMessageBox>
#include <QApplication>
#include <QTranslator>
#include <QStyleFactory>
#include <QFontDatabase>
#include <QFile>
#include <QSslSocket>
#include <ctime>
#include <QSettings>

bool isDarkModeEnabled()
{
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                       QSettings::NativeFormat);
    return settings.value("AppsUseLightTheme", 1).toInt() == 0; // 0 — темная, 1 — светлая
}

int main(int argc, char *argv[])
{
#ifndef QT_DEBUG
    QStringList libPath = QCoreApplication::libraryPaths();
    libPath << qApp->applicationDirPath();
    libPath << qApp->applicationDirPath() + "/platforms";
    libPath << qApp->applicationDirPath() + "/sqldrivers";
    libPath << qApp->applicationDirPath() + "/printsupport";
    libPath << qApp->applicationDirPath() + "/imageformats";
    QCoreApplication::setLibraryPaths(libPath);
#endif
    QApplication a(argc, argv);
    qputenv("QT_ASSUME_UTF8", "1");
    QTranslator t;
    if (t.load(":/lang/FrontDesk.qm")) {
        a.installTranslator( &t);
    }
    LogWriter::write(LogWriterLevel::verbose, "Support SSL", QSslSocket::supportsSsl() ? "true" : "false");
    LogWriter::write(LogWriterLevel::verbose, "Support SSL version", QSslSocket::sslLibraryBuildVersionString());
    while (__c5config.getRegValue("ss_server_address").toString().isEmpty()) {
        DlgServerConnection::showSettings(nullptr);
    }
    C5ServerName c5sn(__c5config.getRegValue("ss_server_address").toString(), "office");
    if (!c5sn.getServers("office")) {
        C5Message::error(c5sn.mErrorString);
        return -1;
    }
#define LOGGING
    if (__c5config.getRegValue("ss_server_address").toString().isEmpty() && C5ServerName::mServers.isEmpty()) {
        C5Message::error("Servername parameter must be pass as argument");
        return 1;
    }
    QString css;
    if (!isDarkModeEnabled()) {
        QFile style(a.applicationDirPath() + "/officestyle.css");
        if (style.exists()) {
            if (style.open(QIODevice::ReadOnly)) {
                css = style.readAll();
                QString css2(css);
                css2.replace("%font-size%", "12");
                css2.replace("%font-family%", "Tahoma");
                a.setStyleSheet(css2);
            }
        }
    }
    C5Login l;
    if (l.exec() == QDialog::Accepted) {
        C5Config::fSettingsName = __c5config.getRegValue("ss_settings").toString();
        C5Config::initParamsFromDb();
    } else {
        return 0;
    }
    if (!isDarkModeEnabled()) {
        css.replace("%font-size%", __c5config.getValue(param_fd_font_size));
        css.replace("%font-family%", __c5config.getValue(param_app_font_family));
        a.setStyleSheet(css);
    }
    if (!C5SystemPreference::checkDecimalPointAndSeparator()) {
        return 0;
    }
    a.setStyle(QStyleFactory::create("fusion"));
    QFontDatabase::addApplicationFont(":/barcode.ttf");
    //    int id = QFontDatabase::addApplicationFont(":/ahuni.ttf");
    //    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont font(a.font());
    font.setPointSize(__c5config.fronDeskFontSize());
    font.setFamily(__c5config.getValue(param_app_font_family));
    a.setFont(font);
    srand(time(NULL));
    C5MainWindow w;
    w.showMaximized();
    a.processEvents();
    w.on_actionLogin_triggered();
    return a.exec();
}
