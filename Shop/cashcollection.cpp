#include "cashcollection.h"
#include "ui_cashcollection.h"
#include "c5storedraftwriter.h"
#include "c5user.h"
#include "jsons.h"

CashCollection::CashCollection() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::CashCollection)
{
    ui->setupUi(this);
    fMax = 0;
    fCoinCashId = 0;
    ui->lePurpose->setText(tr("Cash collection") + " " + QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    C5Database db(__c5config.dbParams());
    db[":f_cash"] = __c5config.cashId();
    db.exec("select  sum(ec.f_amount*ec.f_sign) as f_amount "
            "from e_cash ec "
            "left join a_header h on h.f_id=ec.f_header "
            "left join e_cash_names cn on cn.f_id=ec.f_cash  "
            "where  h.f_state=1 and ec.f_cash=:f_cash");
    if (db.nextRow()) {
        fMax = db.getDouble("f_amount");
        ui->leAmount->setDouble(db.getDouble("f_amount"));
    }
    db[":f_cash"] = __c5config.nocashId();
    db[":f_date"] = QDate::currentDate();
    db.exec("select  sum(ec.f_amount*ec.f_sign) as f_amount "
            "from e_cash ec "
            "left join a_header h on h.f_id=ec.f_header "
            "left join e_cash_names cn on cn.f_id=ec.f_cash  "
            "where  h.f_state=1 and ec.f_cash=:f_cash and h.f_date=:f_date ");
    if (db.nextRow()) {
        ui->leAmountCard->setDouble(db.getDouble("f_amount"));
    }
    db[":f_datecash"] = QDate::currentDate();
    db[":f_state"] = ORDER_STATE_CLOSE;
    db[":f_hall"] = __c5config.defaultHall();
    db.exec("select sum(f_amountprepaid) as f_amountprepaid "
            "from o_header h "
            "where h.f_datecash=:f_datecash and h.f_state=:f_state ");
    if (db.nextRow()) {
        ui->leAmountPrepaid->setDouble(db.getDouble("f_amountprepaid"));
    }
    db[":f_id"] = __c5config.fSettingsId;
    db.exec("select f_config from sys_json_config where f_id=:f_id");
    if (db.nextRow()) {
        QJsonObject jo = __strjson(db.getString(0));
        fCoinCashId = jo["coincash_id"].toInt();
    }
    ui->lbAmountCoin->setVisible(fCoinCashId > 0);
    ui->leAmountCoin->setVisible(fCoinCashId > 0);
    adjustSize();
}

CashCollection::~CashCollection()
{
    delete ui;
}

void CashCollection::on_btnCancel_clicked()
{
    reject();
}

void CashCollection::on_btnSave_clicked()
{
    if (ui->leAmount->getDouble() < 0.001) {
        C5Message::error(tr("Amount must be greater then 0"));
        return;
    }
    C5Database db(__c5config.dbParams());
    if (!db.open()) {
        C5Message::error(db.fLastError);
        return;
    }
    db.startTransaction();
    C5StoreDraftWriter dw(db);
    int counter = dw.counterAType(DOC_TYPE_CASH);
    if (counter == 0) {
        db.rollback();
        C5Message::error(dw.fErrorMsg);
        return;
    }
    QString headerPrefix;
    int headerId;
    if (!dw.hallId(headerPrefix, headerId, __c5config.defaultHall())) {
        db.rollback();
        C5Message::error(dw.fErrorMsg);
        return;
    }
    QString cashdocid;
    if (!dw.writeAHeader(cashdocid, QString::number(counter), DOC_STATE_SAVED, DOC_TYPE_CASH, __user->id(),
                         QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, ui->leAmount->getDouble(),
                         ui->lePurpose->text(), 1, __c5config.getValue(param_default_currency).toInt())) {
        db.rollback();
        C5Message::error(dw.fErrorMsg);
        return;
    }
    if (!dw.writeAHeaderCash(cashdocid, 0, __c5config.cashId(), 0, "", "")) {
        db.rollback();
        C5Message::error(dw.fErrorMsg);
        return;
    }
    QString cashUUID;
    if (!dw.writeECash(cashUUID, cashdocid, __c5config.cashId(), -1, ui->lePurpose->text(), ui->leAmount->getDouble(),
                       cashUUID, 1)) {
        db.rollback();
        C5Message::error(dw.fErrorMsg);
        return;
    }
    db.commit();
    C5Message::info(tr("Saved"));
    accept();
}

void CashCollection::on_leAmount_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if (ui->leAmount->getDouble() > fMax) {
        ui->leAmount->setDouble(fMax);
    }
}
