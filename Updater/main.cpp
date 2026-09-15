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
#include <QSettings>
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

QString defaultPicassoRoot()
{
#ifdef Q_OS_WIN
    const QString programFiles = qEnvironmentVariable("ProgramFiles");
    if (!programFiles.isEmpty()) {
        return QDir(programFiles).filePath(QStringLiteral("Picasso"));
    }
#endif
    return QStringLiteral("C:/Program Files/Picasso");
}

QString registryString(const QString &subkey, const QString &valueName)
{
    const QString path = subkey.isEmpty()
                             ? QStringLiteral("HKEY_LOCAL_MACHINE\\Software\\Picasso")
                             : QStringLiteral("HKEY_LOCAL_MACHINE\\Software\\Picasso\\") + subkey;
    QSettings settings(path, QSettings::NativeFormat);
    return settings.value(valueName).toString().trimmed();
}

QString markerExeForModule(const QString &packageName)
{
    if (packageName == QStringLiteral("frontdesk")) {
        return QStringLiteral("OfficeN.exe");
    }
    if (packageName == QStringLiteral("shop")) {
        return QStringLiteral("Shop_net.exe");
    }
    if (packageName == QStringLiteral("waiter")) {
        return QStringLiteral("Waiter.exe");
    }
    if (packageName == QStringLiteral("cookingprogress")) {
        return QStringLiteral("CookingProgress.exe");
    }
    return QString();
}

QString resolveModuleInstallDir(const QString &appModule, const QString &moduleDirOverride)
{
    if (!moduleDirOverride.isEmpty()) {
        return QDir::fromNativeSeparators(moduleDirOverride);
    }

    const QString packageName = setupPackageName(appModule);
    const QString marker = markerExeForModule(packageName);
    const QString subDirPath = QDir(defaultPicassoRoot()).filePath(packageName);

    const QString regPath = registryString(packageName, QStringLiteral("InstallPath"));
    if (!regPath.isEmpty() && QDir(regPath).exists()) {
        if (marker.isEmpty() || !QFile::exists(QDir(regPath).filePath(marker))) {
            return QDir::fromNativeSeparators(regPath);
        }
        if (QFileInfo(regPath).fileName().compare(packageName, Qt::CaseInsensitive) == 0) {
            return QDir::fromNativeSeparators(regPath);
        }
        return subDirPath;
    }

    QString root = registryString(QString(), QStringLiteral("InstallPath"));
    if (root.isEmpty()) {
        root = defaultPicassoRoot();
    }
    if (!marker.isEmpty() && QFile::exists(QDir(root).filePath(marker))) {
        return subDirPath;
    }
    return subDirPath;
}

QString moduleDirFromArgs(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i) {
        const QString arg = QString::fromLocal8Bit(argv[i]);
        if (arg.startsWith(QStringLiteral("--module-dir="), Qt::CaseInsensitive)) {
            return QDir::fromNativeSeparators(arg.mid(13));
        }
    }
    return QString();
}

bool copyFileOverwrite(const QString &src, const QString &dst)
{
    if (!QFile::exists(src)) {
        return false;
    }
    QFile::remove(dst);
    return QFile::copy(src, dst);
}

QString firstDirWithQtWidgets(const QStringList &dirs)
{
    for (const QString &dir : dirs) {
        if (dir.isEmpty()) {
            continue;
        }
        if (QFile::exists(QDir(dir).filePath(QStringLiteral("Qt6Widgets.dll")))) {
            return dir;
        }
    }
    return QString();
}

/**
 * Copy Qt runtime next to PicassoUpdateHost.exe.
 * The Windows loader resolves Qt6Widgets.dll at process start — before main() —
 * so DLLs must already sit beside the host (PATH / SetDllDirectory are too late).
 */
QString copyQtRuntimeBesideHost(const QString &srcDir, const QString &dstDir)
{
    if (srcDir.isEmpty() || dstDir.isEmpty()) {
        return QString();
    }
    const QString platformsDir = QDir(dstDir).filePath(QStringLiteral("platforms"));
    QDir().mkpath(platformsDir);

    const QStringList rootDlls = {
        QStringLiteral("Qt6Core.dll"),
        QStringLiteral("Qt6Gui.dll"),
        QStringLiteral("Qt6Widgets.dll"),
        QStringLiteral("Qt6Network.dll"),
        QStringLiteral("Qt6Svg.dll"),
        QStringLiteral("vcruntime140.dll"),
        QStringLiteral("vcruntime140_1.dll"),
        QStringLiteral("msvcp140.dll"),
        QStringLiteral("msvcp140_1.dll"),
        QStringLiteral("msvcp140_2.dll"),
    };
    for (const QString &name : rootDlls) {
        copyFileOverwrite(QDir(srcDir).filePath(name), QDir(dstDir).filePath(name));
    }
    copyFileOverwrite(QDir(srcDir).filePath(QStringLiteral("platforms/qwindows.dll")),
                      QDir(platformsDir).filePath(QStringLiteral("qwindows.dll")));
    copyFileOverwrite(QDir(srcDir).filePath(QStringLiteral("platforms/qminimal.dll")),
                      QDir(platformsDir).filePath(QStringLiteral("qminimal.dll")));
    return dstDir;
}

void applyQtRuntimeEnv(const QString &rtDir)
{
    if (rtDir.isEmpty()) {
        return;
    }
    const QString platformsDir = QDir(rtDir).filePath(QStringLiteral("platforms"));
#ifdef Q_OS_WIN
    SetDllDirectoryW(reinterpret_cast<LPCWSTR>(rtDir.utf16()));
#endif
    qputenv("PATH", QFile::encodeName(rtDir) + ';' + qgetenv("PATH"));
    qputenv("QT_PLUGIN_PATH", QFile::encodeName(rtDir));
    qputenv("QT_QPA_PLATFORM_PLUGIN_PATH", QFile::encodeName(platformsDir));
}

/**
 * Self-update strategy:
 * 1. App starts {app}\Updater.exe (Qt already loaded from updater\ or PATH)
 * 2. Updater copies itself + Qt DLLs to %TEMP% and relaunches PicassoUpdateHost.exe
 * 3. Host loads Qt from %TEMP% so Inno can overwrite {app}\Qt6*.dll
 * 4. Host downloads setup via WinHTTP and runs Inno /SILENT
 */
bool ensureRunningFromTemp(int argc, char *argv[])
{
    const QString self = QFileInfo(QString::fromLocal8Bit(argv[0])).absoluteFilePath();
    const QString appDir = QFileInfo(self).absolutePath();
    const QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    const QString tempExe = QDir(tempDir).filePath(QStringLiteral("PicassoUpdateHost.exe"));
    const QString moduleDirArg = moduleDirFromArgs(argc, argv);
    const QString moduleDirEnv = QString::fromLocal8Bit(qgetenv("PICASSO_MODULE_DIR"));
    const QString moduleDir = !moduleDirArg.isEmpty()
                                  ? moduleDirArg
                                  : (!moduleDirEnv.isEmpty()
                                         ? QDir::fromNativeSeparators(moduleDirEnv)
                                         : QString());
    const QString qtSrc = firstDirWithQtWidgets({appDir, moduleDir});

    const QString selfNative = QDir::toNativeSeparators(self);
    const QString tempNative = QDir::toNativeSeparators(tempExe);
    if (selfNative.compare(tempNative, Qt::CaseInsensitive) == 0) {
        applyQtRuntimeEnv(tempDir);
        return true;
    }

    QFile::remove(tempExe);
    if (!QFile::copy(self, tempExe)) {
        applyQtRuntimeEnv(qtSrc.isEmpty() ? appDir : qtSrc);
        return true;
    }

    copyQtRuntimeBesideHost(qtSrc.isEmpty() ? appDir : qtSrc, tempDir);

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
    if (!moduleDir.isEmpty()) {
        env.insert(QStringLiteral("PICASSO_MODULE_DIR"), QDir::toNativeSeparators(moduleDir));
    }
    const QString pathPrefix = QDir::toNativeSeparators(tempDir)
        + QLatin1Char(';')
        + QDir::toNativeSeparators(qtSrc.isEmpty() ? appDir : qtSrc);
    env.insert(QStringLiteral("PATH"), pathPrefix + QLatin1Char(';') + env.value(QStringLiteral("PATH")));
    env.insert(QStringLiteral("QT_PLUGIN_PATH"), QDir::toNativeSeparators(tempDir));
    env.insert(QStringLiteral("QT_QPA_PLATFORM_PLUGIN_PATH"),
               QDir::toNativeSeparators(QDir(tempDir).filePath(QStringLiteral("platforms"))));
    proc.setProcessEnvironment(env);

    if (!proc.startDetached()) {
        applyQtRuntimeEnv(qtSrc.isEmpty() ? appDir : qtSrc);
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
    QCommandLineOption moduleDirOpt(QStringLiteral("module-dir"),
                                    QStringLiteral("Module install directory"),
                                    QStringLiteral("path"));
    parser.addOption(appOpt);
    parser.addOption(verOpt);
    parser.addOption(moduleDirOpt);
    parser.process(a);

    const QString module = parser.value(appOpt).trimmed();
    const QString version = parser.value(verOpt).trimmed();
    const QString moduleInstallDir =
        resolveModuleInstallDir(module, parser.value(moduleDirOpt).trimmed());
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

    UpdateManager um(setupDownloadUrl(module, version),
                     setupFileName(module, version),
                     module,
                     moduleInstallDir);
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
