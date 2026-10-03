#ifndef C5UPDATERLAUNCHER_H
#define C5UPDATERLAUNCHER_H

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QProcessEnvironment>
#include <QSettings>
#include <QString>
#include <QThread>

#ifdef Q_OS_WIN
#  include <windows.h>
#endif

/**
 * Starts shared Picasso\\updater\\Updater.exe (or sibling next to the module).
 * Same behaviour as Waiter / Shop / FrontDesk update flow.
 */
namespace C5UpdaterLauncher {

inline bool tryStart(const QString &appName, const QString &version)
{
    if (appName.isEmpty() || version.isEmpty()) {
        return false;
    }

    const QString moduleDir = QDir::fromNativeSeparators(QCoreApplication::applicationDirPath());
    QString updaterPath;

    const QSettings updaterReg(QStringLiteral("HKEY_LOCAL_MACHINE\\Software\\Picasso\\updater"),
                               QSettings::NativeFormat);
    const QString sharedDir = updaterReg.value(QStringLiteral("InstallPath")).toString().trimmed();
    if (!sharedDir.isEmpty()) {
        const QString candidate = QDir(sharedDir).filePath(QStringLiteral("Updater.exe"));
        if (QFile::exists(candidate)) {
            updaterPath = QDir::toNativeSeparators(QFileInfo(candidate).absoluteFilePath());
        }
    }
    if (updaterPath.isEmpty()) {
        const QString sibling = QDir(moduleDir).filePath(QStringLiteral("../updater/Updater.exe"));
        if (QFile::exists(sibling)) {
            updaterPath = QDir::toNativeSeparators(QFileInfo(sibling).absoluteFilePath());
        }
    }
    if (updaterPath.isEmpty()) {
        const QString inModule = QDir(moduleDir).filePath(QStringLiteral("Updater.exe"));
        if (QFile::exists(inModule)) {
            updaterPath = QDir::toNativeSeparators(QFileInfo(inModule).absoluteFilePath());
        }
    }
    if (updaterPath.isEmpty()) {
        const QString legacy = QDir(moduleDir).filePath(QStringLiteral("updater.exe"));
        if (QFile::exists(legacy)) {
            updaterPath = QDir::toNativeSeparators(QFileInfo(legacy).absoluteFilePath());
        }
    }
    if (updaterPath.isEmpty()) {
        const QString pf = qEnvironmentVariable("ProgramFiles");
        if (!pf.isEmpty()) {
            const QString fallback = QDir(pf).filePath(QStringLiteral("Picasso/updater/Updater.exe"));
            if (QFile::exists(fallback)) {
                updaterPath = QDir::toNativeSeparators(QFileInfo(fallback).absoluteFilePath());
            }
        }
    }
    if (updaterPath.isEmpty()) {
        qWarning("C5UpdaterLauncher: Updater.exe not found (moduleDir=%s)", qPrintable(moduleDir));
        return false;
    }

    const QStringList args{
        QStringLiteral("--app=%1").arg(appName),
        QStringLiteral("--version=%1").arg(version),
        QStringLiteral("--module-dir=%1").arg(QDir::toNativeSeparators(moduleDir)),
    };

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const QString updaterDir = QFileInfo(updaterPath).absolutePath();
    const QString pathPrefix = QDir::toNativeSeparators(moduleDir)
        + QLatin1Char(';')
        + QDir::toNativeSeparators(updaterDir);
    env.insert(QStringLiteral("PATH"), pathPrefix + QLatin1Char(';') + env.value(QStringLiteral("PATH")));
    env.insert(QStringLiteral("PICASSO_MODULE_DIR"), QDir::toNativeSeparators(moduleDir));
    env.insert(QStringLiteral("QT_PLUGIN_PATH"), QDir::toNativeSeparators(moduleDir));
    env.insert(QStringLiteral("QT_QPA_PLATFORM_PLUGIN_PATH"),
               QDir::toNativeSeparators(QDir(moduleDir).filePath(QStringLiteral("platforms"))));

    QProcess proc;
    proc.setProgram(updaterPath);
    proc.setArguments(args);
    proc.setWorkingDirectory(moduleDir);
    proc.setProcessEnvironment(env);

    qint64 pid = 0;
    if (proc.startDetached(&pid)) {
        qDebug("C5UpdaterLauncher: started pid=%lld path=%s cwd=%s",
               static_cast<long long>(pid),
               qPrintable(updaterPath),
               qPrintable(moduleDir));
        QThread::msleep(400);
        return true;
    }

#ifdef Q_OS_WIN
    QString cmd = QStringLiteral("\"%1\"").arg(updaterPath);
    for (const QString &a : args) {
        cmd += QLatin1Char(' ');
        cmd += a.contains(QLatin1Char(' ')) ? QStringLiteral("\"%1\"").arg(a) : a;
    }

    QString envJoined;
    const QStringList keys = env.keys();
    for (const QString &k : keys) {
        envJoined += k;
        envJoined += QLatin1Char('=');
        envJoined += env.value(k);
        envJoined += QLatin1Char('\0');
    }
    envJoined += QLatin1Char('\0');
    std::wstring envBlock = envJoined.toStdWString();
    std::wstring cmdLine = cmd.toStdWString();
    std::wstring workDir = QDir::toNativeSeparators(moduleDir).toStdWString();

    auto tryCreate = [&](DWORD flags) -> bool {
        STARTUPINFOW si = {sizeof(si)};
        PROCESS_INFORMATION pi{};
        std::wstring cmdCopy = cmdLine;
        const BOOL ok = CreateProcessW(
            nullptr,
            cmdCopy.data(),
            nullptr,
            nullptr,
            FALSE,
            flags | CREATE_UNICODE_ENVIRONMENT,
            envBlock.data(),
            workDir.c_str(),
            &si,
            &pi);
        if (!ok) {
            qWarning("C5UpdaterLauncher: CreateProcess flags=0x%lx failed err=%lu",
                     static_cast<unsigned long>(flags),
                     static_cast<unsigned long>(GetLastError()));
            return false;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        QThread::msleep(400);
        return true;
    };

    if (tryCreate(CREATE_BREAKAWAY_FROM_JOB | DETACHED_PROCESS)
        || tryCreate(DETACHED_PROCESS)
        || tryCreate(CREATE_NEW_PROCESS_GROUP)
        || tryCreate(0)) {
        return true;
    }
#endif

    qWarning("C5UpdaterLauncher: failed to start %s", qPrintable(updaterPath));
    return false;
}

} // namespace C5UpdaterLauncher

#endif // C5UPDATERLAUNCHER_H
