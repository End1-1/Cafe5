#include "c5aboutdlg.h"
#include "ui_c5aboutdlg.h"
#include "c5message.h"
#include "fileversion.h"
#include <QDesktopServices>
#include <QProcess>
#include <QUrl>

static bool runSc(const QStringList &args, QString &output, int timeoutMs = 10000)
{
    QProcess p;
    p.start("sc", args);
    if(!p.waitForStarted(2000)) {
        output = QObject::tr("Failed to start sc.exe");
        return false;
    }
    if(!p.waitForFinished(timeoutMs)) {
        p.kill();
        output = QObject::tr("Service command timeout");
        return false;
    }
    output = QString::fromLocal8Bit(p.readAllStandardOutput()) + "\n"
             + QString::fromLocal8Bit(p.readAllStandardError());
    return p.exitStatus() == QProcess::NormalExit && p.exitCode() == 0;
}

static bool hasBreezeService()
{
    QString out;
    return runSc({"query", "Breeze"}, out, 5000);
}

static bool requestElevatedBreezeRestart(QString &output)
{
    QProcess p;
    const QString cmd = "Start-Process -FilePath 'cmd.exe' -Verb RunAs "
                        "-ArgumentList '/c sc stop \"Breeze\" & sc start \"Breeze\"'";
    p.start("powershell", {"-NoProfile", "-ExecutionPolicy", "Bypass", "-Command", cmd});
    if(!p.waitForStarted(2000)) {
        output = QObject::tr("Failed to start PowerShell for elevation.");
        return false;
    }
    if(!p.waitForFinished(15000)) {
        p.kill();
        output = QObject::tr("UAC request timeout.");
        return false;
    }
    output = QString::fromLocal8Bit(p.readAllStandardOutput()) + "\n"
             + QString::fromLocal8Bit(p.readAllStandardError());
    return p.exitStatus() == QProcess::NormalExit && p.exitCode() == 0;
}

c5aboutdlg::c5aboutdlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::c5aboutdlg)
{
    ui->setupUi(this);
    ui->lbVersion->setText(QString("Version: %").arg(FileVersion::getVersionString(qApp->applicationFilePath())));
    ui->btnRestartBreeze->setVisible(hasBreezeService());
}

c5aboutdlg::~c5aboutdlg()
{
    delete ui;
}

void c5aboutdlg::on_btnClose_clicked()
{
    accept();
}

void c5aboutdlg::on_openWeb_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.picasso.am"));
}

void c5aboutdlg::on_btnRestartBreeze_clicked()
{
    QString elevateOutput;
    if(requestElevatedBreezeRestart(elevateOutput)) {
        C5Message::info(tr("UAC confirmation requested. Breeze restart command was started as administrator."));
        return;
    }

    C5Message::error(tr("Failed to request administrator restart for Breeze.")
                     + "\n\n" + tr("Elevation output:") + "\n" + elevateOutput);
}
