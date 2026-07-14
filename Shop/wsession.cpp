#include "wsession.h"
#include "c5message.h"
#include "c5permissions.h"
#include "c5user.h"
#include "ninterface.h"
#include "struct_workstationitem.h"
#include "ui_wsession.h"
#include <QShowEvent>

WSession::WSession(C5User *user, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WSession)
    , mUser(user)
{
    ui->setupUi(this);
    ui->lbStaff->setText(user->shortFullName());
    setup();
}

WSession::~WSession()
{
    delete ui;
}

void WSession::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);

    if (e->spontaneous()) {
        return;
    }

    checkStatus();
}

void WSession::checkStatus()
{
    if (mFinished || mOpening) {
        return;
    }

    const int cashboxId = mWorkStation.cashboxId();

    if (cashboxId <= 0) {
        ui->lbCashboxClosed->setText(tr("Cashbox is not configured for this workstation."));
        ui->btnOpenCashbox->setEnabled(false);
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/waiter/cashbox/check-status"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("cashbox_id"), cashboxId}},
                       [this](const QJsonObject &jdoc) {
                           if (mFinished || mOpening) {
                               return;
                           }

                           mCashboxData = jdoc.value(QStringLiteral("cashbox_session")).toObject();
                           setup();

                           if (mCashboxData.value(QStringLiteral("f_id")).toInt() > 0) {
                               finishSessionOpen();
                           }
                       });
}

void WSession::on_btnOpenCashbox_clicked()
{
    if (mFinished || mOpening) {
        return;
    }

    if (C5Message::question(tr("Open new cashbox session")) != QDialog::Accepted) {
        return;
    }

    mOpening = true;
    ui->btnOpenCashbox->setEnabled(false);

    NInterface::query1(QStringLiteral("/engine/v2/waiter/cashbox/open"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("amount_open"), 0},
                        {QStringLiteral("cashbox_id"), mWorkStation.cashboxId()}},
                       [this](const QJsonObject &jdoc) {
                           if (mFinished) {
                               return;
                           }

                           mCashboxData = jdoc.value(QStringLiteral("cashbox_session")).toObject();

                           if (mCashboxData.value(QStringLiteral("f_id")).toInt() > 0) {
                               finishSessionOpen();
                           } else {
                               mOpening = false;
                               setup();
                           }
                       });
}

void WSession::finishSessionOpen()
{
    if (mFinished) {
        return;
    }

    mFinished = true;
    mOpening = true;
    ui->btnOpenCashbox->setEnabled(false);
    emit sessionOpened(mCashboxData);
}

void WSession::setup()
{
    if (mFinished) {
        return;
    }

    const bool hasSession = mCashboxData.value(QStringLiteral("f_id")).toInt() > 0;
    ui->btnOpenCashbox->setEnabled(!hasSession && !mOpening && mUser->check(cp_t5_waiter_open_close_shift));
}
