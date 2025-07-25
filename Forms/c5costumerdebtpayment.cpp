#include "c5costumerdebtpayment.h"
#include "ui_c5costumerdebtpayment.h"
#include "c5cache.h"
#include "c5cashdoc.h"
#include "bclientdebts.h"
#include "c5user.h"
#include "c5message.h"
#include <QDoubleValidator>

C5CostumerDebtPayment::C5CostumerDebtPayment(int type) :
    C5Dialog(),
    ui(new Ui::C5CostumerDebtPayment)
{
    ui->setupUi(this);
    ui->leAmount->setValidator(new QDoubleValidator(-999999999, 999999999, 2));
    ui->leCostumer->setSelector(ui->leCostumerName, cache_goods_partners);
    ui->leCostumer->setCallbackDialog(this);
    ui->leCash->setSelector(ui->leCashName, cache_cash_names);
    ui->leHall->setSelector(ui->leHallName, cache_halls);
    ui->leCash->setCallbackDialog(this);
    connect(ui->leCostumer, SIGNAL(done(QVector<QJsonValue>)), this, SLOT(selectorDone(QVector<QJsonValue>)));
    fBClientDebt.source = type;
    ui->leType->setText(type == BCLIENTDEBTS_SOURCE_SALE ? tr("Customer payment") : tr("Partner payment"));
}

C5CostumerDebtPayment::~C5CostumerDebtPayment()
{
    delete ui;
}

void C5CostumerDebtPayment::setId(const QString &id)
{
    C5Database db;
    db[":f_cash"] = id;
    db.exec("select * from b_clients_debts where f_cash=:f_cash");
    fBClientDebt.getRecord(db);
    db[":f_id"] = fBClientDebt.cash;
    db.exec("select * from a_header where f_id=:f_id");
    fAHeader.getRecord(db);
    db[":f_header"] = fBClientDebt.cash;
    db.exec("select * from e_cash where f_header=:f_header");
    fECash.getRecord(db);
    ui->leCode->setInteger(fBClientDebt.id);
    ui->deDate->setDate(fBClientDebt.date);
    ui->leType->setText(fBClientDebt.source == 1 ? tr("Customer debt payment") : tr("Partner debt payment"));
    ui->leCash->setValue(fECash.cash);
    ui->leHall->setValue(fBClientDebt.flag);
    ui->leCostumer->setValue(fBClientDebt.costumer);
    ui->leAmount->setDouble(fBClientDebt.amount);
    ui->leCurrency->setText(fBClientDebt.currencyName(db));
    ui->leComment->setText(fBClientDebt.comment);
    ui->btnRemove->setEnabled(true);
}

void C5CostumerDebtPayment::setPartnerAndAmount(int partner, double amount, const QString &clearFlag)
{
    ui->leCostumer->setValue(partner);
    ui->leAmount->setDouble(amount);
    fClearFlag = clearFlag;
}

void C5CostumerDebtPayment::selectorCallback(int row, const QJsonArray &values)
{
    switch(row) {
    case cache_cash_names:
        if(values.isEmpty()) {
            ui->leCurrency->clear();
        } else {
            C5Database db;
            fBClientDebt.currency = values.at(2).toInt();
            ui->leCurrency->setText(fBClientDebt.currencyName(db));
        }

        break;
    }
}

void C5CostumerDebtPayment::on_btnCancel_clicked()
{
    reject();
}

void C5CostumerDebtPayment::on_btnOK_clicked()
{
    if(ui->leCostumer->getInteger() == 0) {
        C5Message::error(tr("Costumer must be defined"));
        return;
    }

    if(ui->leCash->getInteger() == 0 && ui->leAmount->getDouble() > 0) {
        C5Message::error(tr("Cash must be defined"));
        return;
    }

    C5Database db;
    fBClientDebt.date = ui->deDate->date();
    fBClientDebt.costumer = ui->leCostumer->getInteger();
    fBClientDebt.amount = ui->leAmount->getDouble();
    fBClientDebt.flag = ui->leHall->getInteger();
    fBClientDebt.comment = ui->leComment->text();

    if(fBClientDebt.id == 0) {
        if(ui->leAmount->getDouble() > 0) {
            C5CashDoc *doc = new C5CashDoc();
            doc->fDebtFlag = fBClientDebt.flag;
            doc->setRelation(true);
            doc->setPartner(fBClientDebt.costumer);

            switch(fBClientDebt.source) {
            case BCLIENTDEBTS_SOURCE_INPUT:
                doc->setCashOutput(ui->leCash->getInteger());
                doc->setComment(tr("Partner dept payment"));
                break;

            case BCLIENTDEBTS_SOURCE_SALE:
                doc->setCashInput(ui->leCash->getInteger());
                doc->setComment(tr("Customer dept payment"));
                break;
            }

            doc->setDate(ui->deDate->date());
            doc->addRow(ui->leCostumerName->text(), ui->leAmount->getDouble());
            doc->save(true);
            fBClientDebt.cash = doc->uuid();
            fBClientDebt.id = doc->fBClientDebtId;
            delete doc;
        }
    } else {
        C5CashDoc *doc = new C5CashDoc();

        if(doc->openDoc(fBClientDebt.cash)) {
            doc->setDate(ui->deDate->date());

            switch(fBClientDebt.source) {
            case BCLIENTDEBTS_SOURCE_INPUT:
                doc->setCashOutput(ui->leCash->getInteger());
                doc->setComment(ui->leComment->text());
                break;

            case BCLIENTDEBTS_SOURCE_SALE:
                doc->setCashInput(ui->leCash->getInteger());
                doc->setComment(ui->leComment->text());
                break;
            }

            doc->updateRow(0, ui->leCostumerName->text(), ui->leAmount->getDouble());
            doc->fDebtFlag = fBClientDebt.flag;
            doc->save(true);
            fBClientDebt.id = doc->fBClientDebtId;
        }

        delete doc;
    }

    QString err;
    fBClientDebt.write(db, err);
    ui->leCode->setInteger(fBClientDebt.id);
    ui->btnRemove->setEnabled(true);

    if(!fClearFlag.isEmpty()) {
        db[":f_id"] = fClearFlag;
        db.exec("update o_header_flags set f_5=0 where f_id=:f_id");
    }

    C5Message::info(tr("Saved"));
    accept();
}

void C5CostumerDebtPayment::on_btnRemove_clicked()
{
    if(C5Message::question(tr("Confirm to remove")) != QDialog::Accepted) {
        return;
    }

    C5Database db;
    db[":f_id"] = ui->leCode->getInteger();
    db.exec("select f_cash from b_clients_debts where f_id=:f_id");
    QString fcash = db.nextRow() ? db.getString(0) : "";
    db[":f_id"] = ui->leCode->getInteger();
    db.exec("delete from b_clients_debts where f_id=:f_id");
    C5CashDoc *doc = new C5CashDoc();

    if(doc->removeDoc(db, fcash)) {
    } else {
        C5Message::error(tr("Could not remove record"));
        return;
    }

    C5Message::info(tr("Removed"));
    accept();
}
