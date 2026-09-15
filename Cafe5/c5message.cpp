#include "c5message.h"
#include "ui_c5message.h"
#include "ndataprovider.h"
#include <QClipboard>
#include <QTimer>
#include <QProcess>
#include <QProcessEnvironment>
#include <QUrlQuery>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QThread>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#ifdef Q_OS_WIN
#  include <Windows.h>
#  include <string>
#endif
#if defined(__has_include)
#  if __has_include(<QMediaPlayer>) && __has_include(<QAudioOutput>)
#    include <QMediaPlayer>
#    include <QAudioOutput>
#    define C5MESSAGE_HAS_SOUND 1
#  endif
#endif
#ifndef C5MESSAGE_HAS_SOUND
#  define C5MESSAGE_HAS_SOUND 0
#endif

C5Message::C5Message() :
    C5Dialog(nullptr),
    ui(new Ui::C5Message)
{
    ui->setupUi(this);
#ifndef WAITER
    ui->frame->setFrameShape(QFrame::NoFrame);
#endif
    ui->btnCopy->setVisible(false);
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &C5Message::timeout);
    timer->start(3000);
}

C5Message::~C5Message()
{
    delete ui;
}

int C5Message::error(const QString &errorStr, const QString &yes, const QString &no)
{
    return showMessage(errorStr, 2, yes, no, "");
}

int C5Message::info(const QString &infoStr, const QString &yes, const QString &no, bool playSound)
{
    return showMessage(infoStr, 1, yes, no, "", playSound);
}

int C5Message::question(const QString &questionStr, const QString &yes, const QString &no, const QString &a3)
{
    return showMessage(questionStr, 3, yes, no, a3);
}

void C5Message::timeout()
{
#if C5MESSAGE_HAS_SOUND
    // Optional ding for info() with playSound=true (Shop/Waiter). Display apps may omit Multimedia.
    if (fPlaySound) {
        QMediaPlayer *mp = new QMediaPlayer();
        auto *ao = new QAudioOutput();
        mp->setAudioOutput(ao);
        mp->setSource(QUrl(":/icq.wav"));
        ao->setVolume(0.5);
        mp->play();
    }
#else
    Q_UNUSED(fPlaySound);
#endif
}

int C5Message::showMessage(const QString &text, int tp, const QString &yes, const QString &no, const QString &a3,
                           bool playsound)
{
    C5Message *c5 = new C5Message();
    c5->fPlaySound = playsound;
    c5->ui->btnYes->setText(yes);
    c5->ui->btnCancel->setText(no);

    if(no.isEmpty()) {
        c5->ui->btnCancel->setVisible(false);
    }

    c5->ui->btnA3->setVisible(!a3.isEmpty());
    c5->ui->btnA3->setText(a3);
    QString img;

    switch(tp) {
    case 1:
        img = "info";
        break;

    case 2:
        img = "error";
        c5->ui->btnCopy->setVisible(true);
        break;

    case 3:
        img = "help";
        break;
    }

#ifdef WAITER
    c5->ui->btnYes->setMinimumHeight(50);
    c5->ui->btnCancel->setMinimumHeight(50);
#endif
    c5->ui->img->setPixmap(QPixmap(QString(":/%1.png").arg(img)));
    c5->ui->label->setText(text);
    c5->adjustSize();
    int result = c5->exec();
    delete c5;
    return result;
}

void C5Message::on_btnYes_clicked()
{
    accept();
}

void C5Message::on_btnCancel_clicked()
{
    reject();
}

void C5Message::on_btnA3_clicked()
{
    done(2);
}

void C5Message::on_btnCopy_clicked()
{
    qApp->clipboard()->setText(ui->label->text());
}

void C5Message::on_label_linkActivated(const QString &link)
{
    QUrl url(link);

    if(url.scheme() == QLatin1String("http") || url.scheme() == QLatin1String("https")) {
        QDesktopServices::openUrl(url);
        return;
    }

    if(url.path() == QLatin1String("launch-updater")) {
        QUrlQuery urlQuery(url);
        const QString version = urlQuery.queryItemValue(QStringLiteral("version"));
        QString appName = urlQuery.queryItemValue(QStringLiteral("app"));
        if (appName.isEmpty()) {
            appName = NDataProvider::mAppName;
        }
        if (!tryStartUpdater(appName, version)) {
            C5Message::error(tr("Could not start the updater. Reinstall the application or run the setup from picasso.am."));
            return;
        }
        qApp->exit(0);
    }
}

void C5Message::showEvent(QShowEvent *e)
{
    C5Dialog::showEvent(e);
    QTimer::singleShot(100, [this]() { ui->btnYes->setFocus(); });
}

bool C5Message::tryStartUpdater(const QString &appName, const QString &version)
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
        qWarning("tryStartUpdater: Updater.exe not found (moduleDir=%s)", qPrintable(moduleDir));
        return false;
    }

    const QStringList args{
        QStringLiteral("--app=%1").arg(appName),
        QStringLiteral("--version=%1").arg(version),
        QStringLiteral("--module-dir=%1").arg(QDir::toNativeSeparators(moduleDir)),
    };

    // Updater.exe lives in Picasso\updater\ without Qt DLLs; apps keep runtime in their module folder.
    // Start with cwd + PATH pointing at the module so the loader finds Qt6*.dll.
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
        qDebug("tryStartUpdater: started pid=%lld path=%s cwd=%s",
               static_cast<long long>(pid),
               qPrintable(updaterPath),
               qPrintable(moduleDir));
        // Let the host copy itself to %TEMP% before this process exits the job.
        QThread::msleep(400);
        return true;
    }

#ifdef Q_OS_WIN
    // Fallback: CreateProcess with explicit cwd / env / breakaway from job.
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
            qWarning("tryStartUpdater: CreateProcess flags=0x%lx failed err=%lu",
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

    qWarning("tryStartUpdater: failed to start %s", qPrintable(updaterPath));
    return false;
}
