#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QLockFile>
#include <QSettings>
#include <QStyleFactory>
#include <QTimer>
#include "c5connectiondialog.h"
#include "c5dialog.h"
#include "c5message.h"
#include "c5registrysettings.h"
#include "c5systempreference.h"
#include "c5uilanguage.h"
#include "c5user.h"
#include "cookingprogresswindow.h"
#include "dict_workstation.h"
#include "dlgsplashscreen.h"
#include "fileversion.h"
#include "logwriter.h"
#include "ndataprovider.h"
#include "ninterface.h"
#include "version.h"

namespace {

QString normalizeHost(QString host)
{
    host = host.trimmed();
    if(host.startsWith(QStringLiteral("http://"), Qt::CaseInsensitive)) {
        host = host.mid(7);
    } else if(host.startsWith(QStringLiteral("https://"), Qt::CaseInsensitive)) {
        host = host.mid(8);
    }
    while(host.endsWith(QLatin1Char('/'))) {
        host.chop(1);
    }
    return host;
}

void applyNetworkFromDialog()
{
    auto *dlg = C5ConnectionDialog::instance();
    dlg->reloadFromRegistry();
    NDataProvider::mProtocol = dlg->connectionType() == C5ConnectionDialog::noneSecure
                                   ? QStringLiteral("http")
                                   : QStringLiteral("https");
    NDataProvider::mHost = normalizeHost(dlg->serverAddress());
}

/** If CookingProgress profile has no server account, copy from Waiter/Shop profiles. */
void importConnectionIfEmpty()
{
    QSettings mine(QString::fromUtf8(_ORGANIZATION_), C5RegistrySettings::registryPath());
    if(!mine.value(QStringLiteral("ss_server_username")).toString().trimmed().isEmpty()
       && !mine.value(QStringLiteral("ss_server_password")).toString().isEmpty()
       && !mine.value(QStringLiteral("ss_server_address")).toString().trimmed().isEmpty()) {
        return;
    }

    const QList<QPair<QString, QString>> sources = {
        {QStringLiteral("Jazzve"), QStringLiteral("Cafe5\\waiter")},
        {QStringLiteral("BreezeDevs"), QStringLiteral("Shop\\Shop")},
        {QStringLiteral("Jazzve"), QStringLiteral("Cafe5\\Cafe5")},
    };

    for(const auto &src : sources) {
        QSettings other(src.first, src.second);
        const QString host = other.value(QStringLiteral("ss_server_address")).toString().trimmed();
        const QString user = other.value(QStringLiteral("ss_server_username")).toString().trimmed();
        const QString pass = other.value(QStringLiteral("ss_server_password")).toString();
        if(host.isEmpty() || user.isEmpty() || pass.isEmpty()) {
            continue;
        }
        mine.setValue(QStringLiteral("ss_server_address"), host);
        mine.setValue(QStringLiteral("ss_server_username"), user);
        mine.setValue(QStringLiteral("ss_server_password"), pass);
        mine.setValue(QStringLiteral("ss_server_key"), other.value(QStringLiteral("ss_server_key")));
        mine.setValue(QStringLiteral("ss_secure_connection"), other.value(QStringLiteral("ss_secure_connection")));
        mine.sync();
        break;
    }
}

void exitApp()
{
    // Hidden C5ConnectionDialog singleton otherwise keeps the process alive with no UI.
    QTimer::singleShot(0, qApp, &QCoreApplication::quit);
}

void startLogin(DlgSplashScreen *splash);

bool connectionReady()
{
    applyNetworkFromDialog();
    auto *dlg = C5ConnectionDialog::instance();
    const QString user = dlg->username().trimmed();
    const QString pass = dlg->password();
    return !NDataProvider::mHost.trimmed().isEmpty() && !user.isEmpty() && !pass.isEmpty();
}

void beginLoginWithSplash()
{
    auto *splash = new DlgSplashScreen(WORKSTATION_SHOP, nullptr);
    splash->setAttribute(Qt::WA_DeleteOnClose);
    splash->show();
    splash->raise();
    splash->activateWindow();
    QTimer::singleShot(0, qApp, [splash]() { startLogin(splash); });
}

void onLoginFinished(DlgSplashScreen *splash, C5User *user)
{
    if(!user) {
        const int answer = C5Message::question(
            QObject::tr("Program account could not log in. Would you like to open settings?"),
            QObject::tr("Yes"),
            QObject::tr("No"));
        if(answer == QDialog::Accepted) {
            C5ConnectionDialog::showSettings(nullptr);
            if(splash) {
                splash->close();
            }
            beginLoginWithSplash();
            return;
        }
        if(splash) {
            splash->close();
        }
        exitApp();
        return;
    }

    if(splash) {
        splash->close();
    }
    C5Dialog::setMainWindow(nullptr);
    auto *w = new CookingProgressWindow(user);
    w->setAttribute(Qt::WA_DeleteOnClose);
#ifndef QT_DEBUG
    w->showFullScreen();
#else
    w->showMaximized();
#endif
    w->raise();
    w->activateWindow();
}

void startLogin(DlgSplashScreen *splash)
{
    if(!connectionReady()) {
        C5Message::error(QObject::tr("Fill server address, username and password"));
        C5ConnectionDialog::showSettings(nullptr);
        if(!connectionReady()) {
            if(splash) {
                splash->close();
            }
            exitApp();
            return;
        }
    }

    applyNetworkFromDialog();
    auto *dlg = C5ConnectionDialog::instance();
    const QString user = dlg->username().trimmed();
    const QString pass = dlg->password();

    if(!splash) {
        exitApp();
        return;
    }

    splash->login(user, pass, [splash](C5User *user) {
        QTimer::singleShot(0, qApp, [splash, user]() { onLoginFinished(splash, user); });
    });
}

} // namespace

int main(int argc, char *argv[])
{
#ifndef QT_DEBUG
    QStringList libPath;
    libPath << QCoreApplication::applicationDirPath();
    libPath << QCoreApplication::applicationDirPath() + "/platforms";
    libPath << QCoreApplication::applicationDirPath() + "/sqldrivers";
    libPath << QCoreApplication::applicationDirPath() + "/plugins";
    QCoreApplication::setLibraryPaths(libPath);
#endif

    QApplication a(argc, argv);
    qputenv("QT_ASSUME_UTF8", "1");
    a.setStyle(QStyleFactory::create(QStringLiteral("fusion")));
    a.setQuitOnLastWindowClosed(true);

    C5UiLanguage::configure(QStringLiteral(":/lang/CookingProgress.qm"),
                            QStringLiteral(":/lang/CookingProgress_ru.qm"));
    C5UiLanguage::loadSaved();

    QString fileVersion = FileVersion::getVersionString(a.applicationFilePath());

    for(const QString &s : a.arguments()) {
        if(s.startsWith(QStringLiteral("/settingspath"), Qt::CaseInsensitive)) {
            const int eq = s.indexOf(QLatin1Char('='));
            if(eq > 0) {
                const QString subPath = s.mid(eq + 1).trimmed();
                C5RegistrySettings::settingsSubPath = subPath;
                C5ConnectionDialog::mSettingsPath = subPath;
            }
        }
        if(s.startsWith(QStringLiteral("/setversion"))) {
            const QStringList ver = s.split(QLatin1Char('='));
            if(ver.length() == 2) {
                fileVersion = ver.at(1);
            }
        }
    }

    if(fileVersion.isEmpty()) {
        fileVersion = QStringLiteral("%1.%2.%3.%4")
                          .arg(VER_MAJOR)
                          .arg(VER_MINOR)
                          .arg(VER_PATCH)
                          .arg(VER_BUILD);
    }

    QDir d;
    const QString appHome = d.homePath() + "/" + _APPLICATION_;
    d.mkpath(appHome);
    d.mkpath(appHome + "/logs");
    QLockFile lockFile(appHome + "/" + _MODULE_ + ".lock.pid");
    lockFile.setStaleLockTime(0);
    lockFile.removeStaleLockFile();
    if(!lockFile.tryLock()) {
        C5Message::error(QObject::tr("An instance of application already running"));
        return -1;
    }

    if(!C5SystemPreference::checkDecimalPointAndSeparator()) {
        return 0;
    }

    importConnectionIfEmpty();

    NDataProvider::mAppName = QStringLiteral("cookingprogress");
    NDataProvider::mFileVersion = fileVersion;
    NDataProvider::mDebug = a.arguments().contains(QStringLiteral("/debug"));
    applyNetworkFromDialog();

    // Kitchen display: no modal loading dialogs on poll/login.
    NInterface::pushSuppressProgress();

    beginLoginWithSplash();

    return a.exec();
}
