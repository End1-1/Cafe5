#include "c5cashdoc.h"
#include "ui_c5cashdoc.h"
#include "c5storedoc.h"
#include "c5utils.h"
#include "c5double.h"
#include "c5cache.h"
#include "c5mainwindow.h"
#include "c5inputdate.h"
#include "c5storedraftwriter.h"

C5CashDoc::C5CashDoc(const QStringList &dbParams, QWidget *parent) :
    C5Document(dbParams, parent),
    ui(new Ui::C5CashDoc)
{
    ui->setupUi(this);
    fIcon = ":/cash.png";
    fLabel = tr("Cash document");
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 0, 400, 100, 0);
    ui->leDocNum->setPlaceholderText(QString("%1").arg(genNumber(DOC_TYPE_CASH), C5Config::docNumDigitsInput(), 10, QChar('0')));
    ui->leInput->setSelector(dbParams, ui->leInputName, cache_cash_names);
    ui->leOutput->setSelector(dbParams, ui->leOutputName, cache_cash_names);
    ui->lePartner->setSelector(dbParams, ui->lePartnerName, cache_goods_partners);
    ui->lbStoreDoc->setEnabled(false);
    ui->leStoreDoc->setEnabled(false);
    ui->btnOpenStoreDoc->setEnabled(false);
    fRelation = false;
    fActionFromSale = nullptr;
    fActionDraft = nullptr;
    fActionSave = nullptr;
}

C5CashDoc::~C5CashDoc()
{
    delete ui;
    if (fActionFromSale) {
        delete fActionFromSale;
    }
}

QToolBar *C5CashDoc::toolBar()
{
    if (!fToolBar) {
        C5Widget::toolBar();
        fActionSave = fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(save()));
        fToolBar->addAction(QIcon(":/delete.png"), tr("Remove"), this, SLOT(removeDoc()));
        fActionDraft = fToolBar->addAction(QIcon(":/draft.png"), tr("Draft"), this, SLOT(draft()));
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
    C5LineEdit *l = ui->tbl->createLineEdit(row, 2);
    l->setText(t);
    l = ui->tbl->createLineEdit(row, 3);
    l->setValidator(new QDoubleValidator(0, 999999999, 2));
    l->setDouble(a);
    connect(l, SIGNAL(textChanged(QString)), this, SLOT(amountChanged(QString)));
    amountChanged("");
}

void C5CashDoc::updateRow(int row, const QString &t, double a)
{
    ui->tbl->lineEdit(row, 2)->setText(t);
    ui->tbl->lineEdit(row, 3)->setDouble(a);
    amountChanged("");
}

bool C5CashDoc::openDoc(const QString &uuid)
{
    if (fActionFromSale) {
        fActionFromSale->setVisible(false);
    }
    fUuid = uuid;
    C5Database db(fDBParams);
    C5StoreDraftWriter dw(db);
    if (!dw.readAHeader(uuid)) {
        C5Message::error(dw.fErrorMsg);
        return false;
    }
    if (fActionDraft) {
        fActionDraft->setEnabled(dw.value(container_aheader, 0, "f_state").toInt() == DOC_STATE_SAVED);
    }
    ui->deDate->setDate(dw.value(container_aheader, 0, "f_date").toDate());
    ui->leDocNum->setText(dw.value(container_aheader, 0, "f_userid").toString());
    ui->lePartner->setValue(dw.value(container_aheader, 0, "f_partner").toString());
    fRelation = dw.value(container_aheadercash, 0, "f_related").toInt();
    ui->leInput->setValue(dw.value(container_aheadercash, 0, "f_cashin").toString());
    ui->leOutput->setValue(dw.value(container_aheadercash, 0, "f_cashout").toString());
    ui->leRemarks->setText(dw.value(container_aheader, 0, "f_comment").toString());
    fStoreUuid = dw.value(container_aheadercash, 0, "f_storedoc").toString();
    ui->lePartner->setValue(dw.value(container_aheader, 0, "f_partner").toString());
    for (int i = 0; i < dw.rowCount(container_ecash); i++) {
        int row = -1;
        for (int j = 0; j < ui->tbl->rowCount(); j++) {
            if (ui->tbl->getString(j, 4) == dw.value(container_ecash, i, "f_base").toString()) {
                row = j;
                break;
            }
        }
        if (row == -1) {
            row = ui->tbl->addEmptyRow();
        }
        if (dw.value(container_ecash, i, "f_sign").toInt() == 1) {
            ui->tbl->setString(row, 0, dw.value(container_ecash, i, "f_id").toString());
        } else {
            ui->tbl->setString(row, 1, dw.value(container_ecash, i, "f_id").toString());
        }
        ui->tbl->createLineEdit(row, 2)->setText(dw.value(container_ecash, i, "f_remarks").toString());
        C5LineEdit *l = ui->tbl->createLineEdit(row, 3);
        l->setDouble(dw.value(container_ecash, i, "f_amount").toDouble());
        l->setValidator(new QDoubleValidator(0, 999999999, 2));
        connect(l, SIGNAL(textChanged(QString)), this, SLOT(amountChanged(QString)));
        ui->tbl->setData(row, 4, dw.value(container_ecash, i, "f_base"));
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

QDate C5CashDoc::date() const
{
    return ui->deDate->date();
}

int C5CashDoc::inputCash()
{
    return ui->leInput->getInteger();
}

int C5CashDoc::outputCash()
{
    return ui->leOutput->getInteger();
}

bool C5CashDoc::removeDoc(const QStringList &dbParams, const QString &uuid)
{
    C5Database db(dbParams);
    db[":f_header"] = uuid;
    db.exec("delete from e_cash where f_header=:f_header");
    db[":f_id"] = uuid;
    db.exec("delete from a_header where f_id=:f_id");
    db[":f_id"] = uuid;
    db.exec("delete from a_header_cash where f_id=:f_id");
    return true;
}

void C5CashDoc::amountChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    double total = 0;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        total += ui->tbl->lineEdit(i, 3)->getDouble();
    }
    ui->leTotal->setDouble(total);
}

void C5CashDoc::draft()
{
    C5Database db(fDBParams);
    C5StoreDraftWriter dw(db);
    dw.updateField("a_header", "f_state", DOC_STATE_DRAFT, "f_id", fUuid);
    C5Message::info(tr("Draft created"));
    fActionDraft->setEnabled(false);
    if (fRelation) {
        ui->witems->setEnabled(true);
        ui->leInput->setEnabled(true);
        ui->leOutput->setEnabled(true);
        ui->tbl->setEnabled(true);
        ui->leRemarks->setEnabled(true);
        ui->lePartner->setEnabled(true);
    }
}

void C5CashDoc::save(bool fromrelation)
{
    if (fRelation && !fromrelation) {
        C5Message::info(tr("The document cannot be edited directly"));
        return;
    }
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
    C5StoreDraftWriter dw(db);
    if (ui->leDocNum->text().isEmpty()) {
        ui->leDocNum->setInteger(genNumber(DOC_TYPE_CASH));
        updateGenNumber(ui->leDocNum->getInteger(), DOC_TYPE_CASH);
    }
    dw.writeAHeader(fUuid, ui->leDocNum->text(), DOC_STATE_SAVED, DOC_TYPE_CASH, __userid, ui->deDate->date(), QDate::currentDate(), QTime::currentTime(), ui->lePartner->getInteger(), ui->leTotal->getDouble(), ui->leRemarks->text(), 0, 0);
    dw.writeAHeaderCash(fUuid, ui->leInput->getInteger(), ui->leOutput->getInteger(), fRelation, fStoreUuid, "");
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        QString idin = ui->tbl->getString(i, 0);
        QString idout = ui->tbl->getString(i, 1);
        QString base = ui->tbl->getString(i, 4);
        if (ui->leInput->getInteger() > 0) {
            dw.writeECash(idin, fUuid, ui->leInput->getInteger(), 1, ui->tbl->getString(i, 2), ui->tbl->lineEdit(i, 3)->getDouble(), base, i);
            ui->tbl->setString(i, 0, idin);
        }
        if (ui->leOutput->getInteger() > 0) { 
            dw.writeECash(idout, fUuid, ui->leOutput->getInteger(), -1, ui->tbl->getString(i, 2), ui->tbl->lineEdit(i, 3)->getDouble(), base, i);
            ui->tbl->setString(i, 1, idout);
        }
        ui->tbl->setString(i, 4, base);
    }    
    foreach (const QString &s, fRemovedRows) {
        db[":f_base"] = s;
        db.exec("delete from e_cash where f_base=:f_base");
    }
    fRemovedRows.clear();
    C5Message::info(tr("Saved"));
    if (fActionDraft) {
        fActionDraft->setEnabled(true);
    }
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
    int shift;
    QString shiftname;
    if (C5InputDate::date(fDBParams, d, shift, shiftname)) {
        C5Database db(fDBParams);
        db[":f_date"] = d;
        db[":f_state"] = ORDER_STATE_CLOSE;
        db[":f_shift"] = shift;
        db.exec("select sum(f_amountcash) from o_header where f_datecash=:f_date and f_state=:f_state and f_shift=:f_shift");
        if (db.nextRow()) {
            ui->leRemarks->setText(QString("%1 %2, %3").arg(tr("Input from sale")).arg(d.toString(FORMAT_DATE_TO_STR)).arg(shiftname));
            int row = ui->tbl->addEmptyRow();
            ui->tbl->createLineEdit(row, 0)->setText(QString("%1 %2, %3").arg(tr("Input from sale")).arg(d.toString(FORMAT_DATE_TO_STR)).arg(shiftname));
            ui->tbl->createLineEdit(row, 1)->setDouble(db.getDouble("f_amount"));
            amountChanged("");
        }
    }
}

void C5CashDoc::on_btnNewRow_clicked()
{
    int row = ui->tbl->addEmptyRow();
    ui->tbl->createLineEdit(row, 2)->setFocus();
    C5LineEdit *l = ui->tbl->createLineEdit(row, 3);
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
    if (C5Message::question(tr("Confirm to remove") + "<br>" + ui->tbl->item(row, 2)->text()) != QDialog::Accepted) {
        return;
    }
    fRemovedRows.append(ui->tbl->getString(row, 4));
    ui->tbl->removeRow(row);
    amountChanged("");
}
