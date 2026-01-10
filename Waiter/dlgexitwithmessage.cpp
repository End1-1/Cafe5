#include "dlgexitwithmessage.h"
#include "ui_dlgexitwithmessage.h"
#include "c5config.h"

DlgExitWithMessage::DlgExitWithMessage(C5User *user) :
    C5Dialog(user),
    ui(new Ui::DlgExitWithMessage)
{
    ui->setupUi(this);
}

DlgExitWithMessage::~DlgExitWithMessage()
{
    delete ui;
}

void DlgExitWithMessage::openDialog(C5User *user, const QString &msg, int closeTimeout)
{
    DlgExitWithMessage *d = new DlgExitWithMessage(user);
    d->ui->lbMessage->setText(msg);
    d->setCloseTimeout(closeTimeout);
    d->exec();
    delete d;
}

void DlgExitWithMessage::timeout()
{
    fCloseTimeout--;
    ui->lbCloseTime->setText(QString("%1...").arg(fCloseTimeout));

    if(fCloseTimeout == 0) {
        qApp->quit();
    }
}

void DlgExitWithMessage::on_btnClose_clicked()
{
    qApp->quit();
}

void DlgExitWithMessage::setCloseTimeout(int closeTimeout)
{
    if(closeTimeout == 0) {
        ui->lbCloseTime->setVisible(false);
        return;
    }

    fCloseTimeout = closeTimeout;
    ui->lbCloseTime->setText(QString::number(closeTimeout));
    connect(&fTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    fTimer.start(1000);
}
