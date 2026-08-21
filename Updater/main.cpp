#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QProcessEnvironment>
#include <QProgressBar>
#include <QStandardPaths>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#ifdef Q_OS_WIN
#  include <windows.h>
#endif

#include "updatemanger.h"

namespace {

QString setupPackageName(const QString &appName)
{
    if (appName.compare(QStringLiteral("officen"), Qt::CaseInsensitive) == 0
        || appName.compare(QStringLiteral("frontdesk"), Qt::CaseInsensitive) == 0) {
        return QStringLiteral("frontdesk");
    }
    return appName.toLower();
}

QString setupDownloadUrl(const QString &appName, const QString &version)
{
    return QStringLiteral("https://picasso.am/files/%1_setup_%2.exe")
        .arg(setupPackageName(appName), version);
}

QString setupFileName(const QString &appName, const QString &version)
{
    return QStringLiteral("%1_setup_%2.exe").arg(setupPackageName(appName), version);
}

bool copyFileOverwrite(const QString &src, const QString &dst)
{
    if (!QFile::exists(src)) {
        return false;
    }
    QFile::remove(dst);
    return QFile::copy(src, dst);
}

/**
 * Copy Qt runtime out of {app} into TEMP so Inno can replace {app}\Qt6*.dll
 * while PicassoUpdateHost.exe is still running.
 */
QString prepareQtRuntimeAwayFromApp(const QString &appDir)
{
    if (appDir.isEmpty()) {
        return QString();
    }

    const QString tempRoot = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    const QString rtDir = QDir(tempRoot).filePath(QStringLiteral("PicassoUpdateRt"));
    const QString platformsDir = QDir(rtDir).filePath(QStringLiteral("platforms"));
    QDir().mkpath(platformsDir);

    const QStringList rootDlls = {
        QStringLiteral("Qt6Core.dll"),
        QStringLiteral("Qt6Gui.dll"),
        QStringLiteral("Qt6Widgets.dll"),
        QStringLiteral("Qt6Network.dll"),
    };
    for (const QString &name : rootDlls) {
        copyFileOverwrite(QDir(appDir).filePath(name), QDir(rtDir).filePath(name));
    }
    copyFileOverwrite(QDir(appDir).filePath(QStringLiteral("platforms/qwindows.dll")),
                      QDir(platformsDir).filePath(QStringLiteral("qwindows.dll")));
    copyFileOverwrite(QDir(appDir).filePath(QStringLiteral("platforms/qminimal.dll")),
                      QDir(platformsDir).filePath(QStringLiteral("qminimal.dll")));

#ifdef Q_OS_WIN
    SetDllDirectoryW(reinterpret_cast<LPCWSTR>(rtDir.utf16()));
#endif
    qputenv("PATH", QFile::encodeName(rtDir) + ';' + qgetenv("PATH"));
    qputenv("QT_PLUGIN_PATH", QFile::encodeName(rtDir));
    qputenv("QT_QPA_PLATFORM_PLUGIN_PATH", QFile::encodeName(platformsDir));
    return rtDir;
}

/**
 * Self-update strategy:
 * 1. App starts {app}\Updater.exe
 * 2. Updater copies itself to %TEMP%\PicassoUpdateHost.exe and relaunches from there
 * 3. Host uses Qt DLLs copied to %TEMP%\PicassoUpdateRt (NOT {app}) so setup can overwrite {app}
 * 4. Host downloads setup via WinHTTP and runs Inno /SILENT
 */
bool ensureRunningFromTemp(int argc, char *argv[])
{
    const QString self = QFileInfo(QString::fromLocal8Bit(argv[0])).absoluteFilePath();
    const QString appDir = QFileInfo(self).absolutePath();
    const QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    const QString tempExe = QDir(tempDir).filePath(QStringLiteral("PicassoUpdateHost.exe"));

    const QString selfNative = QDir::toNativeSeparators(self);
    const QString tempNative = QDir::toNativeSeparators(tempExe);
    if (selfNative.compare(tempNative, Qt::CaseInsensitive) == 0) {
        const QByteArray envDir = qgetenv("PICASSO_APP_DIR");
        const QString sourceAppDir = envDir.isEmpty() ? appDir : QString::fromLocal8Bit(envDir);
        prepareQtRuntimeAwayFromApp(sourceAppDir);
        return true;
    }

    QFile::remove(tempExe);
    if (!QFile::copy(self, tempExe)) {
        prepareQtRuntimeAwayFromApp(appDir);
        return true;
    }

    QStringList args;
    for (int i = 1; i < argc; ++i) {
        args << QString::fromLocal8Bit(argv[i]);
    }

    QProcess proc;
    proc.setProgram(tempExe);
    proc.setArguments(args);
    proc.setWorkingDirectory(tempDir);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert(QStringLiteral("PICASSO_APP_DIR"), appDir);
    proc.setProcessEnvironment(env);

    if (!proc.startDetached()) {
        prepareQtRuntimeAwayFromApp(appDir);
        return true;
    }
    return false;
}

} // namespace

int main(int argc, char *argv[])
{
    if (!ensureRunningFromTemp(argc, argv)) {
        return 0;
    }

    QApplication a(argc, argv);
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Picasso Updater"));
    parser.addHelpOption();
    QCommandLineOption appOpt(QStringLiteral("app"), QStringLiteral("Application module name"), QStringLiteral("module"));
    QCommandLineOption verOpt(QStringLiteral("version"), QStringLiteral("Version to update to"), QStringLiteral("version"));
    parser.addOption(appOpt);
    parser.addOption(verOpt);
    parser.process(a);

    const QString module = parser.value(appOpt).trimmed();
    const QString version = parser.value(verOpt).trimmed();
    if (module.isEmpty() || version.isEmpty()) {
        QMessageBox::critical(nullptr, QStringLiteral("Updater"),
                              QStringLiteral("Missing required arguments.\nUsage: Updater --app=<officen|shop|waiter|cookingprogress> --version=<X.Y.Z>"));
        return 1;
    }

    QWidget w;
    w.setWindowTitle(QStringLiteral("Picasso Update"));
    w.setWindowFlags(w.windowFlags() | Qt::WindowStaysOnTopHint);
    auto *layout = new QVBoxLayout(&w);
    auto *label = new QLabel(QStringLiteral("Preparing update…"), &w);
    label->setWordWrap(true);
    label->setMinimumHeight(72);
    auto *pb = new QProgressBar(&w);
    pb->setRange(0, 100);
    pb->setTextVisible(true);
    layout->addWidget(label);
    layout->addWidget(pb);
    w.resize(480, 150);
    w.show();
    w.raise();
    w.activateWindow();

    UpdateManager um(setupDownloadUrl(module, version), setupFileName(module, version));
    um.setProgressBar(pb);
    um.setHostWindow(&w);
    QObject::connect(&um, &UpdateManager::statusChanged, label, &QLabel::setText);
    QObject::connect(&um, &UpdateManager::aboutToElevate, &w, [&w]() {
        w.setWindowFlag(Qt::WindowStaysOnTopHint, false);
        w.showNormal();
        w.lower();
    });
    QObject::connect(&um, &UpdateManager::error, [&](const QString &msg) {
        w.setWindowFlag(Qt::WindowStaysOnTopHint, false);
        w.show();
        QMessageBox::critical(&w, QStringLiteral("Update error"), msg);
        a.quit();
    });
    QObject::connect(&um, &UpdateManager::finished, [&]() {
        QTimer::singleShot(2500, &a, &QApplication::quit);
    });
    um.start();
    return a.exec();
}
