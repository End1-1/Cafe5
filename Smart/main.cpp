#include "workspace.h"
#include "c5config.h"
#include "c5systempreference.h"
#include "ndataprovider.h"
#include "dlgpin.h"
#include "ndataprovider.h"
#include "fileversion.h"
#include "logwriter.h"
#include "c5message.h"
#include "printtaxn.h"
#include "c5database.h"
#include <QFile>
#include <QLockFile>
#include <QSslSocket>
#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QScreen>
#include <QDesktopServices>
#include <QDir>

int main(int argc, char* argv[])
{
    qputenv("QT_OPENGL", "angle");
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

    PrintTaxN::mDebugRseq = build;
    NDataProvider::mAppName = "smart";
    NDataProvider::mFileVersion = fileVersion;
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
