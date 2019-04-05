#include "dlgexitbyversion.h"
#include "ui_dlgexitbyversion.h"
#include <QDir>

DlgExitByVersion::DlgExitByVersion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgExitByVersion)
{
    ui->setupUi(this);
    fCounter = 15;
    connect(&fTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    fTimer.start(1000);
}

DlgExitByVersion::~DlgExitByVersion()
{
    delete ui;
}

void DlgExitByVersion::exit(const QString &appVersion, const QString &dbVersion)
{
    DlgExitByVersion *d = new DlgExitByVersion();
    d->setVersions(appVersion, dbVersion);
    d->exec();
    qApp->quit();
}

void DlgExitByVersion::exit(const QString &msg)
{
    DlgExitByVersion *d = new DlgExitByVersion();
    d->ui->lbMessage->setText(msg);
    d->exec();
    qApp->quit();
}

void DlgExitByVersion::timeout()
{
    ui->lbTime->setText(QString("%1...").arg(fCounter));
    fCounter--;
    if (fCounter < 1) {
        QDir d;
        d.remove(d.homePath() + "/" + _APPLICATION_ + "/log.txt");
        qApp->exit(0);
    }
}

void DlgExitByVersion::on_btnClose_clicked()
{
    qApp->exit(0);
}

void DlgExitByVersion::setVersions(const QString &appVersion, const QString &dbVersion)
{
    ui->lbMessage->setText(QString("Application version %1 <br> incompatible with database version %2")
                           .arg(appVersion)
                           .arg(dbVersion));
}
