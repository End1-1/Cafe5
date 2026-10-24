#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QLockFile>
#include <QScreen>
#include <QSettings>
#include <QSslSocket>
#include <QTranslator>
#include "c5config.h"
#include "c5database.h"
#include "c5message.h"
#include "c5systempreference.h"
#include "dlgpin.h"
#include "c5servername.h"
#include "fileversion.h"
#include "logwriter.h"
#include "ndataprovider.h"
#include "printtaxno.h"
#include "workspace.h"
#include <QJsonArray>
#include <QJsonObject>

namespace {

QJsonObject serverByConfig(const QString &configName)
{
    const QJsonArray servers = C5ServerName::mServers;

    if(!configName.isEmpty()) {
        for(const QJsonValue &v : servers) {
            const QJsonObject row = v.toObject();

            if(row.value(QStringLiteral("name")).toString().compare(configName, Qt::CaseInsensitive) == 0
                    || row.value(QStringLiteral("settings")).toString().compare(configName, Qt::CaseInsensitive) == 0
                    || row.value(QStringLiteral("db")).toString().compare(configName, Qt::CaseInsensitive) == 0) {
                return row;
            }
        }

        return {};
    }

    if(servers.isEmpty()) {
        return {};
    }

    return servers.at(0).toObject();
}

}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
#ifndef QT_DEBUG
    QStringList libPath = QCoreApplication::libraryPaths();
    libPath << a.applicationDirPath();
    libPath << a.applicationDirPath() + "/platforms";
    libPath << a.applicationDirPath() + "/sqldrivers";
    libPath << a.applicationDirPath() + "/printsupport";
    libPath << a.applicationDirPath() + "/imageformats";
    QCoreApplication::setLibraryPaths(libPath);
#endif
    //    if (QDate::currentDate() > QDate::fromString("28/02/2023", "dd/MM/yyyy")) {
    //        return 1;
    //    }
    QDir d;
    QFile file(d.homePath() + "/" + _APPLICATION_ + "/" + _MODULE_ + "lock.pid");
    file.remove();
    QLockFile lockFile(d.homePath() + "/" + _APPLICATION_ + "/" + _MODULE_ + "lock.pid");

    if(!lockFile.tryLock()) {
        C5Message::error(QObject::tr("An instance of application already running"));
        return -1;
    }

    LogWriter::write(LogWriterLevel::verbose, "Support SSL", QSslSocket::supportsSsl() ? "true" : "false");
    LogWriter::write(LogWriterLevel::verbose, "Support SSL version", QSslSocket::sslLibraryBuildVersionString());
    QTranslator t;

    if(t.load(":/Smart.qm")) {
        a.installTranslator(&t);
    }

    QStringList args;

    for(int i = 0; i < argc; i++) {
        args << argv[i];
    }

    QFont f("Arial LatArm Unicode", 10);
    //QFont f(__c5config.getValue(param_app_font_family), 10);
    qApp->setFont(f);
    QFile styleFile(qApp->applicationDirPath() + "/smartstyle.css");

    if(styleFile.open(QIODevice::ReadOnly)) {
        a.setStyleSheet(styleFile.readAll());
    }

    for(const QString &s : std::as_const(args)) {
        if(s.startsWith("/monitor")) {
            int monitor = 0;
            QStringList mon = s.split("=");

            if(mon.length() == 2) {
                monitor = mon.at(1).toInt();
                C5Dialog::mScreen = monitor;
            }
        }
    }

    QString fileVersion =  FileVersion::getVersionString(a.applicationFilePath());
    int build = 0;
    const QStringList parts = fileVersion.split('.');

    if(parts.size() >= 4) {
        build = parts.at(3).toInt();
    }

    PrintTaxNO::mDebugRseq = build;
    NDataProvider::mAppName = "smart";
    NDataProvider::mFileVersion = fileVersion;
    QString serverName;
    QString configName;

    for(const QString &s : std::as_const(args)) {
        if(s.startsWith(QStringLiteral("/servername="), Qt::CaseInsensitive)) {
            serverName = s.mid(s.indexOf(QLatin1Char('=')) + 1).trimmed();
        } else if(s.startsWith(QStringLiteral("/config="), Qt::CaseInsensitive)) {
            configName = s.mid(s.indexOf(QLatin1Char('=')) + 1).trimmed();
        }
    }

    if(!serverName.isEmpty()) {
        C5ServerName serverList(serverName);

        if(!serverList.getServers()) {
            C5Message::error(serverList.mErrorString);
            return 1;
        }

        QJsonObject row = serverByConfig(configName);

        if(row.isEmpty() && !configName.isEmpty() && !C5ServerName::mServers.isEmpty()) {
            row = C5ServerName::mServers.at(0).toObject();
        }

        if(row.isEmpty()) {
            C5Message::error(QObject::tr("Connection name not found"));
            return 1;
        }

        QString wsUrl = serverName;

        if(!wsUrl.endsWith(QStringLiteral("/ws"))) {
            wsUrl += QStringLiteral("/ws");
        }

        const QString key = row.value(QStringLiteral("key")).toString();
        const QString db = row.value(QStringLiteral("db")).toString();
        const QString serverKey = (!key.isEmpty() && !db.isEmpty())
                                  ? key + QLatin1Char('.') + db
                                  : row.value(QStringLiteral("key")).toString();
        DlgPin::setConnection(row.value(QStringLiteral("settings")).toString(),
                              row.value(QStringLiteral("database")).toString(),
                              wsUrl,
                              serverKey);

        if(!configName.isEmpty()) {
            C5Config::fSettingsName = configName;
        }
    }

    auto *dlgPin = new DlgPin();

    if(dlgPin->exec() == QDialog::Rejected) {
        return 0;
    }

    if(!C5SystemPreference::checkDecimalPointAndSeparator()) {
        return 0;
    }

    if(C5Config::getValue(param_debuge_mode).toInt() > 0) {
        C5Database::LOGGING = true;
        NDataProvider::mDebug = true;
    }

    C5Dialog::setMainWindow(nullptr);
    auto *w = new Workspace(dlgPin->mUser);
    w->setWindowModality(Qt::NonModal);
    w->showMaximized();
    return a.exec();
}
