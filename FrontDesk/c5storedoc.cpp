#include "c5storedoc.h"
#include "ui_c5storedoc.h"
#include "c5selector.h"
#include "c5cache.h"
#include "c5printing.h"
#include "c5cashdoc.h"
#include "c5printpreview.h"
#include "c5editor.h"
#include "c5mainwindow.h"
#include "ce5partner.h"
#include "ce5goods.h"
#include <QMenu>
#include <QShortcut>

C5StoreDoc::C5StoreDoc(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5StoreDoc)
{
    ui->setupUi(this);
    fIcon = ":/goods.png";
    fLabel = tr("Store document");
    ui->leCash->setSelector(fDBParams, ui->leCashName, cache_cash_names);
    ui->leStoreInput->setSelector(fDBParams, ui->leStoreInputName, cache_goods_store);
    ui->leStoreOutput->setSelector(fDBParams, ui->leStoreOutputName, cache_goods_store);
    ui->lePayment->setSelector(fDBParams, ui->lePaymentName, cache_header_payment);
    ui->leReason->setSelector(fDBParams, ui->leReasonName, cache_store_reason);
    ui->lePartner->setSelector(fDBParams, ui->lePartnerName, cache_goods_partners);
    ui->leComplectationCode->setSelector(fDBParams, ui->leComplectationName, cache_goods, 1, 3);
    disconnect(ui->leComplectationName, SIGNAL(textChanged(QString)), this, SLOT(on_leComplectationName_textChanged(QString)));
    connect(ui->leComplectationName, SIGNAL(textChanged(QString)), this, SLOT(on_leComplectationName_textChanged(QString)));
    ui->tblGoods->setColumnWidths(7, 0, 0, 300, 80, 80, 80, 80);
    ui->tblGoodsStore->setColumnWidths(7, 0, 0, 200, 70, 50, 50, 70);
    ui->btnNewPartner->setVisible(pr(dbParams.at(1), cp_t6_partners));
    ui->btnNewGoods->setVisible(pr(dbParams.at(1), cp_t6_goods));
    ui->leAccepted->setSelector(dbParams, ui->leAcceptedName, cache_users);
    ui->lePassed->setSelector(dbParams, ui->lePassedName, cache_users);
    ui->lbCashDoc->setVisible(!C5Config::noCashDocStore());
    ui->leCash->setVisible(!C5Config::noCashDocStore());
    ui->leCashName->setVisible(!C5Config::noCashDocStore());
    ui->deCashDate->setVisible(!C5Config::noCashDocStore());
    ui->lePayment->setVisible(!C5Config::noCashDocStore());
    ui->lePaymentName->setVisible(!C5Config::noCashDocStore());
    ui->lbPayment->setVisible(!C5Config::noCashDocStore());
    ui->chPaid->setVisible(!C5Config::noCashDocStore());
    ui->btnCreateDoc->setVisible(!C5Config::noCashDocStore());
    ui->leScancode->setVisible(!C5Config::noScanCodeStore());
    fInternalId = "";
    fDocState = DOC_STATE_DRAFT;
    ui->tblGoodsGroup->viewport()->installEventFilter(this);
    fGroupTableCell = nullptr;
    fGroupTableCellMove = false;
    fBasedOnSale = 0;
    connect(ui->leInvoiceNumber, SIGNAL(focusOut()), this, SLOT(checkInvoiceDuplicate()));
    QShortcut *f2 = new QShortcut(QKeySequence(Qt::Key_F2), this);
    connect(f2, &QShortcut::activated, [this](){
        ui->leScancode->setFocus();
    });
    fFocusNextChild = false;
    fCanChangeFocus = true;
    ui->tblAdd->setColumnWidths(ui->tblAdd->columnCount(), 0, 300, 80);
}

C5StoreDoc::~C5StoreDoc()
{
    delete ui;
}

bool C5StoreDoc::openDoc(QString id)
{
    ui->leDocNum->setPlaceholderText("");
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select * from a_header where f_id=:f_id");
    if (!db.nextRow()) {
        C5Message::error(tr("Invalid document number"));
        return false;
    }
    fInternalId = db.getString("f_id");
    ui->leDocNum->setText(db.getString("f_userid"));
    ui->deDate->setDate(db.getDate("f_date"));
    fDocType = db.getInt("f_type");
    fDocState = db.getInt("f_state");
    ui->lePartner->setValue(db.getString("f_partner"));
    ui->leComment->setText(db.getString("f_comment"));
    ui->leTotal->setDouble(db.getDouble("f_amount"));
    QJsonDocument jd = QJsonDocument::fromJson(db.getString("f_raw").toUtf8());
    QJsonObject jo = jd.object();
    ui->leStoreInput->setValue(jo["f_storein"].toString());
    ui->leStoreOutput->setValue(jo["f_storeout"].toString());
    ui->leCash->setValue(jo["f_cash"].toInt());
    fCashUuid = jo["f_cashuuid"].toString();
    fBasedOnSale = jo["based_on_sale"].toInt();
    ui->lePassed->setValue(jo["f_passed"].toString());
    ui->leAccepted->setValue(jo["f_accepted"].toString());
    disconnect(ui->leComplectationName, SIGNAL(textChanged(QString)), this, SLOT(on_leComplectationName_textChanged(QString)));
    ui->leComplectationCode->setValue(jo["f_complectation"].toString());
    ui->leComplectationQty->setText(jo["f_complectationqty"].toString());
    connect(ui->leComplectationName, SIGNAL(textChanged(QString)), this, SLOT(on_leComplectationName_textChanged(QString)));
    ui->deInvoiceDate->setDate(QDate::fromString(jo["f_invoicedate"].toString(), "dd.MM.yyyy"));
    ui->leInvoiceNumber->setText(jo["f_invoice"].toString());
    ui->lePayment->setValue(db.getInt("f_payment"));
    ui->chPaid->setChecked(db.getInt("f_paid") == 1);
    fCashUuid = jo["f_cashuuid"].toString();
    setMode(static_cast<STORE_DOC>(fDocType));
    db[":f_document"] = id;
    switch (fDocType) {
    case DOC_TYPE_STORE_INPUT:
    case DOC_TYPE_STORE_MOVE:
        db[":f_type"] = 1;
        break;
    case DOC_TYPE_STORE_OUTPUT:
    case DOC_TYPE_COMPLECTATION:
        db[":f_type"]= -1;
        break;
    }
    db.exec("select d.f_id, d.f_goods, g.f_name, d.f_qty, u.f_name, d.f_price, d.f_total, d.f_reason \
            from a_store_draft d \
            left join c_goods g on g.f_id=d.f_goods \
            left join c_units u on u.f_id=g.f_unit \
            where d.f_document=:f_document and f_type=:f_type");
    while (db.nextRow()) {
        int row = addGoodsRow();
        ui->leReason->setValue(db.getString("f_reason"));
        ui->tblGoods->setInteger(row, 0, db.getInt(0));
        ui->tblGoods->setInteger(row, 1, db.getInt(1));
        ui->tblGoods->setString(row, 2, db.getString(2));
        ui->tblGoods->lineEdit(row, 3)->setDouble(db.getDouble(3));
        ui->tblGoods->setString(row, 4, db.getString(4));
        ui->tblGoods->lineEdit(row, 5)->setDouble(db.getDouble(5));
        ui->tblGoods->lineEdit(row, 6)->setDouble(db.getDouble(6));
    }
    if (fDocType == DOC_TYPE_COMPLECTATION) {
        db[":f_document"] = id;
        db[":f_type"] = 1;
        db.exec("select d.f_id, d.f_goods, g.f_name, d.f_qty, u.f_name, d.f_price, d.f_total, d.f_reason \
                from a_store_draft d \
                left join c_goods g on g.f_id=d.f_goods \
                left join c_units u on u.f_id=g.f_unit \
                where d.f_document=:f_document and f_type=:f_type");
        if (db.nextRow()) {
            fComplectationId = db.getString("f_id");
            ui->leComplectationCode->setValue(db.getInt("f_goods"));
            ui->leComplectationQty->setDouble(db.getDouble("f_qty"));
        } else {
            C5Message::error(tr("Error in complectation document"));
            return false;
        }
    }    
    db[":f_header"] = id;
    db.exec("select f_name, f_amount from a_complectation_additions where f_header=:f_header order by f_row");
    while (db.nextRow()) {
        int row = ui->tblAdd->rowCount();
        ui->tblAdd->setRowCount(row + 1);
        C5LineEdit *l = ui->tblAdd->createLineEdit(row, 1);
        l->setText(db.getString(0));
        l = ui->tblAdd->createLineEdit(row, 2);
        connect(l, SIGNAL(textEdited(QString)), this, SLOT(tblAddChanged(QString)));
        l->setValidator(new QDoubleValidator());
        l->setDouble(db.getDouble(1));
    }
    setDocEnabled(fDocState == DOC_STATE_DRAFT);
    countTotal();
    if (!fCashUuid.isEmpty()) {
        db[":f_id"] = fCashUuid;
        db.exec("select * from a_header where f_id=:f_id");
        if (db.nextRow()) {
            ui->leCashName->setEnabled(true);
            ui->leCash->setEnabled(true);
            ui->lbCashDoc->setEnabled(true);
            ui->deCashDate->setEnabled(true);
            ui->deCashDate->setDate(db.getDate("f_date"));
        }
    }
    if (fBasedOnSale > 0) {
        ui->wbody->setEnabled(false);
        ui->deDate->setEnabled(false);
        ui->leStoreOutput->setEnabled(false);
        ui->leComment->setEnabled(false);
        ui->leReason->setEnabled(false);
    }
    return true;
}

void C5StoreDoc::setMode(C5StoreDoc::STORE_DOC sd)
{
    ui->lbPayment->setVisible(false);
    ui->lePayment->setVisible(false);
    ui->lePaymentName->setVisible(false);
    ui->chPaid->setVisible(false);
    ui->grComplectation->setVisible(false);
    fDocType = sd;
    bool paymentVisible = !C5Config::noCashDocStore();
    switch (fDocType) {
    case DOC_TYPE_STORE_INPUT:
        ui->lbPayment->setVisible(paymentVisible && true);
        ui->lePayment->setVisible(paymentVisible && true);
        ui->lePaymentName->setVisible(paymentVisible && true);
        ui->chPaid->setVisible(paymentVisible && true);
        break;
    case DOC_TYPE_STORE_MOVE:
        ui->lbCashDoc->setVisible(false);
        ui->leCashName->setVisible(false);
        ui->leCash->setVisible(false);
        ui->deCashDate->setVisible(false);
        ui->btnCreateDoc->setVisible(false);
        break;
    case DOC_TYPE_STORE_OUTPUT:
        ui->lbCashDoc->setVisible(false);
        ui->leCashName->setVisible(false);
        ui->leCash->setVisible(false);
        ui->deCashDate->setVisible(false);
        ui->btnCreateDoc->setVisible(false);
        break;
    case DOC_TYPE_COMPLECTATION:
        ui->grComplectation->setVisible(true);
        break;
    }

    switch (sd) {
    case sdInput:
        ui->leStoreOutput->setVisible(false);
        ui->leStoreOutputName->setVisible(false);
        ui->lbStoreOutput->setVisible(false);
        ui->leReason->setValue(DOC_REASON_INPUT);
        break;
    case sdOutput:
        ui->leStoreInput->setVisible(false);
        ui->leStoreInputName->setVisible(false);
        ui->lePartner->setVisible(false);
        ui->lePartnerName->setVisible(false);
        ui->leInvoiceNumber->setVisible(false);
        ui->deInvoiceDate->setVisible(false);
        ui->lbStoreInput->setVisible(false);
        ui->lbInvoiceDate->setVisible(false);
        ui->lbInvoiceNumber->setVisible(false);
        ui->lbPartner->setVisible(false);
        ui->btnNewPartner->setVisible(false);
        ui->leReason->setValue(DOC_REASON_OUT);
        break;
    case sdMovement:
        ui->lePartner->setVisible(false);
        ui->lePartnerName->setVisible(false);
        ui->leInvoiceNumber->setVisible(false);
        ui->deInvoiceDate->setVisible(false);
        ui->lbInvoiceDate->setVisible(false);
        ui->lbInvoiceNumber->setVisible(false);
        ui->lbPartner->setVisible(false);
        ui->btnNewPartner->setVisible(false);
        ui->leReason->setValue(DOC_REASON_MOVE);
        break;
    case sdComplectation:
        ui->lePartner->setVisible(false);
        ui->lePartnerName->setVisible(false);
        ui->leInvoiceNumber->setVisible(false);
        ui->deInvoiceDate->setVisible(false);
        ui->lbInvoiceDate->setVisible(false);
        ui->lbInvoiceNumber->setVisible(false);
        ui->lbPartner->setVisible(false);
        ui->btnNewPartner->setVisible(false);
        ui->leReason->setValue(DOC_REASON_COMPLECTATION);
    default:
        break;
    }
    if (sd != DOC_TYPE_COMPLECTATION) {
        ui->tw->removeTab(1);
    }
    setGoodsPanelHidden(C5Config::getRegValue("showhidegoods").toBool());
}

void C5StoreDoc::setLastInputPrices()
{
    C5Database db(fDBParams);
    db[":f_id"] = fInternalId;
    db.exec("select b.f_goods, g.f_lastinputprice "
            "from a_store_draft b "
            "left join c_goods g on g.f_id=b.f_goods "
            "where b.f_document=:f_id");
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        for (int j = 0; j < db.rowCount(); j++) {
            if (ui->tblGoods->getInteger(i, 1) == db.getInt(j, "f_goods")) {
                ui->tblGoods->lineEdit(i, 5)->setDouble(db.getDouble(j, "f_lastinputprice"));
                ui->tblGoods->lineEdit(i, 6)->setDouble(ui->tblGoods->lineEdit(i, 3)->getDouble() * db.getDouble(j, "f_lastinputprice"));
                break;
            }
        }
    }
    countTotal();
}

QToolBar *C5StoreDoc::toolBar()
{
    if (!fToolBar) {
        fToolBar = createStandartToolbar(QList<ToolBarButtons>());
        fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDoc()));
        fToolBar->addAction(QIcon(":/draft.png"), tr("Draft"), this, SLOT(draftDoc()));
        fToolBar->addAction(QIcon(":/new.png"), tr("New\ndocument"), this, SLOT(newDoc()));
        fToolBar->addAction(QIcon(":/recycle.png"), tr("Remove"), this, SLOT(removeDocument()));
        fToolBar->addAction(QIcon(":/print.png"), tr("Print"), this, SLOT(printDoc()));
        fToolBar->addAction(QIcon(":/show_list.png"), tr("Show/Hide\ngoods list"), this, SLOT(showHideGoodsList()));
    }
    return fToolBar;
}

bool C5StoreDoc::removeDoc(const QStringList &dbParams, QString id, bool showmessage)
{
    if (showmessage) {
        if (C5Message::question(tr("Confirm to remove document")) != QDialog::Accepted) {
            return false;
        }
    }
    QString err;
    C5Database db(dbParams);
    db[":f_basedoc"] = id;
    if (!db.exec("select d.f_date, d.f_userid from a_store s "
            "inner join a_header d on d.f_id=s.f_document "
             "where f_basedoc=:f_basedoc and f_document<>:f_basedoc and s.f_type=-1")) {
        err += db.fLastError;
        C5Message::error(err);
        return false;
    }
    while (db.nextRow()) {
        err += QString("No: %1, %2<br>").arg(db.getString(1)).arg(db.getDate(0).toString(FORMAT_DATE_TO_STR));
    }
    db[":f_id"] = id;
    db.exec("select f_raw from a_header where f_id=:f_id");
    QString cashDoc;
    if (db.nextRow()) {
        QJsonDocument jd = QJsonDocument::fromJson(db.getString(0).toUtf8());
        QJsonObject jo = jd.object();
        if (jo["based_on_sale"].toInt() > 0) {
            err += tr("Document based on sale cannot be edited manually");
        }
        cashDoc = jo["f_cashuuid"].toString();
    }
    if (!err.isEmpty()) {
        C5Message::error(err);
        return false;
    }
    db[":f_document"] = id;
    db.exec("delete from a_store where f_document=:f_document");
    db[":f_document"] = id;
    db.exec("delete from a_store_draft where f_document=:f_document");
    db[":f_id"] = id;
    db.exec("delete from a_header where f_id=:f_id");
    if (!cashDoc.isEmpty()) {
        db[":f_header"] = cashDoc;
        db.exec("delete from e_cash where f_header=:f_header");
        db[":f_id"] = cashDoc;
        db.exec("delete from a_header where f_id=:f_id");
    }
    return err.isEmpty();
}

bool C5StoreDoc::saveDraft(C5Database &db, int state, QString &err, bool showMsg)
{
    saveDraft(db, state, err);
    if (showMsg && !err.isEmpty()) {
        err.insert(0, QString("%1 #%2<br>").arg(tr("Document")).arg(ui->leDocNum->text()));
        C5Message::error(err);
        return false;
    }
    if (err.isEmpty()) {
        if (fDocType == DOC_TYPE_STORE_INPUT) {
            db.exec("select * from c_goods_waste");
            QMap<int, double> gw;
            int reason = 0;
            while (db.nextRow()) {
                if (reason == 0) {
                    reason = db.getInt("f_reason");
                }
                gw[db.getInt("f_goods")] = db.getDouble("f_waste");
            }
            QMap<int, double> gq;
            for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
                if (gw.contains(ui->tblGoods->getInteger(i, 1))) {
                    gq[ui->tblGoods->getInteger(i, 1)] = ui->tblGoods->getDouble(i, 3) * (gw[ui->tblGoods->getInteger(i, 1)] / 100);
                }
            }
            if (gq.count() > 0) {
                QJsonObject jo;
                jo["f_storein"] = "";
                jo["f_storeout"] = ui->leStoreOutput->text();

                QJsonDocument jd(jo);
                QString docid = C5Database::uuid();
                db[":f_state"] = DOC_STATE_DRAFT;
                db[":f_type"] = DOC_TYPE_STORE_OUTPUT;
                db[":f_operator"] = __userid;
                db[":f_date"] = ui->deDate->date();
                db[":f_createDate"] = QDate::currentDate();
                db[":f_createTime"] = QTime::currentTime();
                db[":f_partner"] = 0;
                db[":f_amount"] = 0;
                db[":f_comment"] = "AUTO WASTE";
                db[":f_raw"] = jd.toJson();
                db[":f_id"]= docid;
                db[":f_payment"] = ui->lePayment->getInteger();
                db[":f_paid"] = ui->chPaid->isChecked() ? 1 : 0;
                db.insert("a_header", false);
                for (QMap<int, double>::const_iterator it = gq.begin(); it != gq.end(); it++) {
                    db[":f_id"] = C5Database::uuid();
                    db[":f_document"] = docid;
                    db[":f_storein"] = ui->leStoreInput->getInteger();
                    db[":f_storeout"] = ui->leStoreOutput->getInteger();
                    db[":f_goods"] = it.key();
                    db[":f_qty"] = it.value();
                    db[":f_price"] = 0;
                    db[":f_total"] = 0;
                    db[":f_reason"] = reason;
                    db.insert("a_store_draft", false);
                }
                C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
                if (!sd->openDoc(docid)) {
                    __mainWindow->removeTab(sd);
                }
            }
        }
    }
    return err.isEmpty();
}

bool C5StoreDoc::allowChangeDatabase()
{
    return false;
}

bool C5StoreDoc::eventFilter(QObject *o, QEvent *e)
{
    if (o == ui->tblGoodsGroup->viewport() && fGroupTableCellMove) {
        if (e->type() == QEvent::MouseButtonPress) {
            QCursor c;
            QTableWidgetItem *item = ui->tblGoodsGroup->itemAt(ui->tblGoodsGroup->mapFromGlobal(c.pos()));
            if (!item) {
                return false;
            }
            fGroupTableCell = new TableCell(this, item);
            fGroupTableCell->setMinimumSize(ui->tblGoodsGroup->columnWidth(0), ui->tblGoodsGroup->rowHeight(0));
            fGroupTableCell->setMaximumSize(ui->tblGoodsGroup->columnWidth(0), ui->tblGoodsGroup->rowHeight(0));
            fGroupTableCell->move(ui->tblGoodsGroup->mapToGlobal(c.pos()));
            fGroupTableCell->show();
        } else if(e->type() == QEvent::MouseButtonRelease) {
            if (fGroupTableCell) {
                QTableWidgetItem *oldItem = fGroupTableCell->fOldItem;
                delete fGroupTableCell;
                fGroupTableCell = nullptr;
                QTableWidgetItem *item = ui->tblGoodsGroup->itemAt(ui->tblGoodsGroup->mapFromGlobal(QCursor().pos()));
                if (item) {
                    QTableWidgetItem tempItem;
                    tempItem.setText(oldItem->text());
                    tempItem.setData(Qt::UserRole, oldItem->data(Qt::UserRole));
                    tempItem.setData(Qt::UserRole + 1, oldItem->data(Qt::UserRole + 1));
                    oldItem->setText(item->text());
                    oldItem->setData(Qt::UserRole, item->data(Qt::UserRole));
                    oldItem->setData(Qt::UserRole + 1, item->data(Qt::UserRole + 1));
                    item->setText(tempItem.text());
                    item->setData(Qt::UserRole, tempItem.data(Qt::UserRole));
                    item->setData(Qt::UserRole + 1, tempItem.data(Qt::UserRole + 1));
                }
            }
        } else if (e->type() == QEvent::MouseMove) {
            if (fGroupTableCell) {
                fGroupTableCellMove = true;
                fGroupTableCell->move(mapFromGlobal(QCursor().pos()));
                return true;
            }
        }
    }
    return C5Widget::eventFilter(o, e);
}

void C5StoreDoc::nextChild()
{
    if (fCanChangeFocus) {
        focusNextChild();
    } else {
        fCanChangeFocus = true;
    }
}

void C5StoreDoc::setUserID(bool withUpdate, int value)
{
    if (!ui->leDocNum->text().isEmpty()) {
        return;
    }
    C5Database db(fDBParams);
    QString counterField;
    int storeId = 0;
    switch (fDocType) {
    case DOC_TYPE_STORE_INPUT:
        storeId = ui->leStoreInput->getInteger();
        counterField = "f_inputcounter";
        break;
    case DOC_TYPE_STORE_MOVE:
        storeId = ui->leStoreOutput->getInteger();
        counterField = "f_movecounter";
        break;
    case DOC_TYPE_STORE_OUTPUT:
        storeId = ui->leStoreOutput->getInteger();
        counterField = "f_outcounter";
        break;
    case DOC_TYPE_COMPLECTATION:
        storeId = ui->leStoreOutput->getInteger();
        counterField = "f_complectcounter";
        break;
    }
    if (storeId == 0) {
        ui->leDocNum->setPlaceholderText("");
        return;
    }
    db[":f_id"] = storeId;
    db.exec("select " + counterField + " + 1 from c_storages where f_id=:f_id for update");
    db.nextRow();
    int val = db.getInt(0);
    if (value > 0) {
        val = value;
    }
    switch (fDocType) {
    case DOC_TYPE_STORE_INPUT:
        ui->leDocNum->setPlaceholderText(QString("%1").arg(val, C5Config::docNumDigitsInput(), 10, QChar('0')));
        break;
    case DOC_TYPE_STORE_MOVE:
        ui->leDocNum->setPlaceholderText(QString("%1").arg(val, C5Config::docNumDigitsMove(), 10, QChar('0')));
        break;
    case DOC_TYPE_STORE_OUTPUT:
        ui->leDocNum->setPlaceholderText(QString("%1").arg(val, C5Config::docNumDigitsOut(), 10, QChar('0')));
        break;
    case DOC_TYPE_COMPLECTATION:
        ui->leDocNum->setPlaceholderText(QString("%1").arg(val, C5Config::docNumDigitsInput(), 10, QChar('0')));
        break;
    }
    if (withUpdate) {
        db[":f_id"] = storeId;
        db[":val"] = val;
        db.exec("update c_storages set " + counterField + " =:val where f_id=:f_id and " + counterField + "<:val");
    }
}

void C5StoreDoc::countTotal()
{
    double total = 0;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        total += ui->tblGoods->lineEdit(i, 6)->getDouble();
    }
    for (int i = 0; i < ui->tblAdd->rowCount(); i++) {
        total += ui->tblAdd->lineEdit(i, 2)->getDouble();
    }
    ui->leTotal->setDouble(total);
}

void C5StoreDoc::countQtyOfComplectation()
{
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        ui->tblGoods->lineEdit(i, 3)->setDouble(fBaseQtyOfComplectation[ui->tblGoods->getInteger(i, 1)] * ui->leComplectationQty->getDouble());
    }
}

bool C5StoreDoc::docCheck(QString &err)
{
    rowsCheck(err);
    switch (fDocType) {
    case sdInput:
        if (ui->leStoreInput->getInteger() == 0) {
            err += tr("Input store is not defined") + "<br>";
        }
        break;
    case sdOutput:
        if (ui->leStoreOutput->getInteger() == 0) {
            err += tr("Output store is not defined") + "<br>";
        }
        break;
    case sdMovement:
        if (ui->leStoreInput->getInteger() == 0) {
            err += tr("Input store is not defined") + "<br>";
        }
        if (ui->leStoreOutput->getInteger() == 0) {
            err += tr("Output store is not defined") + "<br>";
        }
        if (ui->leStoreInput->getInteger() == ui->leStoreOutput->getInteger() && ui->leStoreInput->getInteger() != 0) {
            err += tr("Input store and output store cannot be same") + "<br>";
        }
        break;
    case sdComplectation:
        if (ui->leStoreInput->getInteger() == 0) {
            err += tr("Input store is not defined") + "<br>";
        }
        if (ui->leStoreOutput->getInteger() == 0) {
            err += tr("Output store is not defined") + "<br>";
        }
        if (ui->leComplectationQty->getDouble() < 0.0001) {
            err += tr("The quantity of complectation cannot be zero") + "<br>";
        }
        break;
    }
    if (ui->tblGoods->rowCount() == 0) {
        err += tr("Cannot save an emtpy document") + "<br>";
    }
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (ui->tblGoods->lineEdit(i, 3)->getDouble() < 0.001) {
            err += QString("%1 #%2, %3, missing quantity").arg(tr("Row")).arg(i + 1).arg(ui->tblGoods->getString(i, 2)) + "<br>";
        }
    }
    return err.isEmpty();
}

void C5StoreDoc::rowsCheck(QString &err)
{
    if (ui->tblGoods->rowCount() == 0) {
        err += tr("Empty document") + "<br>";
        return;
    }
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (ui->tblGoods->lineEdit(i, 3)->getDouble() < 0.0001) {
            err += tr("Row #") + QString::number(i + 1) + tr( " empty qty") + "<br>";
        }
    }
}

bool C5StoreDoc::saveDraft(C5Database &db, int state, QString &err)
{
    if (!docCheck(err)) {
        return false;
    }
    if (ui->leDocNum->isEmpty()) {
        setUserID(true, 0);
        ui->leDocNum->setText(ui->leDocNum->placeholderText());
    } else {
        setUserID(true, ui->leDocNum->getInteger());
    }
    if (DOC_STATE_SAVED == state) {
        if (ui->leReason->getInteger() == 0) {
            err += tr("Reason is not defined")+ "<br>";
        }
    }
    if (!err.isEmpty()) {
        return false;
    }
    if (state == DOC_STATE_SAVED && !ui->wheader->isEnabled()) {
        err += tr("Already saved") + "<br>";
        return false;
    }

    if (state != fDocState) {
        if (state == DOC_STATE_DRAFT && !fInternalId.isEmpty()) {
            db[":f_basedoc"] = fInternalId;
            db.exec("select d.f_date, s.f_document, d.f_userid from a_store s "
                    "inner join a_header d on d.f_id=s.f_document "
                    "where f_basedoc=:f_basedoc and f_document<>:f_basedoc and s.f_type=-1");
            err = "";
            while (db.nextRow()) {
                err += QString("No: %1/%2 %3<br>").arg(db.getInt(1)).arg(db.getString(2)).arg(db.getDate(0).toString(FORMAT_DATE_TO_STR));
            }
            if (!err.isEmpty()) {
                err += tr("This order used in next documents") + "<br>" + err;
                return false;
            }
            switch (fDocType) {
            case DOC_TYPE_STORE_INPUT:
            case DOC_TYPE_STORE_OUTPUT:
            case DOC_TYPE_STORE_MOVE:
            case DOC_TYPE_COMPLECTATION:
                db[":f_document"] = fInternalId;
                db.exec("delete from a_store where f_document=:f_document");
                break;
            }
        }
    }

    QJsonObject jo;
    jo["f_storein"] = ui->leStoreInput->text();
    jo["f_storeout"] = ui->leStoreOutput->text();
    jo["f_passed"] = ui->lePassed->text();
    jo["f_accepted"] = ui->leAccepted->text();
    jo["f_invoicedate"] = ui->deDate->date().toString("dd.MM.yyyy");
    jo["f_invoice"] = ui->leInvoiceNumber->text();
    jo["f_complectation"] = ui->leComplectationCode->text();
    jo["f_complectationqty"] = ui->leComplectationQty->text();
    jo["based_on_sale"] = fBasedOnSale;
    jo["f_cash"] = ui->leCash->getInteger();
    jo["f_cashuuid"] = fCashUuid;
    QJsonDocument jd(jo);
    if (fInternalId.isEmpty()) {
        fInternalId = C5Database::uuid();
        db[":f_id"] = fInternalId;
        db.insert("a_header", false);
    }
    db[":f_userid"] = ui->leDocNum->text();
    db[":f_state"] = state;
    db[":f_type"] = fDocType;
    db[":f_operator"] = __userid;
    db[":f_date"] = ui->deDate->date();
    db[":f_createDate"] = QDate::currentDate();
    db[":f_createTime"] = QTime::currentTime();
    db[":f_partner"] = ui->lePartner->getInteger();
    db[":f_amount"] = ui->leTotal->getDouble();
    db[":f_comment"] = ui->leComment->text();
    db[":f_invoice"] = ui->leInvoiceNumber->text();
    db[":f_raw"] = jd.toJson(QJsonDocument::Compact);
    db[":f_payment"] = ui->lePayment->getInteger();
    db[":f_paid"] = ui->chPaid->isChecked() ? 1 : 0;
    if (!db.update("a_header", where_id(fInternalId))) {
        C5Message::error(db.fLastError);
        return false;
    }

    if (state != fDocState) {
        db[":f_document"] = fInternalId;
        db.exec("delete from a_store_draft where f_document=:f_document");
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            insertDraftRow(db, i);
        }
        if (fDocType == DOC_TYPE_COMPLECTATION) {
            fComplectationId = C5Database::uuid();
            db[":f_id"] = fComplectationId;
            db[":f_document"] = fInternalId;
            db[":f_store"] = ui->leStoreInput->getInteger();
            db[":f_type"] = 1;
            db[":f_goods"] = ui->leComplectationCode->getInteger();
            db[":f_qty"] = ui->leComplectationQty->getDouble();
            db[":f_price"] = ui->leTotal->getDouble() / ui->leComplectationQty->getDouble();
            db[":f_total"] = ui->leTotal->getDouble();
            db[":f_reason"] = ui->leReason->getInteger();
            db.insert("a_store_draft", false);
        }
    }

    QStringList outId;
    if (fDocState == DOC_STATE_DRAFT) {
        if (state == DOC_STATE_SAVED) {
            switch (fDocType) {
            case DOC_TYPE_STORE_INPUT:
                writeInput(db);
                if (!fCashUuid.isEmpty()) {
                    updateCashDoc();
                }
                break;
            case DOC_TYPE_STORE_OUTPUT: {
                double amount = 0;
                if (!writeOutput(db, ui->deDate->date(), fInternalId, ui->leStoreOutput->getInteger(), amount, outId, err)) {
                    return false;
                }
                db[":f_amount"] = amount;
                db.update("a_header", where_id(fInternalId));
                break;
            }
            case DOC_TYPE_STORE_MOVE: {
                double amount = 0;
                if (!writeOutput(db, ui->deDate->date(), fInternalId, ui->leStoreOutput->getInteger(), amount, outId, err)) {
                    return false;
                }
                db[":f_amount"] = amount;
                db.update("a_header", where_id(fInternalId));
                foreach (QString recid, outId) {
                    db[":f_id"] = recid;
                    db.exec("select * from a_store where f_id=:f_id");
                    db.nextRow();
                    db.setBindValues(db.getBindValues());
                    db.removeBindValue(":f_id");
                    db[":f_id"] = C5Database::uuid();
                    db[":f_document"] = fInternalId;
                    db[":f_type"] = 1;
                    db[":f_store"] = ui->leStoreInput->getInteger();
                    db.insert("a_store", false);
                }
                break;
            }
            case DOC_TYPE_COMPLECTATION:
                double amount = 0;
                if (!writeOutput(db, ui->deDate->date(), fInternalId, ui->leStoreOutput->getInteger(), amount, outId, err)) {
                    return false;
                }
                countTotal();
                QString id = C5Database::uuid();
                db[":f_id"] = id;
                db[":f_document"] = fInternalId;
                db[":f_store"] = ui->leStoreInput->getInteger();
                db[":f_type"] = 1;
                db[":f_goods"] = ui->leComplectationCode->getInteger();
                db[":f_qty"] = ui->leComplectationQty->getDouble();
                db[":f_price"] = ui->leTotal->getDouble() / ui->leComplectationQty->getDouble();
                db[":f_total"] = ui->leTotal->getDouble();
                db[":f_base"] = outId.at(0);
                db[":f_basedoc"] = fInternalId;
                db[":f_reason"] = ui->leReason->getInteger();
                db.insert("a_store", false);
                db[":f_amount"] = amount;
                db.update("a_header", where_id(fInternalId));
                break;
            }
        }
    }

    if (state != fDocState) {
        // WRITE TO THE A_STORE_DRAFT
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            if (!updateDraftRow(db, i)) {
                err += tr("Error in updateDraftRow");
                return false;
            }
            if (fDocType == DOC_TYPE_COMPLECTATION) {
                db[":f_qty"] = ui->leComplectationQty->getDouble();
                db[":f_price"] = ui->leTotal->getDouble() / ui->leComplectationQty->getDouble();
                db[":f_total"] = ui->leTotal->getDouble();
                db.update("a_store_draft", where_id(fComplectationId));
            }
        }
    }

    db[":f_header"] = fInternalId;
    db.exec("delete from a_complectation_additions");
    for (int i = 0; i < ui->tblAdd->rowCount(); i++) {
        db[":f_id"] = db.uuid();
        db[":f_header"] = fInternalId;
        db[":f_name"] = ui->tblAdd->lineEdit(i, 1)->text();
        db[":f_amount"] = ui->tblAdd->lineEdit(i, 2)->getDouble();
        db[":f_row"] = i + 1;
        db.insert("a_complectation_additions", false);
    }

    fDocState = state;
    setDocEnabled(fDocState == DOC_STATE_DRAFT);
    countTotal();
    if (fDocState == DOC_STATE_SAVED) {
        if (!fCashUuid.isEmpty()) {
            updateCashDoc();
        }
    }
    return true;
}

bool C5StoreDoc::insertDraftRow(C5Database &db, int row)
{
    QMap<QString, QVariant> bv;
    ui->tblGoods->setString(row, 0, C5Database::uuid());
    bv[":f_id"] = ui->tblGoods->getString(row, 0);
    bv[":f_base"] = ui->tblGoods->getString(row, 0);
    bv[":f_document"] = fInternalId;
    bv[":f_store"] = ui->leStoreInput->getInteger();
    bv[":f_type"] = 1;
    bv[":f_goods"] = ui->tblGoods->getInteger(row, 1);
    bv[":f_qty"] = ui->tblGoods->lineEdit(row, 3)->getDouble();
    bv[":f_price"] = ui->tblGoods->lineEdit(row, 5)->getDouble();
    bv[":f_total"] = ui->tblGoods->lineEdit(row, 6)->getDouble();
    bv[":f_reason"] = ui->leReason->getInteger();
    switch (fDocType) {
    case DOC_TYPE_STORE_INPUT:
        db.fBindValues = bv;
        db.insert("a_store_draft", false);
        db[":f_id"] = ui->tblGoods->getInteger(row, 1);
        db[":f_lastinputprice"] = ui->tblGoods->lineEdit(row, 5)->getDouble();
        db.exec("update c_goods set f_lastinputprice=:f_lastinputprice where f_id=:f_id");
        break;
    case DOC_TYPE_STORE_OUTPUT:
    case DOC_TYPE_COMPLECTATION:
        bv[":f_store"] = ui->leStoreOutput->getInteger();
        bv[":f_type"] = -1;
        db.fBindValues = bv;
        db.insert("a_store_draft", false);
        break;
    case DOC_TYPE_STORE_MOVE:
        db.fBindValues = bv;
        db.insert("a_store_draft", false);
        bv[":f_base"] = bv[":f_id"];
        bv[":f_id"] = C5Database::uuid();
        bv[":f_store"] = ui->leStoreOutput->getInteger();
        bv[":f_type"] = -1;
        db.fBindValues = bv;
        db.insert("a_store_draft", false);
        break;
    }
    return true;
}

bool C5StoreDoc::updateDraftRow(C5Database &db, int row)
{
    QMap<QString, QVariant> bv;
    bv[":f_id"] = ui->tblGoods->getString(row, 0);
    bv[":f_base"] = ui->tblGoods->getString(row, 0);
    bv[":f_price"] = ui->tblGoods->lineEdit(row, 5)->getDouble();
    bv[":f_total"] = ui->tblGoods->lineEdit(row, 6)->getDouble();
    switch (fDocType) {
    case DOC_TYPE_STORE_INPUT:
        db.fBindValues = bv;
        db.exec("update a_store_draft set f_price=:f_price, f_total=:f_total where f_id=:f_id");
        break;
    case DOC_TYPE_STORE_OUTPUT:
    case DOC_TYPE_COMPLECTATION:
        db.fBindValues = bv;
        bv[":f_store"] = ui->leStoreOutput->getInteger();
        bv[":f_type"] = -1;
        db.exec("update a_store_draft set f_price=:f_price, f_total=:f_total where f_id=:f_id");
        break;
    case DOC_TYPE_STORE_MOVE:
        db.fBindValues = bv;
        db.exec("update a_store_draft set f_price=:f_price, f_total=:f_total where f_base=:f_base");
        break;
    }
    return true;
}

void C5StoreDoc::writeInput(C5Database &db)
{
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        QString id = C5Database::uuid();
        db[":f_id"] = id;
        db[":f_document"] = fInternalId;
        db[":f_store"] = ui->leStoreInput->getInteger();
        db[":f_type"] = 1;
        db[":f_goods"] = ui->tblGoods->getInteger(i, 1);
        db[":f_qty"] = ui->tblGoods->lineEdit(i, 3)->getDouble();
        db[":f_price"] = ui->tblGoods->lineEdit(i, 5)->getDouble();
        db[":f_total"] = ui->tblGoods->lineEdit(i, 6)->getDouble();
        db[":f_base"] = id;
        db[":f_basedoc"] = fInternalId;
        db[":f_reason"] = ui->leReason->getInteger();
        db.insert("a_store", false);
    }
}

bool C5StoreDoc::writeOutput(C5Database &db, const QDate &date, QString docNum, int store, double &amount, QStringList &outId, QString &err)
{
    amount = 0;
    C5Database dbdoc(fDBParams);
    QStringList goodsID;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        goodsID << ui->tblGoods->getString(i, 1);
    }
    QList<QList<QVariant> > storeData;
    db[":f_store"] = store;
    db[":f_date"] = date;
    db.exec(QString("select s.f_base, s.f_goods, sum(s.f_qty*s.f_type), s.f_price, sum(s.f_total*s.f_type) "
            "from a_store s "
            "inner join a_header d on d.f_id=s.f_document "
            "where s.f_goods in (%1) and s.f_store=:f_store and d.f_date<=:f_date "
            "group by 1, 2, 4 "
            "having sum(s.f_qty*s.f_type) > 0.001 "
            "for update ").arg(goodsID.join(",")), storeData);
    QList<QMap<QString, QVariant> > queries;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        double qty = ui->tblGoods->lineEdit(i, 3)->getDouble();
        ui->tblGoods->lineEdit(i, 6)->setDouble(0);
        for (int j = 0; j < storeData.count(); j++) {
            if (storeData.at(j).at(1).toInt() == ui->tblGoods->getInteger(i, 1)) {
                if (storeData.at(j).at(2).toDouble() > 0) {
                    if (storeData.at(j).at(2).toDouble() >= qty) {
                        storeData[j][2] = storeData.at(j).at(2).toDouble() - qty;
                        dbdoc[":f_base"] = storeData.at(j).at(0).toString();
                        dbdoc.exec("select f_document from a_store where f_base=:f_base and f_type=1");
                        dbdoc.nextRow();
                        QMap<QString, QVariant> newrec;
                        newrec[":f_document"] = docNum;
                        newrec[":f_store"] = store;
                        newrec[":f_type"] = -1;
                        newrec[":f_goods"] = ui->tblGoods->getInteger(i, 1);
                        newrec[":f_qty"] = qty;
                        newrec[":f_price"] = storeData.at(j).at(3).toDouble();
                        newrec[":f_total"] = storeData.at(j).at(3).toDouble() * qty;
                        newrec[":f_base"] = storeData.at(j).at(0).toString();
                        newrec[":f_basedoc"] = dbdoc.getString(0);
                        newrec[":f_reason"] = ui->leReason->getInteger();
                        newrec[":f_draft"] = ui->tblGoods->getString(i, 0);
                        queries << newrec;
                        amount += storeData.at(j).at(3).toDouble() * qty;
                        ui->tblGoods->lineEdit(i, 6)->setDouble(ui->tblGoods->lineEdit(i, 6)->getDouble() + (storeData.at(j).at(3).toDouble() * qty));
                        ui->tblGoods->lineEdit(i, 5)->setDouble(ui->tblGoods->lineEdit(i, 6)->getDouble() / ui->tblGoods->lineEdit(i, 3)->getDouble());
                        qty = 0;
                    } else {
                        dbdoc[":f_base"] = storeData.at(j).at(0).toString();
                        dbdoc.exec("select f_document from a_store where f_base=:f_base and f_type=1");
                        dbdoc.nextRow();
                        QMap<QString, QVariant> newrec;
                        newrec[":f_document"] = docNum;
                        newrec[":f_store"] = store;
                        newrec[":f_type"] = -1;
                        newrec[":f_goods"] = ui->tblGoods->getInteger(i, 1);
                        newrec[":f_qty"] = storeData.at(j).at(2).toDouble();
                        newrec[":f_price"] = storeData.at(j).at(3).toDouble();
                        newrec[":f_total"] = storeData.at(j).at(3).toDouble() * storeData.at(j).at(2).toDouble();
                        newrec[":f_base"] = storeData.at(j).at(0).toString();
                        newrec[":f_basedoc"] = dbdoc.getString(0);
                        newrec[":f_reason"] = ui->leReason->getInteger();
                        newrec[":f_draft"] = ui->tblGoods->getString(i, 0);
                        queries << newrec;
                        ui->tblGoods->lineEdit(i, 6)->setDouble(ui->tblGoods->lineEdit(i, 6)->getDouble() + (storeData.at(j).at(3).toDouble() * storeData.at(j).at(2).toDouble()));
                        ui->tblGoods->lineEdit(i, 5)->setDouble(ui->tblGoods->lineEdit(i, 6)->getDouble() / ui->tblGoods->lineEdit(i, 3)->getDouble());
                        amount += storeData.at(j).at(3).toDouble() * storeData.at(j).at(2).toDouble();
                        qty -= storeData.at(j).at(2).toDouble();
                        storeData[j][2] = 0.0;
                    }
                }
            }
            if (qty < 0.001) {
                break;
            }
        }
        if (qty > 0.001) {
            if (err.isEmpty()) {
                err += tr("No enaugh materials in the store") + "<br>";
            }
            err += QString("%1 - %2").arg(ui->tblGoods->getString(i, 2)).arg(qty) + "<br>";
        }
    }
    if (err.isEmpty()) {
        for (QList<QMap<QString, QVariant> >::const_iterator it = queries.begin(); it != queries.end(); it++) {
            QString newId = C5Database::uuid();
            outId << newId;
            db.setBindValues(*it);
            db[":f_id"] = newId;
            db.insert("a_store", false);
        }
    }
    return err.isEmpty();
}

int C5StoreDoc::addGoodsRow()
{
    int row = ui->tblGoods->rowCount();
    ui->tblGoods->setRowCount(row + 1);
    ui->tblGoods->setItem(row, 0, new QTableWidgetItem());
    ui->tblGoods->setItem(row, 1, new QTableWidgetItem());
    ui->tblGoods->setItem(row, 2, new QTableWidgetItem());
    C5LineEdit *lqty = ui->tblGoods->createLineEdit(row, 3);
    lqty->setValidator(new QDoubleValidator(0, 1000000, 3));
    ui->tblGoods->setItem(row, 4, new QTableWidgetItem());
    C5LineEdit *lprice = ui->tblGoods->createLineEdit(row, 5);
    lprice->setValidator(new QDoubleValidator(0, 100000000, 3));
    lprice->fDecimalPlaces = 3;
    C5LineEdit *ltotal = ui->tblGoods->createLineEdit(row, 6);
    ltotal->setValidator(new QDoubleValidator(0, 100000000, 2));
    connect(lqty, SIGNAL(textEdited(QString)), this, SLOT(tblQtyChanged(QString)));
    connect(lprice, SIGNAL(textEdited(QString)), this, SLOT(tblPriceChanged(QString)));
    connect(ltotal, SIGNAL(textEdited(QString)), this, SLOT(tblTotalChanged(QString)));
    return row;
}

void C5StoreDoc::setDocEnabled(bool v)
{
    ui->deDate->setEnabled(v);
    ui->leStoreInput->setEnabled(v);
    ui->leStoreOutput->setEnabled(v);
    ui->wtoolbar->setEnabled(v);
    for (int r = 0; r < ui->tblGoods->rowCount(); r++) {
        for (int c = 0; c < ui->tblGoods->columnCount(); c++) {
            C5LineEdit *l = dynamic_cast<C5LineEdit*>(ui->tblGoods->cellWidget(r, c));
            if (l) {
                l->setEnabled(v);
            }
        }
    }
    for (int r = 0; r < ui->tblAdd->rowCount(); r++) {
        for (int c = 1; c < ui->tblAdd->columnCount(); c++) {
            C5LineEdit *l = dynamic_cast<C5LineEdit*>(ui->tblAdd->cellWidget(r, c));
            if (l) {
                l->setEnabled(v);
            }
        }
    }
    if (fToolBar) {
        fToolBar->actions().at(1)->setEnabled(!v);
    }
    ui->grComplectation->setEnabled(v);
}

void C5StoreDoc::loadGroupsInput()
{
    C5Database db(fDBParams);
    db.exec("select f_id, f_name from c_groups order by f_order, f_name");
    int col = -1, row = 0;
    ui->tblGoodsGroup->clearContents();
    ui->tblGoodsGroup->setRowCount(0);
    while (db.nextRow()) {
        col++;
        if (col > ui->tblGoodsGroup->columnCount() - 1) {
            col = 0;
            row = ui->tblGoodsGroup->addEmptyRow();
        }
        if (row > ui->tblGoodsGroup->rowCount() - 1) {
            row = ui->tblGoodsGroup->addEmptyRow();
        }
        QTableWidgetItem *item = new QTableWidgetItem(db.getString("f_name"));
        item->setData(Qt::UserRole, db.getInt(0));
        ui->tblGoodsGroup->setItem(row, col, item);
    }
}

void C5StoreDoc::loadGoodsInput()
{
    ui->tblGoodsStore->setSortingEnabled(false);
    ui->tblGoodsStore->clearContents();
    ui->tblGoodsStore->setRowCount(0);
    C5Database db(fDBParams);
    db.exec(QString("select g.f_id, g.f_group, g.f_name as f_goodsname, u.f_name as f_unitname, "
              "sum(s.f_qty*s.f_type) as f_qty, sum(s.f_total*s.f_type) as f_amount "
              "from c_goods g "
              "left join a_store s on s.f_goods=g.f_id and s.f_store=%1 "
              "left join a_header d on d.f_id=s.f_document and d.f_date<=%2 "
              "inner join c_groups gg on gg.f_id=g.f_group "
              "inner join c_units u on u.f_id=g.f_unit "
              "group by 1, 2, 3, 4 ")
            .arg(ui->leStoreInput->getInteger())
            .arg(ui->deDate->toMySQLDate()));
    while (db.nextRow()) {
        int row = ui->tblGoodsStore->addEmptyRow();
        ui->tblGoodsStore->setInteger(row, 0, db.getInt(0));
        ui->tblGoodsStore->setInteger(row, 1, db.getInt(1));
        ui->tblGoodsStore->setString(row, 2, db.getString(2));
        ui->tblGoodsStore->setDouble(row, 3, db.getDouble(4));
        ui->tblGoodsStore->setString(row, 4, db.getString(3));
        ui->tblGoodsStore->setDouble(row, 6, db.getDouble(5));
        if (ui->tblGoodsStore->getDouble(row, 3) > 0.0001) {
            ui->tblGoodsStore->setDouble(row, 5, ui->tblGoodsStore->getDouble(row, 6) / ui->tblGoodsStore->getDouble(row, 3));
        }
    }
    ui->tblGoodsStore->setSortingEnabled(true);
}

void C5StoreDoc::loadGoodsOutput()
{
    loadGoods(ui->leStoreOutput->getInteger());
}

void C5StoreDoc::loadGoods(int store)
{
    ui->tblGoodsStore->setSortingEnabled(false);
    ui->tblGoodsStore->clearContents();
    ui->tblGoodsStore->setRowCount(0);
    C5Database db(fDBParams);
    db.exec(QString("select s.f_goods, g.f_group, g.f_name as f_goodsname, u.f_name as f_unitname, "
              "sum(s.f_qty*s.f_type) as f_qty, sum(s.f_total*s.f_type) as f_amount "
              "from c_goods g "
              "left join a_store s on s.f_goods=g.f_id "
              "left join a_header d on d.f_id=s.f_document "
              "inner join c_groups gg on gg.f_id=g.f_group "
              "inner join c_units u on u.f_id=g.f_unit "
              "where s.f_store=%1 and d.f_date<=%2 "
              "group by 1, 2, 3, 4 "
              "having sum(s.f_qty*s.f_type) > 0.001")
            .arg(store)
            .arg(ui->deDate->toMySQLDate()));
    while (db.nextRow()) {
        int row = ui->tblGoodsStore->addEmptyRow();
        ui->tblGoodsStore->setInteger(row, 0, db.getInt(0));
        ui->tblGoodsStore->setInteger(row, 1, db.getInt(1));
        ui->tblGoodsStore->setString(row, 2, db.getString(2));
        ui->tblGoodsStore->setDouble(row, 3, db.getDouble(4));
        ui->tblGoodsStore->setString(row, 4, db.getString(3));
        ui->tblGoodsStore->setDouble(row, 6, db.getDouble(5));
        if (ui->tblGoodsStore->getDouble(row, 3) > 0.0001) {
            ui->tblGoodsStore->setDouble(row, 5, ui->tblGoodsStore->getDouble(row, 6) / ui->tblGoodsStore->getDouble(row, 3));
        }
    }
    ui->tblGoodsStore->setSortingEnabled(true);
}

void C5StoreDoc::setGoodsPanelHidden(bool v)
{
    QModelIndexList lst = ui->tblGoodsGroup->selectionModel()->selectedIndexes();
    if (!isVisible()) {
        return;
    }
    C5Config::setRegValue("showhidegoods", v);
    ui->wGoods->setVisible(v);
    if (v) {
        switch (fDocType) {
        case DOC_TYPE_STORE_INPUT:
            loadGroupsInput();
            loadGoodsInput();
            break;
        case DOC_TYPE_STORE_OUTPUT:
        case DOC_TYPE_STORE_MOVE:
        case DOC_TYPE_COMPLECTATION:
            loadGroupsInput();
            loadGoodsOutput();
            break;
        }
        ui->tblGoodsGroup->fitColumnsToWidth(45);
        ui->tblGoodsStore->fitColumnsToWidth(45);
        markGoodsComplited();
    }
    if (lst.count() > 0) {
        ui->tblGoodsGroup->selectionModel()->setCurrentIndex(lst.at(0), QItemSelectionModel::Select);
        on_tblGoodsGroup_itemClicked(ui->tblGoodsGroup->item(lst.at(0).row(), lst.at(0).column()));
    }
}

void C5StoreDoc::markGoodsComplited()
{
    QColor sel = QColor::fromRgb(245, 135, 157);
    QColor blank = QColor::fromRgb(255, 255, 255);
    QSet<int> goods;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        goods << ui->tblGoods->item(i, 1)->data(Qt::EditRole).toInt();
    }
    for (int r = 0; r < ui->tblGoodsStore->rowCount(); r++) {
        QColor color = goods.contains(ui->tblGoodsStore->item(r, 0)->data(Qt::EditRole).toInt()) ? sel : blank;
        for (int c = 0; c < ui->tblGoodsStore->columnCount(); c++) {
            QTableWidgetItem *i = ui->tblGoodsStore->item(r, c);
            if (i) {
                i->setData(Qt::BackgroundRole, color);
            }
        }
    }
}

void C5StoreDoc::updateCashDoc()
{
    C5CashDoc *doc = new C5CashDoc(fDBParams);
    doc->openDoc(fCashUuid);
    doc->setCashOutput(ui->leCash->getInteger());
    doc->setDate(ui->deDate->date());
    doc->setComment(tr("Store input") + " #" + ui->leDocNum->text() + "/" + ui->deDate->text());
    doc->updateRow(0, tr("Store input") + " #" + ui->leDocNum->text() + "/" + ui->deDate->text(), ui->leTotal->getDouble());
    doc->save();
    fCashUuid = doc->uuid();
    delete doc;
}

void C5StoreDoc::newDoc()
{
    fInternalId = "";
    fDocState = DOC_STATE_DRAFT;
    ui->leDocNum->clear();
    ui->lePartner->setValue(0);
    ui->leComment->clear();
    ui->leStoreInput->setValue(0);
    ui->leStoreOutput->setValue(0);
    ui->tblGoods->clearContents();
    ui->tblGoods->setRowCount(0);
    ui->leInvoiceNumber->clear();
    setDocEnabled(true);
    C5Database db(fDBParams);
    db.startTransaction();
    db[":f_id"] = (fDocType);
    db.exec("select f_counter from a_type where f_id=:f_id for update");
    db.nextRow();
    switch (fDocType) {
    case DOC_TYPE_STORE_INPUT:
        ui->leDocNum->setPlaceholderText(QString("%1").arg(db.getInt(0) + 1, C5Config::docNumDigitsInput(), 10, QChar('0')));
        break;
    case DOC_TYPE_STORE_MOVE:
        ui->leDocNum->setPlaceholderText(QString("%1").arg(db.getInt(0) + 1, C5Config::docNumDigitsMove(), 10, QChar('0')));
        break;
    case DOC_TYPE_STORE_OUTPUT:
        ui->leDocNum->setPlaceholderText(QString("%1").arg(db.getInt(0) + 1, C5Config::docNumDigitsOut(), 10, QChar('0')));
        break;
    }
    countTotal();
}

void C5StoreDoc::getInput()
{
    QList<QVariant> vals;
    if (!C5Selector::getValue(fDBParams, cache_goods, vals)) {
        return;
    }
    int row = addGoodsRow();
    ui->tblGoods->setString(row, 1, vals.at(0).toString());
    ui->tblGoods->setString(row, 2, vals.at(2).toString());
    ui->tblGoods->setString(row, 4, vals.at(3).toString());
    ui->tblGoods->lineEdit(row, 3)->setFocus();
}

void C5StoreDoc::getOutput()
{
    QString err;
    if (ui->leStoreOutput->getInteger() == 0) {
        err += tr("Output store must be defined") + "<br>";
    }
    if (!err.isEmpty()) {
        C5Message::error(err);
        return;
    }
    QString query = QString("select s.f_goods, gg.f_name as f_groupname, g.f_name as f_goodsname, u.f_name as f_unitname, "
                          "sum(s.f_qty*s.f_type) as f_qty, sum(s.f_total*s.f_type) as f_amount, g.f_scancode "
                          "from a_store s "
                          "inner join a_header d on d.f_id=s.f_document "
                          "inner join c_goods g on g.f_id=s.f_goods "
                          "inner join c_groups gg on gg.f_id=g.f_group "
                          "inner join c_units u on u.f_id=g.f_unit "
                          "where s.f_store=%1 and d.f_date<=%2 "
                          "group by 1, 2, 3, 4 "
                          "having sum(s.f_qty*s.f_type) > 0.001 ")
            .arg(ui->leStoreOutput->getInteger())
            .arg(ui->deDate->toMySQLDate());
    QList<QVariant> vals;
    QMap<QString, QString> trans;
    trans["f_goods"] = tr("Code");
    trans["f_groupname"] = tr("Group");
    trans["f_goodsname"] = tr("Name");
    trans["f_unitname"] = tr("Unit");
    trans["f_qty"] = tr("Qty");
    trans["f_amount"] = tr("Amount");
    trans["f_scancode"] = tr("Scancode");
    if (!C5Selector::getValues(fDBParams, query, vals, trans)) {
        return;
    }
    int row = addGoodsRow();
    ui->tblGoods->setString(row, 1, vals.at(0).toString());
    ui->tblGoods->setString(row, 2, vals.at(2).toString());
    ui->tblGoods->setString(row, 4, vals.at(3).toString());
    ui->tblGoods->lineEdit(row, 3)->setFocus();
}

void C5StoreDoc::saveDoc()
{
    QString err;
    C5Database db(fDBParams);
    db.startTransaction();
    bool saved = saveDraft(db, DOC_STATE_SAVED, err, true);
    if (saved) {
        db.commit();
    } else {
        db.rollback();
    }
    bool v = C5Config::getRegValue("showhidegoods").toBool();
    if (v) {
        setGoodsPanelHidden(v);
    }
    if (saved) {
        C5Message::info(tr("Saved"));
    }
}

void C5StoreDoc::draftDoc()
{
    QString err;
    C5Database db(fDBParams);
    if (saveDraft(db, DOC_STATE_DRAFT, err, true)) {
        db.commit();
    } else {
        db.rollback();
    }
    bool v = C5Config::getRegValue("showhidegoods").toBool();
    if (v) {
        setGoodsPanelHidden(v);
    }
}

void C5StoreDoc::removeDocument()
{
    if (removeDoc(fDBParams, fInternalId, true)) {
        __mainWindow->removeTab(this);
    }
}

void C5StoreDoc::printDoc()
{
    if (fInternalId == nullptr) {
        C5Message::error(tr("Document is not saved"));
        return;
    }
    C5Printing p;
    QList<qreal> points;
    QStringList vals;
    p.setSceneParams(2000, 2700, QPrinter::Portrait);

    p.setFontSize(25);
    p.setFontBold(true);
    QString docTypeText;
    switch (fDocType) {
    case DOC_TYPE_STORE_INPUT:
        docTypeText = tr("Store input");
        break;
    case DOC_TYPE_STORE_OUTPUT:
        docTypeText = tr("Store output");
        break;
    case DOC_TYPE_STORE_MOVE:
        docTypeText = tr("Store movement");
        break;
    }

    p.ctext(QString("%1 N%2").arg(docTypeText).arg(ui->leDocNum->text()));
    p.br();
    p.ctext(ui->leReasonName->text());
    p.br();
    p.br();
    p.setFontSize(20);
    p.setFontBold(false);
    QString storeInName, storeOutName;
    if (ui->leStoreInput->getInteger() > 0) {
        storeInName = ui->leStoreInputName->text();
    }
    if (ui->leStoreOutput->getInteger() > 0) {
        storeOutName = ui->leStoreOutputName->text();
    }
    if (!ui->leComment->isEmpty()) {
        p.ltext(ui->leComment->text(), 0);
        p.br();
    }
    p.br();
    p.setFontBold(true);
    points.clear();
    points << 50 << 200;
    vals << tr("Date");
    if (!storeInName.isEmpty()) {
        vals << tr("Store, input");
        points << 400;
    }
    if (!storeOutName.isEmpty()) {
        vals << tr("Store, output");
        points << 400;
    }
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.setFontBold(false);
    vals.clear();
    vals << ui->deDate->text();
    if (!storeInName.isEmpty()) {
        vals << ui->leStoreInputName->text();
    }
    if (!storeOutName.isEmpty()) {
        vals << ui->leStoreOutputName->text();
    }
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);

    points.clear();
    vals.clear();
    p.setFontBold(true);
    points << 50;
    if (ui->lePartner->getInteger() > 0) {
        vals << tr("Supplier");
        points << 1000;
    }
    if (!ui->leInvoiceNumber->isEmpty()) {
        vals << tr("Purchase document");
        points << 800;
    }
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);

    p.setFontBold(false);
    points.clear();
    vals.clear();
    points << 50;
    if (ui->lePartner->getInteger() > 0) {
        vals << ui->lePartnerName->text();
        points << 1000;
    }
    if (!ui->leInvoiceNumber->isEmpty()) {
        vals << ui->leInvoiceNumber->text() + " /    " + ui->deInvoiceDate->text();
        points << 800;
    }
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br();
    p.br();
    p.br();

    points.clear();
    points << 50 << 100 << 200 << 600 << 250 << 250 << 250 << 270;
    vals.clear();
    vals << tr("NN")
         << tr("Material code")
         << tr("Goods")
         << tr("Qty")
         << tr("Unit")
         << tr("Price")
         << tr("Total");
    p.setFontBold(true);
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.setFontBold(false);
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (p.checkBr(p.fLineHeight + 20)) {
            p.br(p.fLineHeight + 20);
            p.br();
        }
        vals.clear();
        vals << QString::number(i + 1);
        vals << ui->tblGoods->getString(i, 1);
        vals << ui->tblGoods->getString(i, 2);
        vals << ui->tblGoods->lineEdit(i, 3)->text();
        vals << ui->tblGoods->getString(i, 4);
        vals << ui->tblGoods->lineEdit(i, 5)->text();
        vals << ui->tblGoods->lineEdit(i, 6)->text();
        p.tableText(points, vals, p.fLineHeight + 20);
        if (p.checkBr(p.fLineHeight + 20)) {
            p.br(p.fLineHeight + 20);
        }
        p.br(p.fLineHeight + 20);
    }
    points.clear();
    points << 1200
           << 500
           << 270;
    vals.clear();
    vals << tr("Total amount");
    vals << ui->leTotal->text();
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    if (ui->lePayment->getInteger() > 0) {
        points.clear();
        points << 1200
               << 500
               << 270;
        vals.clear();
        vals << tr("Payment type");
        vals << ui->lePaymentName->text();
        p.tableText(points, vals, p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
        if (ui->chPaid->isChecked()) {
            p.ltext(tr("Paid"), 1200);
        } else {
            p.ltext(tr("Not paid"), 1200);
        }
        p.br();
    }
    p.br(p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    switch (fDocType) {
    case DOC_TYPE_STORE_MOVE:
        p.ltext(tr("Passed"), 50);
        p.ltext(tr("Accepted"), 1000);
        p.br(p.fLineHeight + 20);
        p.ltext(ui->lePassedName->text(), 50);
        p.ltext(ui->leAcceptedName->text(), 1000);
        break;
    default:
        p.ltext(tr("Accepted"), 50);
        p.ltext(tr("Passed"), 1000);
        p.br(p.fLineHeight + 20);
        p.ltext(ui->leAcceptedName->text(), 1000);
        p.ltext(ui->lePassedName->text(), 50);
        break;
    }
    p.line(50, p.fTop, 700, p.fTop);
    p.line(1000, p.fTop, 1650, p.fTop);

    C5PrintPreview pp(&p, fDBParams);
    pp.exec();
}

void C5StoreDoc::checkInvoiceDuplicate()
{
    ui->leInvoiceNumber->setStyleSheet("");
    C5Database db(fDBParams);
    db[":f_invoice"] = ui->leInvoiceNumber->text();
    db[":f_id"] = fInternalId;
    db.exec("select count(f_id) from a_header where f_invoice=:f_invoice and f_id<>:f_id");
    if (db.nextRow()) {
        if (db.getInt(0) > 0) {
            ui->leInvoiceNumber->setStyleSheet("background:red;");
        }
    }
}

void C5StoreDoc::filterGoodsPanel(int group)
{
    for (int i = 0; i < ui->tblGoodsStore->rowCount(); i++) {
        bool h = (group != 0) && (group != ui->tblGoodsStore->getInteger(i, 1));
        ui->tblGoodsStore->setRowHidden(i, h);
    }
}

void C5StoreDoc::showHideGoodsList()
{
    bool v = !ui->wGoods->isVisible();
    setGoodsPanelHidden(v);
}

void C5StoreDoc::changeGoodsGroupOrder()
{
    fGroupTableCellMove = true;
}

void C5StoreDoc::saveGoodsGroupOrder()
{
    C5Database db(fDBParams);
    for (int r = 0; r < ui->tblGoodsGroup->rowCount(); r++) {
        for (int c = 0; c < ui->tblGoodsGroup->columnCount(); c++) {
            QTableWidgetItem *i = ui->tblGoodsGroup->item(r, c);
            if (i == nullptr) {
                continue;
            }
            db[":f_order"] = (c + (r * ui->tblGoodsGroup->columnCount()));
            db.update("c_groups", where_id(i->data(Qt::UserRole).toInt()));
        }
    }
    if (fGroupTableCell) {
        delete fGroupTableCell;
        fGroupTableCell = nullptr;
    }
    fGroupTableCellMove = false;
}

void C5StoreDoc::cancelGoodsGroupOrder()
{
    fGroupTableCellMove = false;
    if (fGroupTableCell) {
        delete fGroupTableCell;
        fGroupTableCell = nullptr;
    }
    loadGroupsInput();
}

void C5StoreDoc::tblQtyChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;
    if (!ui->tblGoods->findWidget(static_cast<QWidget*>(sender()), row, col)) {
        return;
    }
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col + 2);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col + 3);
    ltotal->setDouble(lqty->getDouble() * lprice->getDouble());
    countTotal();
}

void C5StoreDoc::tblPriceChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;
    if (!ui->tblGoods->findWidget(static_cast<QWidget*>(sender()), row, col)) {
        return;
    }
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col - 2);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col + 1);
    ltotal->setDouble(lqty->getDouble() * lprice->getDouble());
    countTotal();
}

void C5StoreDoc::tblTotalChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;
    if (!ui->tblGoods->findWidget(static_cast<QWidget*>(sender()), row, col)) {
        return;
    }
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col - 3);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col - 1);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col);
    if (lqty->getDouble() > 0.001) {
        lprice->setDouble(ltotal->getDouble() / lqty->getDouble());
    }
    countTotal();
}

void C5StoreDoc::tblAddChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    //countTotal();
}

void C5StoreDoc::on_btnAddGoods_clicked()
{
    switch (fDocType) {
    case DOC_TYPE_STORE_INPUT:
        getInput();
        break;
    case DOC_TYPE_STORE_OUTPUT:
    case DOC_TYPE_STORE_MOVE:
    case DOC_TYPE_COMPLECTATION:
        getOutput();
        break;
    }
    markGoodsComplited();
}

void C5StoreDoc::on_btnRemoveGoods_clicked()
{
    int row = ui->tblGoods->currentRow();
    if (row < 0) {
        return;
    }
    if (C5Message::question(tr("Confirm to remove") + "<br>" + ui->tblGoods->item(row, 2)->text()) != QDialog::Accepted) {
        return;
    }
    ui->tblGoods->removeRow(row);
    countTotal();
}

void C5StoreDoc::on_leStoreInput_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    bool v = C5Config::getRegValue("showhidegoods").toBool();
    if (v) {
        setGoodsPanelHidden(v);
    }
    setUserID(false, ui->leDocNum->getInteger());
}

void C5StoreDoc::on_leStoreOutput_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    bool v = C5Config::getRegValue("showhidegoods").toBool();
    if (v) {
        setGoodsPanelHidden(v);
    }
    setUserID(false, ui->leDocNum->getInteger());
}

void C5StoreDoc::on_tblGoodsGroup_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    filterGoodsPanel(item->data(Qt::UserRole).toInt());
}

void C5StoreDoc::on_tblGoodsStore_itemDoubleClicked(QTableWidgetItem *item)
{
    if (fDocState == DOC_STATE_SAVED) {
        C5Message::error(tr("Document is saved, unable to make changes"));
        return;
    }
    int r = item->row();
    if (r < 0) {
        return;
    }
    int row = addGoodsRow();
    ui->tblGoods->setInteger(row, 1, ui->tblGoodsStore->getInteger(r, 0));
    ui->tblGoods->setString(row, 2, ui->tblGoodsStore->getString(r, 2));
    ui->tblGoods->setString(row, 4, ui->tblGoodsStore->getString(r, 4));
    ui->tblGoods->lineEdit(row, 3)->setFocus();
    markGoodsComplited();
}

void C5StoreDoc::on_btnNewPartner_clicked()
{
    CE5Partner *ep = new CE5Partner(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        ui->lePartner->setValue(data.at(0)["f_id"].toString());
    }
    delete e;
}

void C5StoreDoc::on_btnNewGoods_clicked()
{
    CE5Goods *ep = new CE5Goods(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        int row = addGoodsRow();
        ui->tblGoods->setData(row, 1, data.at(0)["f_id"]);
        ui->tblGoods->setData(row, 2, data.at(0)["f_name"]);
        ui->tblGoods->setData(row, 4, data.at(0)["f_unitname"]);
        ui->tblGoods->lineEdit(row, 3)->setFocus();
    }
    delete e;
}

void C5StoreDoc::on_leScancode_returnPressed()
{
    C5Database db(C5Config::dbParams());
    db[":f_scancode"] = ui->leScancode->text();
    db.exec("select gg.f_scancode, gg.f_id, gg.f_name, gu.f_name as f_unitname, gg.f_saleprice, "
            "gr.f_taxdept, gr.f_adgcode "
            "from c_goods gg  "
            "left join c_groups gr on gr.f_id=gg.f_group "
            "left join c_units gu on gu.f_id=gg.f_unit "
            "where gg.f_scancode=:f_scancode");
    ui->leScancode->clear();
    if (db.nextRow()) {
        int row = addGoodsRow();
        ui->tblGoods->setInteger(row, 1, db.getInt("f_id"));
        ui->tblGoods->setString(row, 2, db.getString("f_name"));
        ui->tblGoods->setString(row, 4, db.getString("f_unitname"));
        ui->tblGoods->item(row, 2)->setSelected(true);
        fCanChangeFocus = false;
        ui->tblGoods->lineEdit(row, 3)->setFocus();
        markGoodsComplited();
    } else {
        C5Message::error(tr("Goods not found"));
        fCanChangeFocus = false;
        ui->leScancode->setFocus();
    }
}

TableCell::TableCell(QWidget *parent, QTableWidgetItem *item) :
    QLabel(parent, Qt::FramelessWindowHint)
{
    setText(item->text());
    fOldItem = item;
}

void C5StoreDoc::on_tblGoodsGroup_customContextMenuRequested(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);
    if (fGroupTableCellMove) {
        menu->addAction(tr("Save order"), this, SLOT(saveGoodsGroupOrder()));
        menu->addAction(tr("Cancel changes"), this, SLOT(cancelGoodsGroupOrder()));
    } else {
        menu->addAction(tr("Change order"), this, SLOT(changeGoodsGroupOrder()));
    }
    menu->popup(ui->tblGoodsGroup->mapToGlobal(pos));
}

void C5StoreDoc::on_btnCreateDoc_clicked()
{
    if (fDocState != DOC_STATE_SAVED) {
        C5Message::info(tr("Save first"));
        return;
    }
    if (fCashUuid.isEmpty()) {
        C5CashDoc *doc = new C5CashDoc(fDBParams);
        doc->setRelation(true);
        doc->setCashOutput(ui->leCash->getInteger());
        doc->setDate(ui->deDate->date());
        doc->setComment(tr("Store input") + " #" + ui->leDocNum->text() + "/" + ui->deDate->text());
        doc->addRow(tr("Store input") + " #" + ui->leDocNum->text() + "/" + ui->deDate->text(), ui->leTotal->getDouble());
        doc->save();
        fCashUuid = doc->uuid();
        delete doc;
    }
    saveDoc();
}

void C5StoreDoc::on_leComplectationName_textChanged(const QString &arg1)
{
    ui->tblGoods->clearContents();
    ui->tblGoods->setRowCount(0);
    fBaseQtyOfComplectation.clear();
    if (arg1.isEmpty()) {
        countTotal();
        return;
    }
    C5Database db(fDBParams);
    db[":f_base"] = ui->leComplectationCode->getInteger();
    db.exec("select c.f_id, c.f_goods, g.f_name as f_goodsname, c.f_qty, u.f_name as f_unitname, "
            "c.f_price, c.f_qty*c.f_price as f_total "
            "from c_goods_complectation c "
            "left join c_goods g on g.f_id=c.f_goods "
            "left join c_units u on u.f_id=g.f_unit "
            "where c.f_base=:f_base");
    while (db.nextRow()) {
        int row = addGoodsRow();
        fBaseQtyOfComplectation[db.getInt("f_goods")] = db.getDouble("f_qty");
        ui->tblGoods->setInteger(row, 0, db.getInt("f_id"));
        ui->tblGoods->setInteger(row, 1, db.getInt("f_goods"));
        ui->tblGoods->setString(row, 2, db.getString("f_goodsname"));
        ui->tblGoods->lineEdit(row, 3)->setDouble(db.getDouble("f_qty"));
        ui->tblGoods->setString(row, 4, db.getString("f_unitname"));
        ui->tblGoods->lineEdit(row, 5)->setDouble(db.getDouble("f_price"));
        ui->tblGoods->lineEdit(row, 6)->setDouble(db.getDouble("f_total"));
    }
    countQtyOfComplectation();
    countTotal();
    markGoodsComplited();
}

void C5StoreDoc::on_leComplectationQty_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    countQtyOfComplectation();
}

void C5StoreDoc::on_chPaid_clicked(bool checked)
{
    ui->lbCashDoc->setEnabled(checked);
    ui->leCash->setEnabled(checked);
    ui->leCashName->setEnabled(checked);
    ui->deCashDate->setEnabled(checked);
}

void C5StoreDoc::on_btnAddAdd_clicked()
{
    int row = ui->tblAdd->rowCount();
    ui->tblAdd->setRowCount(row + 1);
    C5LineEdit *l = ui->tblAdd->createLineEdit(row, 1);
    l->setFocus();
    l = ui->tblAdd->createLineEdit(row, 2);
    l->setValidator(new QDoubleValidator());
    connect(l, SIGNAL(textEdited(QString)), this, SLOT(tblAddChanged(QString)));
}

void C5StoreDoc::on_btnRemoveAdd_clicked()
{
    int row = ui->tblAdd->currentRow();
    if (row < 0) {
        return;
    }
    if (C5Message::question(tr("Confirm to remove") + "<br>" + ui->tblAdd->lineEdit(row, 2)->text()) != QDialog::Accepted) {
        return;
    }
    ui->tblAdd->removeRow(row);
    countTotal();
}
