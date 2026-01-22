#include "c5mainwindow.h"
#include "c5systempreference.h"
#include "c5login.h"
#include "logwriter.h"
#include "c5servername.h"
#include "c5config.h"
#include "c5message.h"
#include "c5officewidget.h"
#include "c5connectiondialog.h"
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

int main(int argc, char* argv[])
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

    if(t.load(":/lang/FrontDesk.qm")) {
        a.installTranslator(&t);
    }

    LogWriter::write(LogWriterLevel::verbose, "Support SSL", QSslSocket::supportsSsl() ? "true" : "false");
    LogWriter::write(LogWriterLevel::verbose, "Support SSL version", QSslSocket::sslLibraryBuildVersionString());
    auto c5sn  = new C5ServerName(QString("%1://%2").arg(C5ConnectionDialog::instance()->connectionType() == C5ConnectionDialog::noneSecure ? "ws" : "wss",
                                  C5ConnectionDialog::instance()->serverAddress() + "/ws"));

    for(const QString &s : a.arguments()) {
        if(s.startsWith("/monitor")) {
            QList<QScreen*> screens = a.screens();
            int monitor = 0;
            QStringList mon = s.split("=");

            if(mon.length() == 2) {
                monitor = mon.at(1).toInt();
                C5Dialog::mScreen = monitor;
                C5MainWindow::mScreen = monitor;
            }
        }
    }

    if(!c5sn->getServers()) {
        C5ConnectionDialog::showSettings(nullptr);
        C5Message::error(c5sn->mErrorString);
        return -1;
    }

    if(!c5sn->getConnection(__c5config.getRegValue("ss_database").toString())) {
        C5Message::error(c5sn->mErrorString);
        return -1;
    }

    c5sn->deleteLater();
#define LOGGING

    if(__c5config.getRegValue("ss_server_address").toString().isEmpty() && C5ServerName::mServers.isEmpty()) {
        C5Message::error("Servername parameter must be pass as argument");
        return 1;
    }

    QString css;

    if(!isDarkModeEnabled()) {
        QFile style(a.applicationDirPath() + "/officestyle.css");

        if(style.exists()) {
            if(style.open(QIODevice::ReadOnly)) {
                css = style.readAll();
                QString css2(css);
                css2.replace("%font-size%", "12");
                css2.replace("%font-family%", "Tahoma");
                a.setStyleSheet(css2);
            }
        }
    }

    C5Login l(nullptr);

    if(l.exec() == QDialog::Accepted) {
        C5Config::fSettingsName = __c5config.getRegValue("ss_settings").toString();
        C5Config::initParamsFromDb();
    } else {
        return 0;
    }

    if(!isDarkModeEnabled()) {
        css.replace("%font-size%", __c5config.getValue(param_fd_font_size));
        css.replace("%font-family%", __c5config.getValue(param_app_font_family));
        a.setStyleSheet(css);
    }

    if(!C5SystemPreference::checkDecimalPointAndSeparator()) {
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
    w.mUser = C5OfficeWidget::mUser;
    w.showMaximized();
    a.processEvents();
    w.postLoginSetup();
    return a.exec();
}
