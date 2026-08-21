#include "updatemanger.h"

#include <functional>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QThread>
#include <QTimer>
#include <QUrl>

#ifdef Q_OS_WIN
#  include <windows.h>
#  include <winhttp.h>
#  include <shellapi.h>
#endif

namespace {

#ifdef Q_OS_WIN
QString winHttpErrorMessage(DWORD err)
{
    return QStringLiteral("WinHTTP error %1").arg(err);
}

bool downloadWithWinHttp(const QString &urlStr,
                         const QString &destPath,
                         const std::function<void(qint64, qint64)> &onProgress,
                         QString &errorOut)
{
    const QUrl url(urlStr);
    if (!url.isValid() || url.host().isEmpty()) {
        errorOut = QStringLiteral("Invalid URL");
        return false;
    }

    const bool https = url.scheme().compare(QLatin1String("https"), Qt::CaseInsensitive) == 0;
    const INTERNET_PORT port = url.port(https ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT);
    const QString host = url.host();
    QString path = url.path();
    if (path.isEmpty()) {
        path = QStringLiteral("/");
    }
    if (url.hasQuery()) {
        path += QLatin1Char('?') + url.query();
    }

    HINTERNET hSession = WinHttpOpen(L"PicassoUpdater/1.0",
                                     WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                     WINHTTP_NO_PROXY_NAME,
                                     WINHTTP_NO_PROXY_BYPASS,
                                     0);
    if (!hSession) {
        errorOut = winHttpErrorMessage(GetLastError());
        return false;
    }

    HINTERNET hConnect = WinHttpConnect(hSession,
                                        reinterpret_cast<LPCWSTR>(host.utf16()),
                                        port,
                                        0);
    if (!hConnect) {
        errorOut = winHttpErrorMessage(GetLastError());
        WinHttpCloseHandle(hSession);
        return false;
    }

    DWORD flags = https ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET hRequest = WinHttpOpenRequest(hConnect,
                                            L"GET",
                                            reinterpret_cast<LPCWSTR>(path.utf16()),
                                            nullptr,
                                            WINHTTP_NO_REFERER,
                                            WINHTTP_DEFAULT_ACCEPT_TYPES,
                                            flags);
    if (!hRequest) {
        errorOut = winHttpErrorMessage(GetLastError());
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    // Follow redirects (CDN / http→https)
    DWORD redirectPolicy = WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS;
    WinHttpSetOption(hRequest, WINHTTP_OPTION_REDIRECT_POLICY, &redirectPolicy, sizeof(redirectPolicy));

    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                            WINHTTP_NO_REQUEST_DATA, 0, 0, 0)
        || !WinHttpReceiveResponse(hRequest, nullptr)) {
        errorOut = winHttpErrorMessage(GetLastError());
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    DWORD statusCode = 0;
    DWORD statusSize = sizeof(statusCode);
    WinHttpQueryHeaders(hRequest,
                        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                        WINHTTP_HEADER_NAME_BY_INDEX,
                        &statusCode,
                        &statusSize,
                        WINHTTP_NO_HEADER_INDEX);
    if (statusCode < 200 || statusCode >= 300) {
        errorOut = QStringLiteral("HTTP %1").arg(statusCode);
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    qint64 contentLength = -1;
    wchar_t lenBuf[64] = {};
    DWORD lenSize = sizeof(lenBuf);
    if (WinHttpQueryHeaders(hRequest,
                            WINHTTP_QUERY_CONTENT_LENGTH,
                            WINHTTP_HEADER_NAME_BY_INDEX,
                            lenBuf,
                            &lenSize,
                            WINHTTP_NO_HEADER_INDEX)) {
        contentLength = QString::fromWCharArray(lenBuf).toLongLong();
    }

    QFile out(destPath);
    if (!out.open(QIODevice::WriteOnly)) {
        errorOut = QStringLiteral("Cannot create file:\n%1").arg(destPath);
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    qint64 done = 0;
    DWORD avail = 0;
    while (WinHttpQueryDataAvailable(hRequest, &avail)) {
        if (avail == 0) {
            break;
        }
        QByteArray buf;
        buf.resize(int(avail));
        DWORD read = 0;
        if (!WinHttpReadData(hRequest, buf.data(), avail, &read)) {
            errorOut = winHttpErrorMessage(GetLastError());
            out.close();
            out.remove();
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }
        if (read == 0) {
            break;
        }
        buf.resize(int(read));
        if (out.write(buf) != qint64(read)) {
            errorOut = QStringLiteral("Write failed");
            out.close();
            out.remove();
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }
        done += read;
        if (onProgress) {
            onProgress(done, contentLength > 0 ? contentLength : 0);
        }
    }

    out.close();
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    if (done < 1024) {
        QFile::remove(destPath);
        errorOut = QStringLiteral("Downloaded file is empty or too small");
        return false;
    }
    return true;
}
#endif

} // namespace

UpdateManager::UpdateManager(const QString &setupUrl,
                             const QString &setupFileName,
                             QObject *parent)
    : QObject(parent)
    , m_setupUrl(setupUrl)
    , m_setupFileName(setupFileName)
{
    m_downloadDir = QDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
                        .filePath(QStringLiteral("PicassoUpdate"));
    connect(this, &UpdateManager::progress, this, &UpdateManager::setProgress);
}

void UpdateManager::start()
{
    emit statusChanged(QStringLiteral("Downloading update…\n%1").arg(m_setupFileName));
    setProgress(0, 1);
    downloadSetup();
}

void UpdateManager::downloadSetup()
{
    QDir().mkpath(m_downloadDir);
    const QString setupPath = QDir(m_downloadDir).filePath(m_setupFileName);
    if (QFile::exists(setupPath)) {
        QFile::remove(setupPath);
    }

#ifdef Q_OS_WIN
    // System TLS (Schannel) via WinHTTP — does not depend on Qt OpenSSL plugins.
    // Must not block UI thread.
    const QString url = m_setupUrl;
    auto *thread = QThread::create([this, url, setupPath]() {
        QString err;
        const bool ok = downloadWithWinHttp(
            url,
            setupPath,
            [this](qint64 done, qint64 total) {
                QMetaObject::invokeMethod(this, [this, done, total]() {
                    emit progress(done, total);
                }, Qt::QueuedConnection);
            },
            err);
        QMetaObject::invokeMethod(this, [this, ok, setupPath, err]() {
            if (!ok) {
                QFile::remove(setupPath);
                emit error(QStringLiteral("Download failed:\n%1\n%2").arg(m_setupUrl, err));
                return;
            }
            downloadFinishedOk(setupPath);
        }, Qt::QueuedConnection);
    });
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
#else
    Q_UNUSED(setupPath);
    emit error(QStringLiteral("Download is only implemented on Windows"));
#endif
}

void UpdateManager::downloadFinishedOk(const QString &setupPath)
{
    if (!QFile::exists(setupPath) || QFileInfo(setupPath).size() < 1024) {
        QFile::remove(setupPath);
        emit error(QStringLiteral("Downloaded file is empty or too small:\n%1").arg(setupPath));
        return;
    }
    launchSetup(setupPath);
}

void UpdateManager::launchSetup(const QString &setupPath)
{
    emit statusChanged(QStringLiteral(
        "Installing update…\nPlease wait. Do not close this window.\n"
        "If Windows asks for permission, allow it."));
    setBusyProgress();

#ifdef Q_OS_WIN
    // AlwaysOnTop updater window can cover the UAC dialog — drop it first.
    emit aboutToElevate();
    if (m_hostWindow) {
        m_hostWindow->setWindowFlag(Qt::WindowStaysOnTopHint, false);
        m_hostWindow->showNormal();
        m_hostWindow->lower();
    }

    const QString workDir = QFileInfo(setupPath).absolutePath();
    const QString logPath = QDir(workDir).filePath(QStringLiteral("picasso_setup.log"));
    /* No /SUPPRESSMSGBOXES: with it, Files-in-use Abort-Retry becomes Abort → exit 5.
     * Still /SILENT so no wizard; Inno CloseApplications + our pre-kill free locks. */
    const QString argsWithLog =
        QStringLiteral("/SILENT /NORESTART /CLOSEAPPLICATIONS "
                       "/FORCECLOSEAPPLICATIONS /LOG=\"%1\"")
            .arg(QDir::toNativeSeparators(logPath));

    // Let Qt apply window flags before UAC appears.
    QTimer::singleShot(400, this, [this, setupPath, argsWithLog, workDir, logPath]() {
        auto *thread = QThread::create([this, setupPath, argsWithLog, workDir, logPath]() {
            /* Unlock {app}: stop Breeze and kill desktop apps before Inno copies files. */
            auto killIm = [](const wchar_t *im) {
                SHELLEXECUTEINFOW k{};
                k.cbSize = sizeof(k);
                k.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
                k.lpVerb = L"open";
                k.lpFile = L"taskkill.exe";
                QString params = QStringLiteral("/F /T /IM %1").arg(QString::fromWCharArray(im));
                k.lpParameters = reinterpret_cast<LPCWSTR>(params.utf16());
                k.nShow = SW_HIDE;
                if (ShellExecuteExW(&k) && k.hProcess) {
                    WaitForSingleObject(k.hProcess, 15000);
                    CloseHandle(k.hProcess);
                }
            };
            {
                SHELLEXECUTEINFOW sc{};
                sc.cbSize = sizeof(sc);
                sc.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
                sc.lpFile = L"sc.exe";
                sc.lpParameters = L"stop Breeze";
                sc.nShow = SW_HIDE;
                if (ShellExecuteExW(&sc) && sc.hProcess) {
                    WaitForSingleObject(sc.hProcess, 20000);
                    CloseHandle(sc.hProcess);
                }
            }
            killIm(L"OfficeN.exe");
            killIm(L"Shop_net.exe");
            killIm(L"Waiter.exe");
            killIm(L"WaiterDesigner.exe");
            killIm(L"CookingProgress.exe");
            killIm(L"service5.exe");
            killIm(L"Service5.exe");
            killIm(L"Updater.exe");
            Sleep(800);

            BOOL elevated = FALSE;
            HANDLE token = nullptr;
            if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
                TOKEN_ELEVATION te{};
                DWORD sz = 0;
                if (GetTokenInformation(token, TokenElevation, &te, sizeof(te), &sz)) {
                    elevated = te.TokenIsElevated;
                }
                CloseHandle(token);
            }

            SHELLEXECUTEINFOW sei{};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
            sei.lpVerb = elevated ? L"open" : L"runas";
            sei.lpFile = reinterpret_cast<LPCWSTR>(setupPath.utf16());
            sei.lpParameters = reinterpret_cast<LPCWSTR>(argsWithLog.utf16());
            sei.lpDirectory = reinterpret_cast<LPCWSTR>(workDir.utf16());
            sei.nShow = SW_SHOWNORMAL;

            if (!ShellExecuteExW(&sei)) {
                const DWORD err = GetLastError();
                QMetaObject::invokeMethod(this, [this, err]() {
                    if (err == ERROR_CANCELLED) {
                        emit error(QStringLiteral(
                            "Installation cancelled.\n"
                            "You must allow administrator permission in the Windows prompt."));
                    } else {
                        emit error(QStringLiteral("Cannot start installer (error %1).").arg(err));
                    }
                }, Qt::QueuedConnection);
                return;
            }

            DWORD exitCode = 1;
            if (sei.hProcess) {
                WaitForSingleObject(sei.hProcess, INFINITE);
                GetExitCodeProcess(sei.hProcess, &exitCode);
                CloseHandle(sei.hProcess);
            }

            QMetaObject::invokeMethod(this, [this, exitCode, setupPath, logPath]() {
                if (exitCode != 0) {
                    /* Inno Setup exit codes: https://jrsoftware.org/ishelp/topic_setupexitcodes.htm
                     * Code 5 is NOT "UAC denied" — it is Cancel/Abort during install
                     * (often file in use; with /SUPPRESSMSGBOXES Abort-Retry-Ignore becomes Abort). */
                    QString hint;
                    switch (exitCode) {
                    case 2:
                        hint = QStringLiteral(
                            "\nInstallation was cancelled before it started.");
                        break;
                    case 4:
                        hint = QStringLiteral(
                            "\nA fatal error occurred while copying files.\n"
                            "Close Waiter / Shop / FrontDesk / Service5 and try again.\n"
                            "Or run this file as administrator:\n%1")
                                   .arg(QDir::toNativeSeparators(setupPath));
                        break;
                    case 5:
                        hint = QStringLiteral(
                            "\nSetup was aborted during installation (often a locked file).\n"
                            "Close all Picasso apps (Task Manager → Waiter.exe, Shop_net.exe, …),\n"
                            "then run this file as administrator:\n%1")
                                   .arg(QDir::toNativeSeparators(setupPath));
                        break;
                    case 7:
                    case 8:
                        hint = QStringLiteral(
                            "\nSetup cannot proceed (another install pending or reboot required).\n"
                            "Restart Windows and try again, or run:\n%1")
                                   .arg(QDir::toNativeSeparators(setupPath));
                        break;
                    default:
                        hint = QStringLiteral(
                            "\nIf this keeps happening, close all Picasso apps and run:\n%1")
                                   .arg(QDir::toNativeSeparators(setupPath));
                        break;
                    }
                    if (QFile::exists(logPath)) {
                        hint += QStringLiteral("\n\nLog: %1").arg(QDir::toNativeSeparators(logPath));
                    }
                    emit error(QStringLiteral("Installer finished with an error (code %1).%2")
                                   .arg(exitCode)
                                   .arg(hint));
                    return;
                }
                emit statusChanged(QStringLiteral("Update completed.\nYou can start the application."));
                if (m_pb) {
                    m_pb->setRange(0, 100);
                    m_pb->setValue(100);
                }
                emit finished();
            }, Qt::QueuedConnection);
        });
        connect(thread, &QThread::finished, thread, &QObject::deleteLater);
        thread->start();
    });
#else
    auto *proc = new QProcess(this);
    proc->setProgram(setupPath);
    proc->setArguments({
        QStringLiteral("/SILENT"),
        QStringLiteral("/NORESTART"),
        QStringLiteral("/CLOSEAPPLICATIONS"),
        QStringLiteral("/FORCECLOSEAPPLICATIONS"),
    });
    proc->setWorkingDirectory(QFileInfo(setupPath).absolutePath());
    connect(proc, &QProcess::errorOccurred, this, [this, proc, setupPath](QProcess::ProcessError) {
        const QString detail = proc->errorString();
        proc->deleteLater();
        emit error(QStringLiteral("Cannot start installer:\n%1\n%2").arg(setupPath, detail));
    });
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [this, proc](int exitCode, QProcess::ExitStatus status) {
                proc->deleteLater();
                if (status != QProcess::NormalExit || exitCode != 0) {
                    emit error(QStringLiteral("Installer finished with an error (code %1).").arg(exitCode));
                    return;
                }
                emit statusChanged(QStringLiteral("Update completed.\nYou can start the application."));
                if (m_pb) {
                    m_pb->setRange(0, 100);
                    m_pb->setValue(100);
                }
                emit finished();
            });
    proc->start();
    if (!proc->waitForStarted(15000)) {
        const QString detail = proc->errorString();
        proc->deleteLater();
        emit error(QStringLiteral("Cannot start installer:\n%1\n%2").arg(setupPath, detail));
    }
#endif
}

void UpdateManager::setProgress(qint64 done, qint64 total)
{
    if (!m_pb) {
        return;
    }
    if (total <= 0) {
        m_pb->setRange(0, 0);
        return;
    }
    m_pb->setRange(0, 100);
    m_pb->setValue(int((double(done) / double(total)) * 100.0));
}

void UpdateManager::setBusyProgress()
{
    if (!m_pb) {
        return;
    }
    m_pb->setRange(0, 0);
}
