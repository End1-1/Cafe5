#include "c5message.h"
#include "ui_c5message.h"
#include "ndataprovider.h"
#include <QClipboard>
#include <QTimer>
#include <QProcess>
#include <QUrlQuery>
#include <QDir>
#include <QFile>
#include <QThread>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QDebug>
#include <Windows.h>
#include <string>
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
            C5Message::error(tr("Updater not found. Please download the update from the link."));
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
    const QString updaterPath = QDir::toNativeSeparators(
        QCoreApplication::applicationDirPath() + QStringLiteral("/updater.exe"));
    if (!QFile::exists(updaterPath)) {
        return false;
    }

    const QStringList args{
        QStringLiteral("--app=%1").arg(appName),
        QStringLiteral("--version=%1").arg(version),
    };

    QString cmd = QStringLiteral("\"%1\"").arg(updaterPath);
    for (const QString &a : args) {
        cmd += QLatin1Char(' ');
        cmd += a.contains(QLatin1Char(' ')) ? QStringLiteral("\"%1\"").arg(a) : a;
    }

    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi{};
    std::wstring cmdLine = cmd.toStdWString();
    const BOOL ok = CreateProcessW(
        nullptr,
        cmdLine.data(),
        nullptr,
        nullptr,
        FALSE,
        CREATE_BREAKAWAY_FROM_JOB | DETACHED_PROCESS,
        nullptr,
        nullptr,
        &si,
        &pi);

    if (!ok) {
        qDebug() << "CreateProcess failed for updater:" << GetLastError();
        return false;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}
