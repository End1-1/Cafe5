#include "c5cashdoc.h"
#include "ui_c5cashdoc.h"
#include "c5storedoc.h"
#include "c5utils.h"
#include "c5double.h"
#include "c5cache.h"
#include "c5mainwindow.h"
#include "c5inputdate.h"
#include <QJsonDocument>
#include <QJsonObject>

C5CashDoc::C5CashDoc(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5CashDoc)
{
    ui->setupUi(this);
    fIcon = ":/cash.png";
    fLabel = tr("Cash document");
    ui->tbl->setColumnWidths(2, 400, 100);
    C5Database db(fDBParams);
    db[":f_id"] = DOC_TYPE_CASH;
    db.exec("select f_counter from a_type where f_id=:f_id for update");
    if (!db.nextRow()) {
        db[":f_id"] = DOC_TYPE_CASH;
        db[":f_counter"] = 1;
        db[":f_name"] = tr("Cash doc");
        db.insert("f_counter", false);
        db.commit();
        db[":f_id"] = DOC_TYPE_CASH;
        db.exec("select f_counter from a_type where f_id=:f_id for update");
        db.nextRow();
    }
    ui->leDocNum->setPlaceholderText(QString("%1").arg(db.getInt(0) + 1, C5Config::docNumDigitsInput(), 10, QChar('0')));
    ui->leInput->setSelector(dbParams, ui->leInputName, cache_cash_names);
    ui->leOutput->setSelector(dbParams, ui->leOutputName, cache_cash_names);
    ui->lePartner->setSelector(dbParams, ui->lePartnerName, cache_goods_partners);
    ui->lbStoreDoc->setEnabled(false);
    ui->leStoreDoc->setEnabled(false);
    ui->btnOpenStoreDoc->setEnabled(false);
    fRelation = false;
}

C5CashDoc::~C5CashDoc()
{
    delete ui;
    //delete fActionFromSale;
}

QToolBar *C5CashDoc::toolBar()
{
    if (!fToolBar) {
        C5Widget::toolBar();
        fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(save()));
        fToolBar->addAction(QIcon(":/delete.png"), tr("Remove"), this, SLOT(removeDoc()));
        fActionFromSale = fToolBar->addAction(QIcon(":/cash.png"), tr("Input from sale"), this, SLOT(inputFromSale()));
    }
    return fToolBar;
}

void C5CashDoc::setRelation(bool r)
{
    fRelation = r;
}

void C5CashDoc::setCashInput(int id)
{
    ui->leInput->setValue(id);
}

void C5CashDoc::setCashOutput(int id)
{
    ui->leOutput->setValue(id);
}

void C5CashDoc::setDate(const QDate &d)
{
    ui->deDate->setDate(d);
}

void C5CashDoc::setComment(const QString &t)
{
    ui->leRemarks->setText(t);
}

void C5CashDoc::addRow(const QString &t, double a)
{
    int row = ui->tbl->addEmptyRow();
    C5LineEdit *l = ui->tbl->createLineEdit(row, 0);
    l->setText(t);
    l = ui->tbl->createLineEdit(row, 1);
    l->setValidator(new QDoubleValidator(0, 999999999, 2));
    l->setDouble(a);
    connect(l, SIGNAL(textChanged(QString)), this, SLOT(amountChanged(QString)));
    amountChanged("");
}

void C5CashDoc::updateRow(int row, const QString &t, double a)
{
    ui->tbl->lineEdit(row, 0)->setText(t);
    ui->tbl->lineEdit(row, 1)->setDouble(a);
    amountChanged("");
}

bool C5CashDoc::openDoc(const QString &uuid)
{
    fActionFromSale->setVisible(false);
    fUuid = uuid;
    C5Database db(fDBParams);
    db[":f_id"] = uuid;
    db.exec("select * from a_header where f_id=:f_id");
    if (db.nextRow()) {
        ui->deDate->setDate(db.getDate("f_date"));
        ui->leDocNum->setText(db.getString("f_userid"));
        ui->lePartner->setValue(db.getString("f_partner"));
        QJsonDocument jd = QJsonDocument::fromJson(db.getString("f_raw").toUtf8());
        QJsonObject jo = jd.object();
        int sign = 1;
        fRelation = jo["relation"].toString().toInt() == 1;
        ui->leInput->setValue(jo["cashin"].toString());
        ui->leOutput->setValue(jo["cashout"].toString());
        ui->leRemarks->setText(db.getString("f_comment"));
        fStoreUuid = jo["storedoc"].toString();
        if (ui->leOutput->getInteger() > 0 && ui->leInput->getInteger() == 0) {
            sign = -1;
        }
        db[":f_header"] = uuid;
        db[":f_sign"] = sign;
        db.exec("select * from e_cash where f_header=:f_header and f_sign=:f_sign");
        while (db.nextRow()) {
            int row = ui->tbl->addEmptyRow();
            ui->tbl->createLineEdit(row, 0)->setText(db.getString("f_remarks"));
            C5LineEdit *l = ui->tbl->createLineEdit(row, 1);
            l->setDouble(db.getDouble("f_amount"));
            l->setValidator(new QDoubleValidator(0, 999999999, 2));
            connect(l, SIGNAL(textChanged(QString)), this, SLOT(amountChanged(QString)));
        }
        amountChanged("0");
        if (!fStoreUuid.isEmpty()) {
            db[":f_id"] = fStoreUuid;
            db.exec("select f_userid from a_header where f_id=:f_id");
            if (db.nextRow()) {
                ui->lbStoreDoc->setEnabled(true);
                ui->leStoreDoc->setEnabled(true);
                ui->btnOpenStoreDoc->setEnabled(true);
                ui->leStoreDoc->setText(db.getString("f_userid"));
            }
        }
    } else {
        C5Message::error(tr("Invalid document id"));
        return false;
    }
    if (fRelation) {
        ui->witems->setEnabled(false);
        ui->leInput->setEnabled(false);
        ui->leOutput->setEnabled(false);
        ui->tbl->setEnabled(false);
        ui->leRemarks->setEnabled(false);
        ui->lePartner->setEnabled(false);
    }
    return true;
}

void C5CashDoc::setStoreDoc(const QString &uuid)
{
    fStoreUuid = uuid;
    C5Database db(fDBParams);
    db[":f_id"] = uuid;
    db.exec("select * from a_header where f_id=:f_id");
    if (db.nextRow()) {
        ui->leStoreDoc->setText(db.getString("f_userid"));
        int row = ui->tbl->addEmptyRow();
        ui->tbl->createLineEdit(row, 0)->setText(QString("%1 #%2").arg(tr("Store input")).arg(db.getString("f_userid")));
        ui->tbl->createLineEdit(row, 1)->setDouble(db.getDouble("f_amount"));
        amountChanged("");
        ui->lbStoreDoc->setEnabled(true);
        ui->leStoreDoc->setEnabled(true);
        ui->btnOpenStoreDoc->setEnabled(true);
    }
}

QString C5CashDoc::uuid() const
{
    return fUuid;
}

int C5CashDoc::inputCash()
{
    return ui->leInput->getInteger();
}

bool C5CashDoc::removeDoc(const QStringList &dbParams, const QString &uuid)
{
    C5Database db(dbParams);
    db[":f_header"] = uuid;
    db.exec("delete from e_cash where f_header=:f_header");
    db[":f_id"] = uuid;
    db.exec("delete from a_header where f_id=:f_id");
    return true;
}

void C5CashDoc::amountChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    double total = 0;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        total += ui->tbl->lineEdit(i, 1)->getDouble();
    }
    ui->leTotal->setDouble(total);
}

void C5CashDoc::save()
{
    QString err;
    if (ui->leInput->getInteger() == 0 && ui->leOutput->getInteger() == 0) {
        err += tr("No cash name selected") + "<br>";
    }
    if (ui->leInput->getInteger() == ui->leOutput->getInteger() && ui->leInput->getInteger() > 0) {
        err += tr("Input and output cash desks are identical");
    }
    if (zerodouble(ui->leTotal->getDouble())) {
        err += tr("Empty document") + "<br>";
    }
    if (!err.isEmpty()) {
        C5Message::error(err);
        return;
    }
    C5Database db(fDBParams);
    if (ui->leDocNum->text().isEmpty()) {
        db.startTransaction();
        db[":f_id"] = (DOC_TYPE_CASH);
        db.exec("select f_counter from a_type where f_id=:f_id for update");
        int count = 1;
        if (db.nextRow()) {
            count = db.getInt(0) + 1;
            ui->leDocNum->setText(QString("%1").arg(count, C5Config::docNumDigitsInput(), 10, QChar('0')));
        } else {
            ui->leDocNum->setText(QString("%1").arg(count, C5Config::docNumDigitsInput(), 10, QChar('0')));
        }
        db[":f_counter"] = count;
        if (!db.update("a_type", where_id(DOC_TYPE_CASH))) {
            C5Message::error(db.fLastError);
        }
        db.commit();
    } else {
        int docnum = ui->leDocNum->getInteger();
        db[":f_id"] = DOC_TYPE_CASH;
        db[":f_counter"] = docnum;
        db.exec("update a_type set f_counter=:f_counter where f_id=:f_id and f_counter<:f_counter");
    }
    QJsonObject jo;
    jo["cashin"] = ui->leInput->text();
    jo["cashout"] = ui->leOutput->text();
    jo["storedoc"] = fStoreUuid;
    jo["relation"] = fRelation ? "1" : "0";
    QJsonDocument jd;
    jd.setObject(jo);
    if (fUuid.isEmpty()) {
        fUuid = C5Database::uuid();
        db[":f_id"] = fUuid;
        db[":f_operator"] = __userid;
        db[":f_state"] = DOC_STATE_SAVED;
        db[":f_type"] = DOC_TYPE_CASH;
        db[":f_createdate"] = QDate::currentDate();
        db[":f_createtime"] = QTime::currentTime();
        db.insert("a_header", false);
    } else {
        db[":f_header"] = fUuid;
        db.exec("delete from e_cash where f_header=:f_header");
    }
    db[":f_partner"] = ui->lePartner->getInteger();
    db[":f_userid"] = ui->leDocNum->text();
    db[":f_date"] = ui->deDate->date();
    db[":f_amount"] = ui->leTotal->getDouble();
    db[":f_comment"] = ui->leRemarks->text();
    db[":f_raw"] = jd.toJson();
    db.update("a_header", where_id(fUuid));
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (ui->leInput->getInteger() > 0) {
            db[":f_header"] = fUuid;
            db[":f_sign"] = 1;
            db[":f_cash"] = ui->leInput->getInteger();
            db[":f_remarks"] = ui->tbl->lineEdit(i, 0)->text();
            db[":f_amount"] = ui->tbl->lineEdit(i, 1)->getDouble();
            db.insert("e_cash", false);
        }
        if (ui->leOutput->getInteger() > 0) {
            db[":f_header"] = fUuid;
            db[":f_sign"] = -1;
            db[":f_cash"] = ui->leOutput->getInteger();
            db[":f_remarks"] = ui->tbl->lineEdit(i, 0)->text();
            db[":f_amount"] = ui->tbl->lineEdit(i, 1)->getDouble();
            db.insert("e_cash", false);
        }
    }
    if (!fStoreUuid.isEmpty()) {
        db[":f_id"] = fStoreUuid;
        db.exec("select * from a_header where f_id=:f_id");
        if (db.nextRow()) {
            QJsonDocument jd = QJsonDocument::fromJson(db.getString("f_raw").toUtf8());
            QJsonObject jo = jd.object();
            jo["cashdoc"] = fUuid;
            jd.setObject(jo);
            db[":f_raw"] = jd.toJson();
            db[":f_id"] = fStoreUuid;
            db.exec("update a_header set f_raw=:f_raw where f_id=:f_id");
        }
    }
    C5Message::info(tr("Saved"));
}

void C5CashDoc::removeDoc()
{
    if (fRelation) {
        C5Message::error(tr("Cannot remove this document, becouse an relation exist"));
        return;
    }
    if (C5Message::question(tr("Confirm to remove selected documents")) != QDialog::Accepted) {
        return;
    }
    if (!fUuid.isEmpty()) {
        if (!removeDoc(fDBParams, fUuid)) {
            return;
        }
    }
    __mainWindow->removeTab(this);
}

void C5CashDoc::inputFromSale()
{
    QDate d = QDate::currentDate();
    if (C5InputDate::date(d)) {
        C5Database db(fDBParams);
        db[":f_date"] = d;
        db.exec("select sum(f_amountcash) from o_header where f_datecash=:f_date");
        if (db.nextRow()) {
            ui->leRemarks->setText(QString("%1 %2").arg(tr("Input from sale")).arg(d.toString(FORMAT_DATE_TO_STR)));
            int row = ui->tbl->addEmptyRow();
            ui->tbl->createLineEdit(row, 0)->setText(QString("%1 %2").arg(tr("Input from sale")).arg(d.toString(FORMAT_DATE_TO_STR)));
            ui->tbl->createLineEdit(row, 1)->setDouble(db.getDouble("f_amount"));
            amountChanged("");
        }
    }
}

void C5CashDoc::on_btnNewRow_clicked()
{
    int row = ui->tbl->addEmptyRow();
    ui->tbl->createLineEdit(row, 0)->setFocus();
    C5LineEdit *l = ui->tbl->createLineEdit(row, 1);
    l->setValidator(new QDoubleValidator(0, 999999999, 2));
    connect(l, SIGNAL(textChanged(QString)), this, SLOT(amountChanged(QString)));
}

void C5CashDoc::on_btnOpenStoreDoc_clicked()
{
    if (fStoreUuid.isEmpty()) {
        return;
    }
    auto *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
    if (!sd->openDoc(fStoreUuid)) {
        __mainWindow->removeTab(sd);
    }
}

void C5CashDoc::on_btnRemoveRow_clicked()
{
    int row = ui->tbl->currentRow();
    if (row < 0) {
        return;
    }
    if (C5Message::question(tr("Confirm to remove") + "<br>" + ui->tbl->item(row, 0)->text()) != QDialog::Accepted) {
        return;
    }
    ui->tbl->removeRow(row);
    amountChanged("");
}
