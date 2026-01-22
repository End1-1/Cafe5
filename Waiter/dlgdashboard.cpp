#include "dlgdashboard.h"
#include "ui_dlgdashboard.h"
#include "c5message.h"
#include "ninterface.h"
#include "c5user.h"
#include "struct_workstationitem.h"
#include "c5permissions.h"
#include "dlgreports.h"
#include "dlgface.h"
#include "dlgrealreports.h"
#include "format_date.h"
#include "format_date.h"
#include "c5printing.h"
#include <QShowEvent>
#include <QPrinter>
#include <QPrinterInfo>
#include <QFile>

DlgDashboard::DlgDashboard(const QJsonObject &jcashbox, C5User *user)
    : C5WaiterDialog(user),
      ui(new Ui::DlgDashboard),
      mCashboxData(jcashbox)
{
    ui->setupUi(this);
    setup();
}

DlgDashboard::~DlgDashboard()
{
    delete ui;
}

int DlgDashboard::exec()
{
    C5WaiterDialog::exec();
    return mRoute;
}

void DlgDashboard::showEvent(QShowEvent *e)
{
    C5WaiterDialog::showEvent(e);

    if(e->spontaneous()) {
        return;
    }

    if(mCashboxData.isEmpty()) {
        NInterface::query1("/engine/v2/waiter/cashbox/check-status", mUser->mSessionKey, this,
        {{"cashbox_id", mWorkStation.cashboxId()}},
        [ this ](const QJsonObject & jdoc) {
            mCashboxData = jdoc["cashbox_session"].toObject();
            setup();
        });
    }
}

void DlgDashboard::on_btnOpenCashbox_clicked()
{
    if(C5Message::question(tr("Open new cashbox session")) != QDialog::Accepted) {
        return;
    }

    NInterface::query1("/engine/v2/waiter/cashbox/open", mUser->mSessionKey, this, {
        {"amount_open", 0},
        {"cashbox_id", mWorkStation.cashboxId()}
    },
    [this](const QJsonObject & jdoc) {
        mCashboxData = jdoc["cashbox_session"].toObject();
        setup();
    });
}

void DlgDashboard::setup()
{
    if(mCashboxData.value("f_id").toInt() == 0) {
        ui->wsessionifno->setVisible(false);
        ui->wsessionstatus->setVisible(true);
        ui->btnOpenCashbox->setEnabled(mUser->check(cp_t5_waiter_open_close_shift));
        ui->btnCloseCashbox->setEnabled(mUser->check(cp_t5_waiter_open_close_shift));
    } else {
        ui->lbShiftNumber->setText(QString::number(mCashboxData["f_id"].toInt()));
        QDateTime shiftOpenDatetime = QDateTime::fromString(mCashboxData["f_date_open"].toString(), FORMAT_DATETIME_TO_STR_MYSQL);
        ui->lbShiftOpen->setText(shiftOpenDatetime.toString(FORMAT_DATETIME_TO_STR));
        ui->lbShiftOpenUser->setText(mCashboxData["f_user_open_name"].toString());
        ui->lbShiftOrdersCount->setText(QString::number(mCashboxData["f_orders_count"].toInt()));
        ui->lbShiftOrdersAmount->setText((mCashboxData["f_amount_expected"].toString()));
        ui->wsessionifno->setVisible(true);
        ui->wsessionstatus->setVisible(false);
        ui->btnOpenCashbox->setEnabled(false);
        ui->btnCloseCashbox->setEnabled(true);
    }

    ui->btnOrders->setEnabled(mUser->check(cp_t5_waiter_reports));
    ui->btnReports->setEnabled(mUser->check(cp_t5_waiter_reports));
}

void DlgDashboard::on_btnCloseCashbox_clicked()
{
    if(C5Message::question(tr("Do you want to close active session?")) != QDialog::Accepted) {
        return ;
    }

    NInterface::query1("/engine/v2/waiter/cashbox/close", mUser->mSessionKey, this,
    {{"cashbox_id", mWorkStation.cashboxId()}},
    [this](const QJsonObject & jdoc) {
        mCashboxData = QJsonObject();
        setup();
        C5Printing p;
        QPrinterInfo pi = QPrinterInfo::printerInfo(mWorkStation.defaultPrinter());
        QPrinter printer(pi);
        printer.setPageSize(QPageSize::Custom);
        printer.setFullPage(false);
        QRectF pr = printer.pageRect(QPrinter::DevicePixel);
        constexpr qreal SAFE_RIGHT_MM = 4.0;
        qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
        p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
        p.setFont(qApp->font());
        QString logoFile = qApp->applicationDirPath() + "/logo_receipt.png";

        if(QFile::exists(logoFile)) {
            p.image(logoFile, Qt::AlignHCenter);
            p.br();
        }

        p.ctext(tr("Closing session") + " " + QString::number(mCashboxData.value("f_id").toInt()));
        p.br();
        p.ctext(mCashboxData["f_date_close"].toString());
        p.br();
        p.lrtext(tr("Open"), mCashboxData["f_user_open_name"].toString());
        p.br();
        p.lrtext(tr("Close"), mCashboxData["f_user_close_name"].toString());
        p.br();
        p.lrtext(tr("Operations"), mCashboxData["f_orders_count"].toString());
        p.br();
        p.lrtext(tr("Expected amount"), mCashboxData["f_amount_expected"].toString());
        p.br();
        p.line();
        p.br();
        p.print(printer);
    });
}

void DlgDashboard::on_btnGoHall_clicked()
{
    mRoute = 3;
    accept();
}

void DlgDashboard::on_btnOrders_clicked()
{
    DlgReports d(mUser);
    d.exec();
}

void DlgDashboard::on_btnClose_clicked()
{
    reject();
}

void DlgDashboard::on_btnReports_clicked()
{
    DlgRealReports d(mUser);
    d.exec();
}
