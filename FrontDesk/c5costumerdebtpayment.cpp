#include "c5costumerdebtpayment.h"
#include "ui_c5costumerdebtpayment.h"
#include "c5cache.h"
#include "c5cashdoc.h"
#include <QDoubleValidator>

C5CostumerDebtPayment::C5CostumerDebtPayment(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5CostumerDebtPayment)
{
    ui->setupUi(this);
    ui->leAmount->setValidator(new QDoubleValidator(-999999999, 999999999, 2));
    ui->leCostumer->setSelector(dbParams, ui->leCostumerName, cache_discount_client);
    ui->leCostumer->setCallbackDialog(this);
    ui->leCash->setSelector(dbParams, ui->leCashName, cache_cash_names);
    connect(ui->leCostumer, SIGNAL(done(QList<QVariant>)), this, SLOT(selectorDone(QList<QVariant>)));
}

C5CostumerDebtPayment::~C5CostumerDebtPayment()
{
    delete ui;
}

void C5CostumerDebtPayment::setId(int id)
{
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select f_id, f_date, f_costumer, f_cash, f_amount, f_govnumber from b_clients_debts where f_id=:f_id");
    if (db.nextRow()) {
        ui->leCode->setInteger(id);
        ui->deDate->setDate(db.getDate("f_date"));
        ui->leCostumer->setValue(db.getInt("f_costumer"));
        ui->leAmount->setDouble(db.getDouble("f_amount"));
        ui->leGovnumber->setText(db.getString("f_govnumber"));
        ui->btnRemove->setEnabled(true);
        C5CashDoc *doc = new C5CashDoc(fDBParams);
        if (doc->openDoc(db.getString("f_cash"))) {
            ui->leCash->setValue(doc->inputCash());
        }
        delete doc;
    } else {
        C5Message::error(tr("Document not exists"));
    }
}

void C5CostumerDebtPayment::selectorCallback(int row, const QList<QVariant> &values)
{
    Q_UNUSED(row);
    if (values.count() > 0) {
        //ui->leGovnumber->setText(values.at(2).toString());
        //fSelectedGovNumber = values.at(2).toString();
    }
}

void C5CostumerDebtPayment::selectorDone(const QList<QVariant> &values)
{
    if (values.count() > 0) {
        ui->leGovnumber->setText(values.at(3).toString());
        fSelectedGovNumber = values.at(3).toString();
    }
}

void C5CostumerDebtPayment::on_btnCancel_clicked()
{
    reject();
}

void C5CostumerDebtPayment::on_btnOK_clicked()
{
    if (ui->leCostumer->getInteger() == 0) {
        C5Message::error(tr("Costumer must be defined"));
        return;
    }
    if (ui->leCash->getInteger() == 0 && ui->leAmount->getDouble() > 0) {
        C5Message::error(tr("Cash must be defined"));
        return;
    }
//    if (ui->leAmount->getDouble() < 0.001) {
//        C5Message::error(tr("Amount must be greater then 0"));
//        return;
//    }
    C5Database db(fDBParams);
    db[":f_date"] = ui->deDate->date();
    db[":f_costumer"] = ui->leCostumer->getInteger();
    db[":f_amount"] = ui->leAmount->getDouble();
    db[":f_govnumber"] = ui->leGovnumber->text();
    if (ui->leCode->getInteger() == 0) {
        ui->leCode->setInteger(db.insert("b_clients_debts"));
        if (ui->leAmount->getDouble() > 0) {
            C5CashDoc *doc = new C5CashDoc(fDBParams);
            doc->setRelation(true);
            doc->setCashInput(ui->leCash->getInteger());
            doc->setDate(ui->deDate->date());
            doc->setComment(tr("Dept payment") + ", " + ui->leCostumerName->text() + " " + ui->leGovnumber->text());
            doc->addRow(tr("Dept payment") + ", " + ui->leCostumerName->text()  + " " + ui->leGovnumber->text(), ui->leAmount->getDouble());
            doc->save(true);
            db[":f_cash"] = doc->uuid();
            delete doc;
        }
        db[":f_id"] = ui->leCode->getInteger();
        db.exec("update b_clients_debts set f_cash=:f_cash where f_id=:f_id");
    } else {
        db.update("b_clients_debts", where_id(ui->leCode->getInteger()));
        db[":f_id"] = ui->leCode->getInteger();
        db.exec("select f_cash from b_clients_debts where f_id=:f_id");
        if (db.nextRow() && ui->leAmount->getDouble() > 0) {
            C5CashDoc *doc = new C5CashDoc(fDBParams);
            if (doc->openDoc(db.getString("f_cash"))) {
                doc->setDate(ui->deDate->date());
                doc->setComment(tr("Dept payment") + ", " + ui->leCostumerName->text());
                doc->updateRow(0, tr("Dept payment") + ", " + ui->leCostumerName->text(), ui->leAmount->getDouble());
                doc->save(true);
            }
            delete doc;
        }
    }
    ui->btnRemove->setEnabled(true);
    C5Message::info(tr("Saved"));
}

void C5CostumerDebtPayment::on_btnRemove_clicked()
{
    if (C5Message::question(tr("Confirm to remove")) != QDialog::Accepted) {
        return;
    }
    C5Database db(fDBParams);
    db.startTransaction();
    db[":f_id"] = ui->leCode->getInteger();
    db.exec("select f_cash from b_clients_debts where f_id=:f_id");
    QString fcash = db.nextRow() ? db.getString(0) : "";
    db[":f_id"] = ui->leCode->getInteger();
    db.exec("delete from b_clients_debts where f_id=:f_id");
    C5CashDoc *doc = new C5CashDoc(fDBParams);
    if (doc->removeDoc(fDBParams, fcash)) {
        db.commit();
    } else {
        db.rollback();
        C5Message::error(tr("Could not remove record"));
        return;
    }
    C5Message::info(tr("Removed"));
    accept();
}
