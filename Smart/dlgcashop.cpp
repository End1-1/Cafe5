#include "dlgcashop.h"
#include "ui_dlgcashop.h"
#include "c5database.h"
#include "c5message.h"
#include "c5config.h"
#include "c5utils.h"
#include "c5user.h"
#include <QStyle>
#include <QDoubleValidator>
#include <QDate>
#include <QTime>
#include <QJsonDocument>
#include <QJsonObject>

namespace {

int drawerCashId(C5Database &db)
{
    int cashId = C5Config::fMainJson.value(QStringLiteral("cash_cash_id")).toVariant().toInt();

    if(cashId > 0) {
        return cashId;
    }

    cashId = __c5config.cashId();

    if(cashId > 0) {
        return cashId;
    }

    QString configName = C5Config::fSettingsName.trimmed();

    if(configName.isEmpty()) {
        configName = __c5config.getRegValue(QStringLiteral("ss_settings")).toString().trimmed();
    }

    if(configName.isEmpty()) {
        return 0;
    }

    db[":f_name"] = configName;

    if(!db.exec("select f_config from sys_json_config where f_name=:f_name") || !db.nextRow()) {
        return 0;
    }

    const QJsonObject cfg = QJsonDocument::fromJson(db.getString(0).toUtf8()).object();
    return cfg.value(QStringLiteral("cash_cash_id")).toVariant().toInt();
}

}

DlgCashOp::DlgCashOp(bool in, C5User *u) :
    C5Dialog(u),
    ui(new Ui::DlgCashOp)
{
    ui->setupUi(this);
    fUser = u;
    ui->lbInOut->setStyleSheet(in ? "color:green;" : "color:red;");
    ui->lbInOut->style()->polish(ui->lbInOut);
    fSign = in;
    ui->lbInOut->setText(in ? tr("In") : tr("Out"));
    ui->leAmount->setValidator(new QDoubleValidator(0, 999999999, 2));
}

DlgCashOp::~DlgCashOp()
{
    delete ui;
}

void DlgCashOp::on_btnCancel_clicked()
{
    reject();
}

void DlgCashOp::on_btnSave_clicked()
{
    if(ui->leAmount->getDouble() < 0.01) {
        C5Message::error(tr("Amount cannot be zero"));
        return;
    }

    if(ui->lePurpose->isEmpty()) {
        C5Message::error(tr("Purpose must be defined"));
        return;
    }

    C5Database db;
    const int cashId = drawerCashId(db);

    if(cashId <= 0) {
        C5Message::error(tr("Cash is not configured"));
        return;
    }

    db[":f_id"] = DOC_TYPE_CASH;

    if(!db.exec("select cast(f_counter + 1 as int) from a_type where f_id=:f_id") || !db.nextRow()) {
        C5Message::error(db.fLastError);
        return;
    }

    const int counter = db.getInt(0);
    db[":f_id"] = DOC_TYPE_CASH;
    db[":f_counter"] = counter;

    if(!db.exec("update a_type set f_counter=:f_counter where f_id=:f_id and f_counter<:f_counter")) {
        C5Message::error(db.fLastError);
        return;
    }

    const QString cashdoc = C5Database::uuid();
    db[":f_id"] = cashdoc;
    db[":f_userid"] = QString::number(counter);
    db[":f_state"] = DOC_STATE_SAVED;
    db[":f_type"] = DOC_TYPE_CASH;
    db[":f_operator"] = fUser->id();
    db[":f_date"] = QDate::currentDate();
    db[":f_createdate"] = QDate::currentDate();
    db[":f_createtime"] = QTime::currentTime();
    db[":f_partner"] = 0;
    db[":f_amount"] = ui->leAmount->getDouble();
    db[":f_comment"] = ui->lePurpose->text();
    db[":f_paid"] = 1;
    db[":f_working_session"] = 0;
    db[":f_currency"] = __c5config.getValue(param_default_currency).toInt();

    if(db.insert("a_header", false) == 0) {
        C5Message::error(db.fLastError);
        return;
    }

    db[":f_id"] = cashdoc;
    db[":f_cashin"] = fSign ? cashId : 0;
    db[":f_cashout"] = fSign ? 0 : cashId;
    db[":f_related"] = 0;
    db[":f_storedoc"] = "";
    db[":f_oheader"] = "";

    if(db.insert("a_header_cash", false) == 0) {
        C5Message::error(db.fLastError);
        return;
    }

    db[":f_id"] = C5Database::uuid();
    db[":f_header"] = cashdoc;
    db[":f_cash"] = cashId;
    db[":f_sign"] = fSign ? 1 : -1;
    db[":f_remarks"] = ui->lePurpose->text();
    db[":f_amount"] = ui->leAmount->getDouble();
    db[":f_base"] = C5Database::uuid();
    db[":f_row"] = 1;
    db[":f_working_session"] = __c5config.getRegValue("sessionid");

    if(db.insert("e_cash", false) == 0) {
        C5Message::error(db.fLastError);
        return;
    }

    C5Message::info(QString("%1<br>%2<br>%3").arg(tr("Saved"), ui->lePurpose->text(), ui->leAmount->text()));
    accept();
}
