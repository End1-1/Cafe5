#include "c5storedoc.h"
#include "ui_c5storedoc.h"
#include "c5selector.h"
#include "c5cache.h"
#include "c5printing.h"
#include "c5cashdoc.h"
#include "c5printpreview.h"
#include "c5editor.h"
#include "c5mainwindow.h"
#include "c5daterange.h"
#include "ce5partner.h"
#include "dlgdirtystoredoc.h"
#include "ce5goods.h"
#include "c5storebarcode.h"
#include "c5user.h"
#include "c5storedraftwriter.h"
#include "calculator.h"
#include "datadriver.h"
#include "chatmessage.h"
#include "c5storedocselectprinttemplate.h"
#include "threadsendmessage.h"
#include "bclientdebts.h"
#include "xlsxall.h"
#include <QMenu>
#include <QHash>
#include <QClipboard>
#include <QInputDialog>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QShortcut>

C5StoreDoc::C5StoreDoc(const QStringList &dbParams, QWidget *parent) :
    C5Document(dbParams, parent),
    ui(new Ui::C5StoreDoc)
{
    ui->setupUi(this);
    fIcon = ":/goods.png";
    fLabel = tr("Store document");
    ui->leCash->setSelector(fDBParams, ui->leCashName, cache_cash_names);
    ui->leStoreInput->setSelector(fDBParams, ui->leStoreInputName, cache_goods_store);
    ui->leStoreOutput->setSelector(fDBParams, ui->leStoreOutputName, cache_goods_store);
    ui->leStoreInput->addEventKeys("+-");
    ui->leStoreOutput->addEventKeys("+-");
    ui->lbComplectUnit->setText("");
    ui->wSearchInDocs->setVisible(false);
    connect(ui->leStoreInput, SIGNAL(keyPressed(QChar)), this, SLOT(lineEditKeyPressed(QChar)));
    connect(ui->leStoreOutput, SIGNAL(keyPressed(QChar)), this, SLOT(lineEditKeyPressed(QChar)));
    ui->leReason->setSelector(fDBParams, ui->leReasonName, cache_store_reason);
    ui->lePartner->setSelector(fDBParams, ui->lePartnerName, cache_goods_partners);
    ui->leComplectationCode->setSelector(fDBParams, ui->leComplectationName, cache_goods, 1, 3);
    disconnect(ui->leComplectationName, SIGNAL(textChanged(QString)), this, SLOT(on_leComplectationName_textChanged(QString)));
    connect(ui->leComplectationName, SIGNAL(textChanged(QString)), this, SLOT(on_leComplectationName_textChanged(QString)));
#ifdef QT_DEBUG
    ui->tblDishes->setColumnWidths(ui->tblDishes->columnCount(), 0, 0, 400, 80, 400);
#else
    ui->tblDishes->setColumnWidths(ui->tblDishes->columnCount(), 0, 0, 400, 80, 0);
#endif
    ui->tblGoods->setColumnWidths(ui->tblGoods->columnCount(), 0, 0, 0, 0, 400, 80, 80, 80, 80, 80, 300);
    ui->tblGoodsStore->setColumnWidths(7, 0, 0, 200, 70, 50, 50, 70);
    ui->btnNewPartner->setVisible(__user->check(cp_t7_partners));
    ui->btnNewGoods->setVisible(__user->check(cp_t6_goods));
    ui->leAccepted->setSelector(dbParams, ui->leAcceptedName, cache_users);
    ui->lePassed->setSelector(dbParams, ui->lePassedName, cache_users);
    ui->lbCashDoc->setVisible(!C5Config::noCashDocStore());
    ui->leCash->setVisible(!C5Config::noCashDocStore());
    ui->leCashName->setVisible(!C5Config::noCashDocStore());
    ui->deCashDate->setVisible(!C5Config::noCashDocStore());
    ui->chPaid->setVisible(!C5Config::noCashDocStore());
    ui->leScancode->setVisible(!C5Config::noScanCodeStore());
    ui->btnNewGoods->setVisible(__user->check(cp_t6_goods));
    ui->btnEditGoods->setVisible(__user->check(cp_t6_goods));
    fInternalId = "";
    fDocState = DOC_STATE_DRAFT;
    ui->tblGoodsGroup->viewport()->installEventFilter(this);
    fGroupTableCell = nullptr;
    fGroupTableCellMove = false;
    fBasedOnSale = 0;
    connect(ui->leInvoiceNumber, SIGNAL(focusOut()), this, SLOT(checkInvoiceDuplicate()));
    QShortcut *f2 = new QShortcut(QKeySequence(Qt::Key_F2), this);
    QShortcut *fplus = new QShortcut(QKeySequence(Qt::Key_Plus), this);
    QShortcut *fminus = new QShortcut(QKeySequence(Qt::Key_Minus), this);
    connect(fplus, &QShortcut::activated, [this]() {
        on_btnAddGoods_clicked();
    });
    connect(fminus, &QShortcut::activated, [this]() {
        on_btnRemoveGoods_clicked();
    });
    connect(f2, &QShortcut::activated, [this](){
        ui->leScancode->setFocus();
    });
    fFocusNextChild = false;
    fCanChangeFocus = true;
    ui->tblAdd->setColumnWidths(ui->tblAdd->columnCount(), 0, 300, 80);
    ui->leComplectationQty->setValidator(new QDoubleValidator(0, 999999999, 3));
    ui->deDate->setEnabled(__user->check(cp_t1_allow_change_store_doc_date));
    ui->btnRememberStoreIn->setChecked(__c5config.getRegValue("storedoc_storeinput").toBool());
    if (__c5config.getRegValue("storedoc_storeinput").toBool()) {
        ui->leStoreInput->setValue(__c5config.getRegValue("storedoc_storeinput_id").toInt());
    }

    ui->tblCalcPrice->setColumnWidth(0, 300);
    adjustSize();
    C5Database db(dbParams);
    db.open();
    db.exec("select * from e_currency order by f_id");
    while (db.nextRow()) {
        ui->cbCurrency->addItem(db.getString("f_name"), db.getInt("f_id"));
    }
    ui->cbCurrency->setCurrentIndex(ui->cbCurrency->findData(__c5config.getValue(param_default_currency).toInt()));
    ui->leReasonPartner->setSelector(fDBParams, ui->leReasonPartnerName, cache_goods_partners);
    setReasonPartnerName();
}

C5StoreDoc::~C5StoreDoc()
{
    __mainWindow->removeBroadcastListener(this);
    delete ui;
}

bool C5StoreDoc::openDoc(QString id, QString &err)
{
    ui->leDocNum->setPlaceholderText("");
    C5Database db(fDBParams);
    C5StoreDraftWriter dw(db);
    if (!dw.readAHeader(id)) {
        err = dw.fErrorMsg;
        return false;
    }

    fInternalId = id;
    ui->leDocNum->setText(dw.value(container_aheader, 0, "f_userid").toString());
    ui->deDate->setDate(dw.value(container_aheader, 0, "f_date").toDate());
    fDocType = dw.value(container_aheader, 0, "f_type").toInt();
    fDocState = dw.value(container_aheader, 0, "f_state").toInt();
    ui->lePartner->setValue(dw.value(container_aheader, 0, "f_partner").toInt());
    if (dw.rowCount(container_bclient_debts) > 0) {
        ui->leReasonPartner->setValue(dw.value(container_bclient_debts, 0, "f_costumer").toInt());
    }
    ui->leComment->setText(dw.value(container_aheader, 0, "f_comment").toString());
    ui->leTotal->setDouble(dw.value(container_aheader, 0, "f_amount").toDouble());
    ui->cbCurrency->setCurrentIndex(ui->cbCurrency->findData(dw.value(container_aheader, 0, "f_currency").toInt()));
    setMode(static_cast<STORE_DOC>(fDocType));
    for (int i = 0; i < dw.rowCount(container_astoredishwaste); i++) {
        int row = ui->tblDishes->addEmptyRow();
        ui->tblDishes->createLineEdit(row, 3);
        ui->tblDishes->setString(row, 0, dw.value(container_astoredishwaste, i, "f_id").toString());
        ui->tblDishes->setString(row, 1, dw.value(container_astoredishwaste, i, "f_dish").toString());
        ui->tblDishes->setString(row, 2, dw.value(container_astoredishwaste, i, "f_dishname").toString());
        ui->tblDishes->lineEdit(row, 3)->setDouble(dw.value(container_astoredishwaste, i, "f_qty").toDouble());
        ui->tblDishes->setString(row, 4, dw.value(container_astoredishwaste, i, "f_data").toString());
        connect(ui->tblDishes->lineEdit(row, 3), SIGNAL(textChanged(QString)), this, SLOT(tblDishQtyChanged(QString)));
    }
    for (int i = 0; i < dw.rowCount(container_astoredraft); i++) {
        int row = -1;
        if (dw.value(container_astoredraft, i, "f_type").toInt() == 1) {
            if (dw.value(container_aheader, 0, "f_type").toInt() == DOC_TYPE_COMPLECTATION) {
                continue;
            }
            for (int j = 0; j < ui->tblGoods->rowCount(); j++) {
                if (dw.value(container_astoredraft, i, "f_id") == ui->tblGoods->getString(j, 2)) {
                    row = j;
                    break;
                }
            }
        } else {
            for (int j = 0; j < ui->tblGoods->rowCount(); j++) {
                if (dw.value(container_astoredraft, i, "f_base").toString().isEmpty()) {
                    break;
                }
                if (dw.value(container_astoredraft, i, "f_base") == ui->tblGoods->getString(j, 0)) {
                    row = j;
                    break;
                }
            }
        }
        if (row < 0) {
            row = addGoodsRow();
        }
        //if (ui->leReason->getInteger() == 0) {
            ui->leReason->setValue(dw.value(container_astoredraft, i, "f_reason").toString());
        //}
        if (dw.value(container_astoredraft, i, "f_type").toInt() == 1) {
            ui->tblGoods->setString(row, 0, dw.value(container_astoredraft, i, "f_id").toString());
        } else {
            ui->tblGoods->setString(row, 1, dw.value(container_astoredraft, i, "f_id").toString());
            ui->tblGoods->setString(row, 2, dw.value(container_astoredraft, i, "f_base").toString());
        }
        ui->tblGoods->setInteger(row, 3, dw.value(container_astoredraft, i, "f_goods").toInt());
        ui->tblGoods->setString(row, 4, dw.value(container_astoredraft, i, "f_goodsname").toString());
        ui->tblGoods->lineEdit(row, 5)->setDouble(dw.value(container_astoredraft, i, "f_qty").toDouble());
        ui->tblGoods->setString(row, 6, dw.value(container_astoredraft, i, "f_unitname").toString());
        ui->tblGoods->lineEdit(row, 7)->setDouble(dw.value(container_astoredraft, i, "f_price").toDouble());
        ui->tblGoods->lineEdit(row, 8)->setDouble(dw.value(container_astoredraft, i, "f_total").toDouble());
        ui->tblGoods->getWidget<C5DateEdit>(row, 9)->setDate(dw.value(container_astoredraft, i, "f_validdate").toDate());
        ui->tblGoods->lineEdit(row, 10)->setText(dw.value(container_astoredraft, i, "f_comment").toString());
    }

    disconnect(ui->leComplectationName, SIGNAL(textChanged(QString)), this, SLOT(on_leComplectationName_textChanged(QString)));
    disconnect(ui->leComplectationQty, SIGNAL(textChanged(QString)), this, SLOT(on_leComplectationQty_textChanged(QString)));
    ui->leStoreInput->setValue(dw.value(container_aheaderstore, 0, "f_storein").toString());
    ui->leStoreOutput->setValue(dw.value(container_aheaderstore, 0, "f_storeout").toString());
    fBasedOnSale = dw.value(container_aheaderstore, 0, "f_baseonsale").toInt();
    ui->lePassed->setValue(dw.value(container_aheaderstore, 0, "f_userpass").toString());
    ui->leAccepted->setValue(dw.value(container_aheaderstore, 0, "f_useraccept").toString());
    ui->leComplectationCode->setValue(dw.value(container_aheaderstore, 0, "f_complectation").toString());
    ui->leComplectationQty->setDouble(dw.value(container_aheaderstore, 0, "f_complectationqty").toDouble());
    ui->deInvoiceDate->setDate(dw.value(container_aheaderstore, 0, "f_invoicedate").toDate());
    ui->leInvoiceNumber->setText(dw.value(container_aheaderstore, 0, "f_invoice").toString());
    fCashDocUuid = dw.value(container_aheaderstore, 0, "f_cashuuid").toString();
    ui->chPaid->setChecked(!fCashDocUuid.isEmpty());
    if (!fCashDocUuid.isEmpty()) {
        on_chPaid_clicked(true);
        C5CashDoc cd(fDBParams);
        cd.openDoc(fCashDocUuid);
        ui->deCashDate->setDate(cd.date());
        ui->leCash->setValue(cd.outputCash());
    } else {
        on_chPaid_clicked(false);
    }

    connect(ui->leComplectationName, SIGNAL(textChanged(QString)), this, SLOT(on_leComplectationName_textChanged(QString)));
    connect(ui->leComplectationQty, SIGNAL(textChanged(QString)), this, SLOT(on_leComplectationQty_textChanged(QString)));

    if (fDocType == DOC_TYPE_COMPLECTATION) {
        db[":f_document"] = id;
        db[":f_type"] = 1;
        db.exec("select d.f_id, d.f_goods, concat(g.f_name, if(g.f_scancode is null, '', concat(' ', g.f_scancode))) as f_goodsname, \
                d.f_qty, u.f_name, d.f_price, d.f_total, d.f_reason \
                from a_store_draft d \
                left join c_goods g on g.f_id=d.f_goods \
                left join c_units u on u.f_id=g.f_unit \
                where d.f_document=:f_document and f_type=:f_type");
        if (db.nextRow()) {
            fComplectationId = db.getString("f_id");
            ui->leComplectationCode->setValue(db.getInt("f_goods"));
            ui->leComplectationQty->setDouble(db.getDouble("f_qty"));
        } else {
            err = tr("Error in complectation document");
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

    for (int i = 0; i < dw.rowCount(container_acalcprice); i++) {
        int row = -1;
        for (int s = 0; s < ui->tblGoods->rowCount(); s++) {
            if (ui->tblGoods->getString(s, 0) == dw.value(container_acalcprice, i, "f_id").toString()) {
                row = s;
                break;
            }
        }
        if (row < 0) {
            continue;
        }
        qDebug() << i << row << dw.value(container_astoredraft, row, "f_goodsname").toString();
        ui->tblCalcPrice->setString(row, 0, dw.value(container_astoredraft, row, "f_goodsname").toString());
        ui->tblCalcPrice->setDouble(row, 1, dw.value(container_astoredraft, row, "f_qty").toDouble());
        ui->tblCalcPrice->setString(row, 2, dw.value(container_astoredraft, row, "f_unitname").toString());
        ui->tblCalcPrice->setDouble(row, 3, dw.value(container_astoredraft, row, "f_price").toDouble());
        ui->tblCalcPrice->setDouble(row, 4, dw.value(container_acalcprice, i, "f_price2").toDouble());
        ui->tblCalcPrice->lineEdit(row, 6)->setDouble(dw.value(container_acalcprice, i, "f_margin").toDouble());
        calcPrice2(row);
    }
    calcTotalSale();

    if (fToolBar && fDocType == sdOutput) {
        fToolBar->addAction(QIcon(":/storeinput.png"), tr("Create store input"), this, SLOT(createStoreInput()));
    }

    return true;
}

void C5StoreDoc::correctDebt()
{
    double amount = ui->leTotal->getDouble() * -1;
    C5Database db(fDBParams);
    C5StoreDraftWriter dw(db);

    if (fDocState == DOC_STATE_SAVED) {
        if (ui->lePartner->getInteger() > 0){
            dw.updateField("b_clients_debts", "f_amount", amount, "f_storedoc", fInternalId);
        }
    }
}

void C5StoreDoc::setMode(C5StoreDoc::STORE_DOC sd)
{
    ui->chPaid->setVisible(false);
    ui->grComplectation->setVisible(false);
    ui->chWholeGroup->setVisible(false);
    fDocType = sd;
    bool paymentVisible = !C5Config::noCashDocStore();
    switch (fDocType) {
    case DOC_TYPE_STORE_INPUT:
        ui->chPaid->setVisible(paymentVisible && true);
        if (fToolBar) {
            fToolBar->addAction(QIcon(":/goods_store.png"), tr("Input of service"), this, SLOT(inputOfService()));
            fToolBar->addAction(QIcon(":/goods_store.png"), tr("Output of service"), this, SLOT(outputOfService()));
            fToolBar->addAction(QIcon(":/constructor.png"), tr("Dirty edit"), this, SLOT(dirtyEdit()));
        }
        break;
    case DOC_TYPE_STORE_MOVE:
        ui->lbCashDoc->setVisible(false);
        ui->leCashName->setVisible(false);
        ui->leCash->setVisible(false);
        ui->deCashDate->setVisible(false);
        ui->chWholeGroup->setVisible(true);
        break;
    case DOC_TYPE_STORE_OUTPUT:
        ui->lbCashDoc->setVisible(false);
        ui->leCashName->setVisible(false);
        ui->leCash->setVisible(false);
        ui->deCashDate->setVisible(false);
        ui->chWholeGroup->setVisible(true);
        break;
    case DOC_TYPE_COMPLECTATION:
        ui->grComplectation->setVisible(true);
        break;
    case DOC_TYPE_DECOMPLECTATION:
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
        ui->btnRememberStoreIn->setVisible(false);
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
    case sdDeComplectation:
        ui->lePartner->setVisible(false);
        ui->lePartnerName->setVisible(false);
        ui->leInvoiceNumber->setVisible(false);
        ui->deInvoiceDate->setVisible(false);
        ui->lbInvoiceDate->setVisible(false);
        ui->lbInvoiceNumber->setVisible(false);
        ui->lbPartner->setVisible(false);
        ui->btnNewPartner->setVisible(false);
        ui->leReason->setValue(DOC_REASON_DECOMPLECTATION);
        break;
    default:
        break;
    }
    if (sd != DOC_TYPE_COMPLECTATION && sd != DOC_TYPE_STORE_INPUT) {
        ui->tw->removeTab(1);
    }
    //ui->twCalcPrice->setVisible(sd == DOC_TYPE_STORE_INPUT);
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
            if (ui->tblGoods->getInteger(i, 3) == db.getInt(j, "f_goods")) {
                ui->tblGoods->lineEdit(i, 7)->setDouble(db.getDouble(j, "f_lastinputprice"));
                ui->tblGoods->lineEdit(i, 8)->setDouble(ui->tblGoods->lineEdit(i, 5)->getDouble() * db.getDouble(j, "f_lastinputprice"));
                break;
            }
        }
    }
    countTotal();
}

void C5StoreDoc::setStore(int store1, int store2)
{
    ui->leStoreInput->setValue(store1);
    ui->leStoreOutput->setValue(store2);
}

void C5StoreDoc::setReason(int reason)
{
    ui->leReason->setValue(reason);
}

void C5StoreDoc::setComment(const QString comment)
{
    ui->leComment->setText(comment);
}

void C5StoreDoc::setFlag(const QString &name, const QVariant &value)
{
    fFlags[name] = value;
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
        fToolBar->addAction(QIcon(":/excel.png"), tr("Export\nto Excel"),  this, SLOT(exportToExcel()));
        fToolBar->addAction(QIcon(":/show_list.png"), tr("Show/Hide\ngoods list"), this, SLOT(showHideGoodsList()));
        fToolBar->addAction(QIcon(":/barcode.png"), tr("Print\nbarcode"), this, SLOT(printBarcode()));
        fToolBar->addAction(QIcon(":/goods_store.png"), tr("Duplicate as output"), this, SLOT(duplicateOutput()));
        fToolBar->addAction(QIcon(":/goods_store.png"), tr("Duplicate as input"), this, SLOT(duplicateAsInput()));
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
    C5StoreDraftWriter dw(db);
    if (!dw.removeStoreDocument(db, id, err)){
        if (showmessage) {
            C5Message::error(err);
        }
        return false;
    }
    return true;
}

QVariant C5StoreDoc::docProperty(const QString &field) const
{
    if (field == "date") {
        return ui->deDate->date();
    } else if (field == "uuid") {
        return fInternalId;
    } else if (field == "docnumber") {
        if (ui->leDocNum->text().isEmpty()) {
            return ui->leDocNum->placeholderText();
        } else {
            return ui->leDocNum->text();
        }
    } else if (field == "inputstorename") {
        return ui->leStoreInputName->text();
    } else if (field == "outputstorename") {
        return ui->leStoreOutputName->text();
    } else if (field == "typename") {
        int row = C5Cache::cache(fDBParams, cache_doc_type)->find(fDocType);
        return C5Cache::cache(fDBParams, cache_doc_type)->getString(row, 1);
//        DbStoreDocType dt(fDocType);
//        return dt.typeName();
    } else if (field == "typeid") {
        return fDocType;
    } else {
        Q_ASSERT(field == "unknown");
    }
    return "";
}

bool C5StoreDoc::allowChangeDatabase()
{
    return false;
}

void C5StoreDoc::addByScancode(const QString &code, const QString &qty, QString price)
{
    C5Database db(fDBParams);
    if (ui->chWholeGroup->isChecked()) {
        db[":f_scancode"] = code;
        db.exec("select f_group from c_goods where f_scancode=:f_scancode");
        if (!db.nextRow()) {
            C5Message::error(tr("Scancode doesnt exists"));
            return;
        }
        db[":f_store"] = ui->leStoreOutput->getInteger();
        db[":f_date"] = ui->deDate->date();
        db[":f_group"] = db.getInt("f_group");
        db.exec("select g.f_id as f_code,ss.f_name as f_storage,gg.f_name as f_group, "
                "concat(g.f_name, ' ', coalesce(g.f_scancode, '')) as f_goods,sum(s.f_qty*s.f_type) as f_qty, "
                "u.f_name as f_unitname "
                "from a_store s left join c_goods g on g.f_id=s.f_goods "
                "inner join c_storages ss on ss.f_id=s.f_store "
                "inner join c_groups gg on gg.f_id=g.f_group "
                "left join c_units u on u.f_id=g.f_unit "
                "inner join a_header h on h.f_id=s.f_document  "
                "where h.f_date<=:f_date and h.f_state=1 and gg.f_id=:f_group and ss.f_id=:f_store "
                "group by g.f_id,ss.f_name,gg.f_name,g.f_name,u.f_name "
                "having sum(s.f_qty*s.f_type) <> 0 ");
        while (db.nextRow()) {
            int row = addGoodsRow();
            ui->tblGoods->setInteger(row, 3, db.getInt("f_code"));
            ui->tblGoods->setString(row, 4, db.getString("f_goods"));
            ui->tblGoods->lineEdit(row, 5)->setDouble(db.getDouble("f_qty"));
            ui->tblGoods->setString(row, 6, db.getString("f_unitname"));
            ui->tblGoods->item(row, 4)->setSelected(true);
            fCanChangeFocus = false;
        }
        markGoodsComplited();
        return;
    }

    db[":f_scancode"] = code;
    db.exec("select gg.f_scancode, gg.f_id, concat(gg.f_name, ' ', gg.f_scancode) as f_name, gu.f_name as f_unitname, gg.f_saleprice, "
            "gr.f_taxdept, gr.f_adgcode, gg.f_lastinputprice "
            "from c_goods gg  "
            "left join c_groups gr on gr.f_id=gg.f_group "
            "left join c_units gu on gu.f_id=gg.f_unit "
            "where gg.f_scancode=:f_scancode");
    ui->leScancode->clear();
    if (db.nextRow()) {
        int row = addGoodsRow();
        ui->tblGoods->setInteger(row, 3, db.getInt("f_id"));
        ui->tblGoods->setString(row, 4, db.getString("f_name"));
        ui->tblGoods->setString(row, 6, db.getString("f_unitname"));
        ui->tblGoods->item(row, 4)->setSelected(true);
        fCanChangeFocus = false;
        ui->tblGoods->lineEdit(row, 5)->setText(qty);
        ui->tblGoods->lineEdit(row, 5)->setFocus();
        ui->tblGoods->lineEdit(row, 7)->setText(price);
        ui->tblGoods->lineEdit(row, 7)->setPlaceholderText(float_str(db.getDouble("f_lastinputprice"), 2));
        if (__c5config.getValue(param_input_doc_fix_price).toInt() > 0) {
            if (price.toDouble() < 0.001) {
                if (db.getDouble("f_lastinputprice") > 0.001) {
                    ui->tblGoods->lineEdit(row, 7)->setDouble(db.getDouble("f_lastinputprice"));
                    price = db.getString("f_lastinputprice");
                }
            }
        }
        emit ui->tblGoods->lineEdit(row, 7)->textEdited(price);
        markGoodsComplited();
    } else {
        db[":f_scancode"] = code;
        db.exec("select gg.f_scancode, gg.f_id, concat(gg.f_name, ' ', gg.f_scancode) as f_name, gu.f_name as f_unitname, gg.f_saleprice, "
                "gr.f_taxdept, gr.f_adgcode, gg.f_lastinputprice "
                "from c_goods gg  "
                "left join c_groups gr on gr.f_id=gg.f_group "
                "left join c_units gu on gu.f_id=gg.f_unit "
                "where gg.f_id in (select f_goods from c_goods_multiscancode where f_id=:f_scancode)");
        if (db.nextRow()) {
            int row = addGoodsRow();
            ui->tblGoods->setInteger(row, 3, db.getInt("f_id"));
            ui->tblGoods->setString(row, 4, db.getString("f_name"));
            ui->tblGoods->setString(row, 6, db.getString("f_unitname"));
            ui->tblGoods->item(row, 4)->setSelected(true);
            fCanChangeFocus = false;
            ui->tblGoods->lineEdit(row, 5)->setText(qty);
            ui->tblGoods->lineEdit(row, 5)->setFocus();
            ui->tblGoods->lineEdit(row, 7)->setText(price);
            ui->tblGoods->lineEdit(row, 7)->setPlaceholderText(float_str(db.getDouble("f_lastinputprice"), 2));
            if (__c5config.getValue(param_input_doc_fix_price).toInt() > 0) {
                if (price.toDouble() < 0.001) {
                    if (db.getDouble("f_lastinputprice") > 0.001) {
                        ui->tblGoods->lineEdit(row, 7)->setDouble(db.getDouble("f_lastinputprice"));
                        price = db.getString("f_lastinputprice");
                    }
                }
            }
            emit ui->tblGoods->lineEdit(row, 7)->textEdited(price);
            markGoodsComplited();
        } else {
            C5Message::error(tr("Goods not found"));
            fCanChangeFocus = false;
            ui->leScancode->setFocus();
        }
    }
}

double C5StoreDoc::total()
{
    return ui->leTotal->getDouble();
}

void C5StoreDoc::hotKey(const QString &key)
{
    if (key.toLower() == "ctrl+f") {
        ui->wSearchInDocs->setVisible(true);
    } else {
        C5Document::hotKey(key);
    }
}

bool C5StoreDoc::openDraft(const QString &id)
{
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select * from o_draft_sale where f_id=:f_id");
    db.nextRow();
    ui->deDate->setDate(db.getDate("f_date"));
    ui->leReason->setValue(DOC_REASON_SALE_RETURN);
    ui->lePartner->setValue(db.getInt("f_partner"));
    ui->leComment->setText(tr("Back from") + " " + ui->lePartnerName->text());

    db[":f_header"] = id;
    db.exec("select b.f_goods, g.f_name as f_goodsname, b.f_qty, u.f_name as f_unitname, b.f_store "
            "from o_draft_sale_body b "
            "left join c_goods g on g.f_id=b.f_goods "
            "left join c_units u on u.f_id=g.f_unit "
            "where b.f_header=:f_header");
    while (db.nextRow()) {
        ui->leStoreInput->setValue(db.getInt("f_store"));
        int row = addGoods(db.getInt("f_goods"), db.getString("f_goodsname"), db.getDouble("f_qty"), db.getString("f_unitname"), 0, 0, "");
    }
    return true;
}

void C5StoreDoc::setReasonPartnerName()
{
    qDebug() << ui->leReason->getInteger();
    ui->lbReasonPartner->setVisible(ui->leReason->getInteger() == 11);
    ui->leReasonPartner->setVisible(ui->leReason->getInteger() == 11);
    ui->leReasonPartnerName->setVisible(ui->leReason->getInteger() == 11);
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
       // focusNextChild();
    } else {
        fCanChangeFocus = true;
    }
}

bool C5StoreDoc::writeDocument(int state, QString &err)
{
    if (!docCheck(err)) {
        return false;
    }
    C5Database db(fDBParams);
    C5StoreDraftWriter dw(db);
    db.startTransaction();

    dw.updateField("a_store_draft", "f_reason", ui->leReason->getInteger(), "f_document", fInternalId);
    dw.updateField("a_store", "f_reason", ui->leReason->getInteger(), "f_document", fInternalId);
    if (state == fDocState && state == DOC_STATE_SAVED) {
        dw.writeAHeaderPartial(fInternalId, ui->leDocNum->text(), __user->id(), QDate::currentDate(), QTime::currentTime(),
                                 ui->lePartner->getInteger(),ui->leComment->text());
        dw.updateField("a_header", "f_currency", ui->cbCurrency->currentData(), "f_id", fInternalId);
        if (ui->chPaid->isChecked()) {

        } else {

        }
        dw.updateField("b_clients_debts", "f_currency",ui->cbCurrency->currentData(), "f_storedoc", fInternalId);
        db.commit();
        return true;
    }

    if (ui->leDocNum->isEmpty() || (!ui->leDocNum->text().isEmpty() && ui->leDocNum->text().toInt() < 1)) {
        setUserId(true, 0);
        ui->leDocNum->setText(ui->leDocNum->placeholderText());
    } else {
        setUserId(true, ui->leDocNum->getInteger());
    }
    if (state == DOC_STATE_DRAFT) {
        if (!fInternalId.isEmpty()) {
            if (dw.haveRelations(fInternalId, err, true)) {
                return false;
            } else {

            }
        }
        db[":f_storedoc"] = fInternalId;
        db.exec("delete from b_clients_debts where f_storedoc=:f_storedoc");
    }

    dw.clearAStoreDraft(fInternalId);
    if (!dw.writeAHeader(fInternalId, ui->leDocNum->text(), fDocState, fDocType, __user->id(),
                         ui->deDate->date(), QDate::currentDate(), QTime::currentTime(),
                         ui->lePartner->getInteger(), ui->leTotal->getDouble(), ui->leComment->text(),
                         (int) ui->chPaid->isChecked(), ui->cbCurrency->currentData().toInt())) {
        err += db.fLastError + "<br>";
    }
    dw.updateField("a_header", "f_currency", ui->cbCurrency->currentData(), "f_id", fInternalId);
    if (!dw.writeAHeaderStore(fInternalId, ui->leAccepted->getInteger(), ui->lePassed->getInteger(), ui->leInvoiceNumber->text(), ui->deInvoiceDate->date(),
                              ui->leStoreInput->getInteger(), ui->leStoreOutput->getInteger(), fBasedOnSale, fCashDocUuid, ui->leComplectationCode->getInteger(),
                              ui->leComplectationQty->getDouble(), "")) {
        err += db.fLastError + "<br>";
    }
    //TODO: complectation store document
    if (fDocType == DOC_TYPE_COMPLECTATION) {
        if (!dw.writeAStoreDraft(fComplectationId, fInternalId, ui->leStoreInput->getInteger(), 1, ui->leComplectationCode->getInteger(),
                                 ui->leComplectationQty->getDouble(), ui->leTotal->getDouble() / ui->leComplectationQty->getDouble(),
                                 ui->leTotal->getDouble(), ui->leReason->getInteger(), "", 1, "")) {
            err += db.fLastError + "<br>";
        }
    }
    db[":f_header"] = fInternalId;
    db.exec("delete from a_complectation_additions where f_header=:f_header");
    for (int i = 0; i < ui->tblAdd->rowCount(); i++) {
        db[":f_id"] = db.uuid();
        db[":f_header"] = fInternalId;
        db[":f_name"] = ui->tblAdd->lineEdit(i, 1)->text();
        db[":f_amount"] = ui->tblAdd->lineEdit(i, 2)->getDouble();
        db[":f_row"] = i + 1;
        db.insert("a_complectation_additions", false);
    }
    for (int i = 0; i < ui->tblDishes->rowCount(); i++) {
        QString id = ui->tblDishes->getString(i, 0);
        if (!dw.writeAStoreDishWaste(id, fInternalId, ui->tblDishes->getInteger(i, 1), ui->tblDishes->lineEdit(i, 3)->getDouble(), ui->tblDishes->getString(i, 4))) {
            err += db.fLastError + "<br>";
        }
        ui->tblDishes->setString(i, 0, id);
    }
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        QString inid = ui->tblGoods->getString(i, 0);
        QString outid = ui->tblGoods->getString(i, 1);
        if (ui->leStoreInput->getInteger() > 0) {
            if (fDocType == DOC_TYPE_STORE_INPUT
                    || fDocType == DOC_TYPE_STORE_MOVE || fDocType == DOC_TYPE_DECOMPLECTATION) {
                if (!dw.writeAStoreDraft(inid, fInternalId, ui->leStoreInput->getInteger(), 1, ui->tblGoods->getInteger(i, 3),
                                         ui->tblGoods->lineEdit(i, 5)->getDouble(), ui->tblGoods->lineEdit(i, 7)->getDouble(), ui->tblGoods->lineEdit(i, 8)->getDouble(),
                                         ui->leReason->getInteger(), "", i, ui->tblGoods->lineEdit(i, 10)->text())) {
                    err += db.fLastError + "<br>";
                }
                if (fDocType == DOC_TYPE_STORE_INPUT) {
                    db[":f_id"] = ui->tblGoods->getInteger(i, 3);
                    db[":f_lastinputprice"] = ui->tblGoods->lineEdit(i, 7)->getDouble();
                    db.exec("update c_goods set f_lastinputprice=:f_lastinputprice where f_id=:f_id");
                }
                ui->tblGoods->setString(i, 0, inid);
            }
        }
        if (ui->leStoreOutput->getInteger() > 0) {
            if (!dw.writeAStoreDraft(outid, fInternalId, ui->leStoreOutput->getInteger(), -1, ui->tblGoods->getInteger(i, 3),
                                     ui->tblGoods->lineEdit(i, 5)->getDouble(), ui->tblGoods->lineEdit(i, 7)->getDouble(), ui->tblGoods->lineEdit(i, 8)->getDouble(),
                                     ui->leReason->getInteger(), inid, i, ui->tblGoods->lineEdit(i, 9)->text())){
                err += db.fLastError + "<br>";
            }
            ui->tblGoods->setString(i, 1, outid);
            ui->tblGoods->setString(i, 2, inid);
        }
    }

    if (fDocType == DOC_TYPE_STORE_INPUT) {
        db.deleteFromTable("a_calc_price", "f_document", fInternalId);
        for (int i = 0; i < ui->tblCalcPrice->rowCount(); i++) {
            db[":f_id"] = ui->tblGoods->getString(i, 0);
            db[":f_document"] = fInternalId;
            db[":f_price2"] = ui->tblCalcPrice->getDouble(i, 4);
            db[":f_margin"] = ui->tblCalcPrice->lineEdit(i, 6)->getDouble();
            db.insert("a_calc_price", false);
        }
        db[":f_document"] = fInternalId;
        db.exec("delete from a_store_valid where f_document=:f_document");
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            db[":f_id"] = ui->tblGoods->getString(i, 0);
            db[":f_document"] = fInternalId;
            db[":f_date"] = ui->tblGoods->getWidget<C5DateEdit>(i, 9)->date();
            db.insert("a_store_valid", false);
        }
    }

    if (!err.isEmpty()) {
        db.rollback();
        return false;
    }

    if (state == DOC_STATE_SAVED) {
        switch (fDocType) {
        case DOC_TYPE_STORE_INPUT:
            dw.writeInput(fInternalId, err);
            break;
        case DOC_TYPE_STORE_OUTPUT:
            dw.writeOutput(fInternalId, err);
            break;
        case DOC_TYPE_STORE_MOVE:
            dw.writeOutput(fInternalId, err);
            break;
        case DOC_TYPE_COMPLECTATION:
            dw.writeOutput(fInternalId, err);
            break;
        case DOC_TYPE_DECOMPLECTATION:
            dw.writeOutput(fInternalId, err);
            break;
        }
    }
    if (!err.isEmpty()) {
        db.rollback();
        return false;
    }

//    if (fDocType == DOC_TYPE_STORE_INPUT) {
//        if (DOC_STATE_DRAFT == state) {
//            db[":f_id"] = fInternalId;
//            db.exec("delete from a_header_shop2partner where f_id=:f_id");
//            db[":f_document"] = fInternalId;
//            db.exec("delete from a_header_shop2partneraccept where f_id in (select f_id from a_store_draft where f_document=:f_document)");
//        } else {
//            db[":f_id"] = fInternalId;
//            db.exec("select * from a_header_shop2partner where f_id=:f_id");
//            if (db.nextRow() == false) {
//                db[":f_id"] = fInternalId;
//                db[":f_accept"] = 0;
//                db[":f_store"] = ui->leStoreInput->getInteger();
//                db.insert("a_header_shop2partner");
//            }
//        }
//    }

    db[":f_document"] = fInternalId;
    db.exec("select f_id, f_price, f_total, f_type from a_store_draft where f_document=:f_document ");
    QList<OGoods> gl;
    while (db.nextRow()) {
        OGoods g;
        g.storeRec = db.getString("f_id");
        g.price = db.getDouble("f_price");
        g.total = db.getDouble("f_total");
        gl.append(g);
    }
    foreach (const OGoods &g, gl) {
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            if (ui->tblGoods->getString(i, 0) == g.storeRec || ui->tblGoods->getString(i, 1) == g.storeRec) {
                ui->tblGoods->lineEdit(i, 7)->setDouble(g.price);
                ui->tblGoods->lineEdit(i, 8)->setDouble(g.total);
                continue;
            }
        }
    }

    countTotal();
    if (!dw.writeAHeader(fInternalId, ui->leDocNum->text(), state, fDocType, __user->id(),
                         ui->deDate->date(), QDate::currentDate(), QTime::currentTime(),
                         ui->lePartner->getInteger(), ui->leTotal->getDouble(), ui->leComment->text(),
                         (int) ui->chPaid->isChecked(),ui->cbCurrency->currentData().toInt())) {
        err += db.fLastError + "<br>";
    }
    if (fDocType == DOC_TYPE_COMPLECTATION) {
        db[":f_id"] = ui->leComplectationCode->getInteger();
        db[":f_lastinputprice"] = ui->leTotal->getDouble() / ui->leComplectationQty->getDouble();
        db.exec("update c_goods set f_lastinputprice=:f_lastinputprice where f_id=:f_id");
    }

    fDocState = state;
    db.commit();

    /*write cash doc */
    if (fDocType == DOC_TYPE_STORE_INPUT) {
        db[":f_id"] = fCashDocUuid;
        db.exec("select * from a_header where f_id=:f_id");
        if (db.nextRow()) {
            C5CashDoc::removeDoc(fDBParams, fCashDocUuid);
        }
        fCashDocUuid.clear();
        if (ui->chPaid->isChecked() && state == DOC_STATE_SAVED) {
            C5CashDoc cd(fDBParams);
            QString purpose = tr("Purchase of goods");
            cd.addRow(purpose, ui->leTotal->getDouble());
            cd.setDate(ui->deCashDate->date());
            cd.setCashOutput(ui->leCash->getInteger());
            cd.setComment(purpose);
            cd.setPartner(ui->lePartner->getInteger());
            cd.save(false);
            fCashDocUuid = cd.uuid();
            db[":f_cashuuid"] = fCashDocUuid;
            db.update("a_header_store", "f_id", fInternalId);
        }
        db[":f_storedoc"] = fInternalId;
        db.exec("delete from b_clients_debts where f_storedoc=:f_storedoc");
        if (state == DOC_STATE_SAVED) {
            if (ui->leReason->getInteger() == DOC_REASON_BACK_FROM_PARTNER) {
                db[":f_id"] = ui->leStoreInput->getInteger();
                db.exec("select f_hall from c_storages where f_id=:f_id");
                db.nextRow();
                int flag = db.getInt("f_hall");
                if (ui->leReasonPartner->getInteger() > 0) {
                    BClientDebts bcd;
                    bcd.date = ui->deDate->date();
                    bcd.source = BCLIENTDEBTS_SOURCE_SALE;
                    bcd.amount = ui->leTotal->getDouble();
                    bcd.currency = ui->cbCurrency->currentData().toInt();
                    bcd.store = fInternalId;
                    bcd.costumer = ui->leReasonPartner->getInteger();
                    bcd.flag = flag;
                    bcd.write(db, err);
                }
            } else {
                if (ui->lePartner->getInteger() > 0 && !ui->chPaid->isChecked()) {
                    BClientDebts bcd;
                    bcd.date = ui->deDate->date();
                    bcd.source = BCLIENTDEBTS_SOURCE_INPUT;
                    bcd.amount = ui->leTotal->getDouble() * -1;
                    bcd.currency = ui->cbCurrency->currentData().toInt();
                    bcd.store = fInternalId;
                    bcd.costumer = ui->lePartner->getInteger();
                    bcd.write(db, err);
                }
            }
        }
    }

    if (ui->tblAdd->rowCount() > 0){
        db[":f_name"] = "last_store_additions";
        db.exec("select * from s_draft where f_name=:f_name");
        if (db.nextRow()) {
            db[":f_name"] = "last_store_additions";
            db[":f_value"] = fInternalId;
            db.exec("update s_draft set f_value=:f_value where f_name=:f_name");
        } else {
            db[":f_name"] = "last_store_additions";
            db[":f_value"] = fInternalId;
            db.insert("s_draft", false);
        }
    }
    setDocEnabled(state == DOC_STATE_DRAFT);
    bool v = C5Config::getRegValue("showhidegoods").toBool();
    if (v) {
        setGoodsPanelHidden(v);
    }
    return true;
}

void C5StoreDoc::setUserId(bool withUpdate, int value)
{
    if (!ui->leDocNum->text().isEmpty() && ui->leDocNum->text().toInt() > 0) {
        return;
    }
    int storeId = 0;
    switch (fDocType) {
    case DOC_TYPE_STORE_INPUT:
        storeId = ui->leStoreInput->getInteger();
        break;
    case DOC_TYPE_STORE_MOVE:
        storeId = ui->leStoreOutput->getInteger();
        break;
    case DOC_TYPE_STORE_OUTPUT:
        storeId = ui->leStoreOutput->getInteger();
        break;
    case DOC_TYPE_COMPLECTATION:
        storeId = ui->leStoreOutput->getInteger();
        break;
    }
    if (storeId == 0) {
        ui->leDocNum->setPlaceholderText("");
        return;
    }

    C5Database db(fDBParams);
    C5StoreDraftWriter dw(db);
    ui->leDocNum->setPlaceholderText(dw.storeDocNum(fDocType, storeId, withUpdate, value));
}

void C5StoreDoc::correctDishesRows(int row, int count)
{
    for (int i = row; i < ui->tblDishes->rowCount(); i++) {
        QJsonArray ja = QJsonDocument::fromJson(ui->tblDishes->getString(i, 4).toUtf8()).array();
        for (int j = 0; j < ja.count(); j++) {
            QJsonObject jo = ja.at(j).toObject();
            jo["f_row"] = jo["f_row"].toInt() - count;
            ja[j] = jo;
        }
        ui->tblDishes->setString(i, 4, QJsonDocument(ja).toJson());
    }
}

void C5StoreDoc::countTotal()
{
    double total = 0, totalQty = 0.0;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        total += ui->tblGoods->lineEdit(i, 8)->getDouble();
        totalQty += ui->tblGoods->lineEdit(i, 5)->getDouble();
    }
    for (int i = 0; i < ui->tblAdd->rowCount(); i++) {
        total += ui->tblAdd->lineEdit(i, 2)->getDouble();
    }
    ui->leTotal->setDouble(total);
    ui->leTotalQty->setDouble(totalQty);
}

void C5StoreDoc::countQtyOfComplectation()
{
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        ui->tblGoods->lineEdit(i, 5)->setDouble((fBaseQtyOfComplectation[i] * ui->leComplectationQty->getDouble()) / fBaseComplectOutput);
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
        if (ui->chPaid->isChecked() && ui->leCash->getInteger() == 0) {
            err += tr("Cash must be selected") + "<br>";
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
        if (ui->leComplectationQty->getDouble() < 0.00001) {
            err += tr("The quantity of complectation cannot be zero") + "<br>";
        }
        if (ui->leComplectationCode->getInteger() == 0) {
            err += tr("The goods of input cannot be unknown") + "<br>";
        }
        break;
    }
    if (ui->tblGoods->rowCount() == 0) {
        err += tr("Cannot save an emtpy document") + "<br>";
    }
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (ui->tblGoods->lineEdit(i, 5)->getDouble() < 0.00001) {
            err += QString("%1 #%2, %3, %4").arg(tr("Row")).arg(i + 1).arg(ui->tblGoods->getString(i, 4)).arg(tr("missing quantity")) + "<br>";
        }
    }
    if (ui->leReason->getInteger() == 0) {
        err += tr("The reason of document cannot be empty");
    }
    if (ui->chPaid->isChecked()) {
        if (ui->lePartner->getInteger() == 0) {
            err += tr("Supplier not specified") + "<br>";
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
        if (ui->tblGoods->lineEdit(i, 5)->getDouble() < 0.0001) {
            err += tr("Row #") + QString::number(i + 1) + tr( " empty qty") + "<br>";
        }
    }
}

void C5StoreDoc::writeDocumentWithState(int state)
{
    QString err;
    writeDocument(state, err);
    if (err.isEmpty()) {
        writeAStoreSale(ui->leStoreInput->getInteger(), ui->leStoreOutput->getInteger());
        C5Message::info(tr("Saved"));
        if (fFlags.contains("outputservice")) {
            C5Database db(fDBParams);
            for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
                QString goodsid = ui->tblGoods->lineEdit(i, 9)->text().right(36);
                db[":f_storerec"] = ui->tblGoods->getString(i, 1);
                db.update("o_goods", "f_id", goodsid);
            }
        }
    } else {
        C5Message::error(err);
    }
}

int C5StoreDoc::addGoodsRow()
{
    int row = ui->tblGoods->rowCount();
    ui->tblGoods->setRowCount(row + 1);
    ui->tblGoods->setItem(row, 0, new QTableWidgetItem());
    ui->tblGoods->setItem(row, 1, new QTableWidgetItem());
    ui->tblGoods->setItem(row, 2, new QTableWidgetItem());
    ui->tblGoods->setItem(row, 3, new QTableWidgetItem());
    ui->tblGoods->setItem(row, 4, new QTableWidgetItem());
    C5LineEdit *lqty = ui->tblGoods->createLineEdit(row, 5);
    lqty->setValidator(new QDoubleValidator(0, 1000000, 4));
    lqty->fDecimalPlaces = 4;
    lqty->addEventKeys("+-*");
    connect(lqty, SIGNAL(keyPressed(QChar)), this, SLOT(lineEditKeyPressed(QChar)));
    connect(lqty, SIGNAL(returnPressed()), this, SLOT(focusNextChildren()));
    ui->tblGoods->setItem(row, 6, new QTableWidgetItem());
    C5LineEdit *lprice = ui->tblGoods->createLineEdit(row, 7);
    lprice->setValidator(new QDoubleValidator(0, 100000000, 3));
    lprice->fDecimalPlaces = 3;
    lprice->addEventKeys("+-*");
    connect(lprice, SIGNAL(keyPressed(QChar)), this, SLOT(lineEditKeyPressed(QChar)));
    connect(lprice, SIGNAL(returnPressed()), this, SLOT(focusNextChildren()));
    C5LineEdit *ltotal = ui->tblGoods->createLineEdit(row, 8);
    ltotal->setValidator(new QDoubleValidator(0, 100000000, 2));
    ltotal->addEventKeys("+-*");
    connect(ltotal, SIGNAL(keyPressed(QChar)), this, SLOT(lineEditKeyPressed(QChar)));
    connect(ltotal, SIGNAL(returnPressed()), this, SLOT(focusNextChildren()));
    ui->tblGoods->createWidget<C5DateEdit>(row, 9);
    ui->tblGoods->createLineEdit(row, 10);

    connect(lqty, SIGNAL(textEdited(QString)), this, SLOT(tblQtyChanged(QString)));
    connect(lprice, SIGNAL(textEdited(QString)), this, SLOT(tblPriceChanged(QString)));
    connect(ltotal, SIGNAL(textEdited(QString)), this, SLOT(tblTotalChanged(QString)));

    ui->tblCalcPrice->addEmptyRow();
    C5LineEdit *l = ui->tblCalcPrice->createLineEdit(row, 5);
    connect(l, &C5LineEdit::textEdited, this, &C5StoreDoc::tblCalcMarginPercentChanged);
    l = ui->tblCalcPrice->createLineEdit(row, 6);
    connect(l, &C5LineEdit::textEdited, this, &C5StoreDoc::tblCalcMarginChanged);
    l = ui->tblCalcPrice->createLineEdit(row, 7);
    connect(l, &C5LineEdit::textEdited, this, &C5StoreDoc::tblCalcFinalChanged);



    return row;
}

int C5StoreDoc::addGoods(int goods, const QString &name, double qty, const QString &unit, double price, double total, const QString &comment)
{
    int row = addGoodsRow();
    ui->tblGoods->setData(row, 3, goods);
    ui->tblGoods->setData(row, 4, name);
    ui->tblGoods->setData(row, 6, unit);
    if (qty > 0) {
        ui->tblGoods->lineEdit(row, 5)->setDouble(qty);
    }
    if (price > 0) {
        ui->tblGoods->lineEdit(row, 7)->setDouble(price);
        ui->tblGoods->lineEdit(row, 8)->setDouble(total);
    }
    ui->tblGoods->lineEdit(row, 9)->setText(comment);
    ui->tblGoods->setCurrentCell(row, 0);

    ui->tblCalcPrice->setString(row, 0, name);
    if (qty > 0) {
        ui->tblCalcPrice->setDouble(row, 1, qty);
    }
    ui->tblCalcPrice->setString(row, 2, unit);
    if (price > 0) {
        ui->tblCalcPrice->setDouble(row, 3, price);
    }
    return row;
}

void C5StoreDoc::setDocEnabled(bool v)
{
    ui->deDate->setEnabled(v && __user->check(cp_t1_allow_change_store_doc_date));
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
        //fToolBar->actions().at(1)->setEnabled(!v);
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
    db.exec(QString("select g.f_id, g.f_group, concat(g.f_name, if (g.f_scancode is null, '', concat(' ', g.f_scancode))) as f_goodsname, "
                    "u.f_name as f_unitname, "
                    "sum(s.f_qty*s.f_type) as f_qty, sum(s.f_total*s.f_type) as f_amount "
                    "from c_goods g "
                    "left join a_store s on s.f_goods=g.f_id and s.f_store=%1 "
                    "left join a_header d on d.f_id=s.f_document and d.f_date<=%2 and d.f_state=1 "
                    "inner join c_groups gg on gg.f_id=g.f_group "
                    "inner join c_units u on u.f_id=g.f_unit "
                    "group by 1, 2, 3, 4 ")
                    //"having sum(s.f_qty*s.f_type) > 0.00001 ")
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
              "where s.f_store=%1 and d.f_date<=%2 and d.f_state=1 "
              "group by 1, 2, 3, 4 "
              "having sum(s.f_qty*s.f_type) > 0.00001")
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
        goods << ui->tblGoods->item(i, 3)->data(Qt::EditRole).toInt();
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

void C5StoreDoc::addGoodsByCalculation(int goods, const QString &name, double qty)
{
    QJsonArray ja;
    C5Database db(fDBParams);
    db[":f_dish"] = goods;
    db.exec("select f_goods, f_qty from d_recipes where f_dish=:f_dish");
    int r;
    auto *c = C5Cache::cache(fDBParams, cache_goods);
    while (db.nextRow()) {
        QList<QVariant> vg = c->getValuesForId(db.getInt("f_goods"));
        r = -1;
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            if (ui->tblGoods->getInteger(i, 3) == db.getInt("f_goods")) {
                r = i;
                break;
            }
        }
        if (r < 0) {
            r = addGoodsRow();
        }
        ui->tblGoods->setString(r, 3, vg.at(0).toString());
        ui->tblGoods->setString(r, 4, vg.at(2).toString());
        ui->tblGoods->setString(r, 6, vg.at(3).toString());
        ui->tblGoods->lineEdit(r, 7)->setPlaceholderText(float_str(vg.at(5).toDouble() , 2));
        //ui->tblGoods->lineEdit(r, 5)->setDouble(db.getDouble("f_qty") * qty);
        QJsonObject jo;
        jo["f_goods"] = db.getInt("f_goods");
        jo["f_qty"] = db.getDouble("f_qty");
        jo["f_row"] = r;
        jo["f_prevqty"] = 0;
        ja.append(jo);
    }
    r = ui->tblDishes->addEmptyRow();
    ui->tblDishes->setInteger(r, 1, goods);
    ui->tblDishes->setString(r, 2, name);
    ui->tblDishes->createLineEdit(r, 3)->setFocus();
    ui->tblDishes->setString(r, 4, QJsonDocument(ja).toJson());
    connect(ui->tblDishes->lineEdit(r, 3), SIGNAL(textChanged(QString)), this, SLOT(tblDishQtyChanged(QString)));
    ui->tblDishes->lineEdit(r, 3)->setDouble(qty);
}

void C5StoreDoc::printV1()
{
    C5Printing p;
    QFont f(qApp->font());
    C5Database().logEvent(f.family());
    p.setFont(f);
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
    case DOC_TYPE_COMPLECTATION:
        docTypeText = tr("Store complectation");
        break;
    }

    p.ctext(QString("%1 N%2").arg(docTypeText, ui->leDocNum->text()));
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
        p.ltext(ui->leComment->text(), 50);
        p.br();
    }
    p.br();
    p.setFontBold(true);
    points.clear();
    points << 50 << 200;
    vals << tr("Date");
    if (!storeInName.isEmpty()) {
        vals << tr("Store, input");
        points << 500;
    }
    if (!storeOutName.isEmpty()) {
        vals << tr("Store, output");
        points << 500;
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

    if (fDocType == DOC_TYPE_COMPLECTATION) {
        points.clear();
        points << 50 << 100 << 800 << 250;
        vals.clear();
        vals.append(tr("NN"));
        vals.append(tr("Other charges"));
        vals.append(tr("Amount"));
        p.setFontBold(true);
        p.tableText(points, vals, p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
        for (int i = 0; i < ui->tblAdd->rowCount(); i++) {
            vals.clear();
            vals.append(QString::number(i + 1));
            vals.append(ui->tblAdd->getString(i, 1));
            vals.append(float_str(ui->tblAdd->lineEdit(i, 2)->getDouble(), 2));
            p.setFontBold(false);
            p.tableText(points, vals, p.fLineHeight + 20);
            p.br(p.fLineHeight + 20);
        }
        p.br();

        points.clear();
        points << 50 << 100 << 200 << 600 << 250 << 250 << 250;
        vals.clear();
        vals.append(tr("NN"));
        vals.append(tr("Code"));
        vals.append(tr("Input material"));
        vals.append(tr("Qty"));
        vals.append(tr("Price"));
        vals.append(tr("Amount"));
        p.setFontBold(true);
        p.tableText(points, vals, p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
        vals.clear();
        vals.append("1");
        vals.append(ui->leComplectationCode->text());
        vals.append(ui->leComplectationName->text());
        vals.append(ui->leComplectationQty->text());
        vals.append(float_str(ui->leTotal->getDouble() / ui->leComplectationQty->getDouble(), 2));
        vals.append(ui->leTotal->text());
        p.setFontBold(false);
        p.tableText(points, vals, p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
    }
    p.br();

    QString goodsColName = tr("Goods");
    if (fDocType == DOC_TYPE_COMPLECTATION) {
        goodsColName = tr("Output material");
    }
    points.clear();
    points << 50 << 100 << 200 << 600 << 250 << 250 << 250 << 250;
    vals.clear();
    vals << tr("NN")
         << tr("Material code")
         << goodsColName
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
        vals << ui->tblGoods->getString(i, 3);
        vals << ui->tblGoods->getString(i, 4);
        vals << ui->tblGoods->lineEdit(i, 5)->text();
        vals << ui->tblGoods->getString(i, 6);
        vals << ui->tblGoods->lineEdit(i, 7)->text();
        vals << ui->tblGoods->lineEdit(i, 8)->text();
        p.tableText(points, vals, p.fLineHeight + 20);
        if (p.checkBr(p.fLineHeight + 20)) {
            p.br(p.fLineHeight + 20);
        }
        p.br(p.fLineHeight + 20);
    }

    p.setFontBold(true);
    points.clear();
    points << 950
           << 750
           << 250;
    vals.clear();
    vals << tr("Total amount");
    vals << ui->leTotal->text();
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
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
    p.setFontBold(false);

    C5PrintPreview pp(&p, fDBParams);
    pp.exec();
}

void C5StoreDoc::printV2()
{
    C5Printing p;
    QFont f(qApp->font());
    C5Database().logEvent(f.family());
    p.setFont(f);
    QList<qreal> points;
    QStringList vals;
    p.setSceneParams(2700, 2000, QPrinter::Landscape);
    int c1 = (2700 / 2) / 2;
    int c2 = (2700 / 2) + (c1);
    int c0 = (2700 / 2) + 10;

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
    case DOC_TYPE_COMPLECTATION:
        docTypeText = tr("Store complectation");
        break;
    }

    p.ctextof(QString("%1 N%2").arg(docTypeText, ui->leDocNum->text()), c1);
    p.ctextof(QString("%1 N%2").arg(docTypeText, ui->leDocNum->text()), c2);
    p.br();
    p.ctextof(ui->leReasonName->text(), c1);
    p.ctextof(ui->leReasonName->text(), c2);
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
        p.ltext(ui->leComment->text(), 50);
        p.br();
    }
    p.br();
    //PART 1 - 1
    p.setFontBold(true);
    points.clear();
    points << 40 << 200;
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
    //PART 1 - 2
    p.setFontBold(true);
    points.clear();
    vals.clear();
    points << 40 + c0 << 200;
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

    //PART 2 - 1
    p.br(p.fLineHeight + 20);
    p.setFontBold(false);
    vals.clear();
    points.clear();
    points << 40 << 200;
    vals << ui->deDate->text();
    if (!storeInName.isEmpty()) {
        vals << ui->leStoreInputName->text();
        points << 400;
    }
    if (!storeOutName.isEmpty()) {
        vals << ui->leStoreOutputName->text();
        points << 400;
    }
    p.tableText(points, vals, p.fLineHeight + 20);

    //PART 2 - 2
    p.setFontBold(false);
    vals.clear();
    points.clear();
    points << 40 + c0 << 200 ;
    vals << ui->deDate->text();
    if (!storeInName.isEmpty()) {
        vals << ui->leStoreInputName->text();
        points << 400;
    }
    if (!storeOutName.isEmpty()) {
        vals << ui->leStoreOutputName->text();
        points << 400;
    }
    p.tableText(points, vals, p.fLineHeight + 20);


    p.br(p.fLineHeight + 20);

    //PART 3 - 1
    points.clear();
    vals.clear();
    p.setFontBold(true);
    points << 40;
    if (ui->lePartner->getInteger() > 0) {
        vals << tr("Supplier");
        points << 600;
    }
    if (!ui->leInvoiceNumber->isEmpty()) {
        vals << tr("Purchase document");
        points << 400;
    }
    p.tableText(points, vals, p.fLineHeight + 20);
    //PART 3 - 2
    points.clear();
    vals.clear();
    p.setFontBold(true);
    points << 40 + c0;
    if (ui->lePartner->getInteger() > 0) {
        vals << tr("Supplier");
        points << 600;
    }
    if (!ui->leInvoiceNumber->isEmpty()) {
        vals << tr("Purchase document");
        points << 400;
    }
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);

    //PART 4 - 1
    p.setFontBold(false);
    points.clear();
    vals.clear();
    points << 40;
    if (ui->lePartner->getInteger() > 0) {
        vals << ui->lePartnerName->text();
        points << 600;
    }
    if (!ui->leInvoiceNumber->isEmpty()) {
        vals << ui->leInvoiceNumber->text() + " /    " + ui->deInvoiceDate->text();
        points << 400;
    }
    p.tableText(points, vals, p.fLineHeight + 20);
    //PART 4 - 2
    points.clear();
    vals.clear();
    points << 40 + c0;
    if (ui->lePartner->getInteger() > 0) {
        vals << ui->lePartnerName->text();
        points << 600;
    }
    if (!ui->leInvoiceNumber->isEmpty()) {
        vals << ui->leInvoiceNumber->text() + " /    " + ui->deInvoiceDate->text();
        points << 400;
    }
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br();
    p.br();
    p.br();


    if (fDocType == DOC_TYPE_COMPLECTATION) {
        //PART 5 - 1
        points.clear();
        points << 40 << 100 << 800 << 250;
        vals.clear();
        vals.append(tr("NN"));
        vals.append(tr("Other charges"));
        vals.append(tr("Amount"));
        p.tableText(points, vals, p.fLineHeight + 20);
        //PART 5 - 2
        points.clear();
        points << 40 + c0 << 100 << 800 << 250;
        vals.clear();
        vals.append(tr("NN"));
        vals.append(tr("Other charges"));
        vals.append(tr("Amount"));
        p.setFontBold(true);
        p.tableText(points, vals, p.fLineHeight + 20);

        p.br(p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
        for (int i = 0; i < ui->tblAdd->rowCount(); i++) {
            //PART 6 - 1
            points.clear();
            points << 50 << 100 << 800 << 250;
            vals.clear();
            vals.append(QString::number(i + 1));
            vals.append(ui->tblAdd->getString(i, 1));
            vals.append(float_str(ui->tblAdd->lineEdit(i, 2)->getDouble(), 2));
            p.tableText(points, vals, p.fLineHeight + 20);
            //PART 6 - 2
            points.clear();
            points << 50 + c0 << 100 << 800 << 250;
            vals.clear();
            vals.append(QString::number(i + 1));
            vals.append(ui->tblAdd->getString(i, 1));
            vals.append(float_str(ui->tblAdd->lineEdit(i, 2)->getDouble(), 2));
            p.setFontBold(false);
            p.tableText(points, vals, p.fLineHeight + 20);
            p.br(p.fLineHeight + 20);
        }
        p.br();

        //PART 7 - 1
        points.clear();
        points << 50 << 100 << 200 << 600 << 250 << 250 << 250;
        vals.clear();
        vals.append(tr("NN"));
        vals.append(tr("Code"));
        vals.append(tr("Input material"));
        vals.append(tr("Qty"));
        vals.append(tr("Price"));
        vals.append(tr("Amount"));
        p.setFontBold(true);
        p.tableText(points, vals, p.fLineHeight + 20);
        //PART 7 - 2
        points.clear();
        points << 50 + c0 << 100 << 200 << 600 << 250 << 250 << 250;
        vals.clear();
        vals.append(tr("NN"));
        vals.append(tr("Code"));
        vals.append(tr("Input material"));
        vals.append(tr("Qty"));
        vals.append(tr("Price"));
        vals.append(tr("Amount"));
        p.setFontBold(true);
        p.tableText(points, vals, p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);

        //PART 8 - 1
        points.clear();
        points << 50 << 100 << 200 << 600 << 250 << 250 << 250;
        vals.clear();
        vals.append("1");
        vals.append(ui->leComplectationCode->text());
        vals.append(ui->leComplectationName->text());
        vals.append(ui->leComplectationQty->text());
        vals.append(float_str(ui->leTotal->getDouble() / ui->leComplectationQty->getDouble(), 2));
        vals.append(ui->leTotal->text());
        p.setFontBold(false);
        p.tableText(points, vals, p.fLineHeight + 20);
        //PART 8 - 2
        points.clear();
        points << 50 + c0 << 100 << 200 << 600 << 250 << 250 << 250;
        vals.clear();
        vals.append("1");
        vals.append(ui->leComplectationCode->text());
        vals.append(ui->leComplectationName->text());
        vals.append(ui->leComplectationQty->text());
        vals.append(float_str(ui->leTotal->getDouble() / ui->leComplectationQty->getDouble(), 2));
        vals.append(ui->leTotal->text());
        p.setFontBold(false);
        p.tableText(points, vals, p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
    }
    p.br();

    QString goodsColName = tr("Goods");
    if (fDocType == DOC_TYPE_COMPLECTATION) {
        goodsColName = tr("Output material");
    }
    //PART 9 - 1
    points.clear();
    points << 40 << 60 << 170 << 450 << 130 << 130 << 150 << 170;
    vals.clear();
    vals << tr("NN")
         << tr("Material code")
         << goodsColName
         << tr("Qty")
         << tr("Unit")
         << tr("Price")
         << tr("Total");
    p.setFontBold(true);
    p.tableText(points, vals, p.fLineHeight + 20);
    //PART 9 - 2
    points.clear();
    points << 40 + c0 << 60 << 170 << 450 << 130 << 130 << 150 << 170;
    vals.clear();
    vals << tr("NN")
         << tr("Material code")
         << goodsColName
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
        //PART 10 - 1
        points.clear();
        points << 40 << 60 << 170 << 450 << 130 << 130 << 150 << 170;
        vals.clear();
        vals << QString::number(i + 1);
        vals << ui->tblGoods->getString(i, 3);
        vals << ui->tblGoods->getString(i, 4);
        vals << ui->tblGoods->lineEdit(i, 5)->text();
        vals << ui->tblGoods->getString(i, 6);
        vals << ui->tblGoods->lineEdit(i, 7)->text();
        vals << ui->tblGoods->lineEdit(i, 8)->text();
        p.tableText(points, vals, p.fLineHeight + 20);
        //PART 10 - 2
        points.clear();
        points << 40 + c0 << 60 << 170 << 450 << 130 << 130 << 150 << 170;
        vals.clear();
        vals << QString::number(i + 1);
        vals << ui->tblGoods->getString(i, 3);
        vals << ui->tblGoods->getString(i, 4);
        vals << ui->tblGoods->lineEdit(i, 5)->text();
        vals << ui->tblGoods->getString(i, 6);
        vals << ui->tblGoods->lineEdit(i, 7)->text();
        vals << ui->tblGoods->lineEdit(i, 8)->text();
        p.tableText(points, vals, p.fLineHeight + 20);
        if (p.checkBr(p.fLineHeight + 20)) {
            p.br(p.fLineHeight + 20);
        }
        p.br(p.fLineHeight + 20);
    }

    p.setFontBold(true);
    //PART 11 - 1
    points.clear();
    points << 720 << 410 << 170;
    vals.clear();
    vals << tr("Total amount");
    vals << ui->leTotal->text();
    p.tableText(points, vals, p.fLineHeight + 20);
    //PART 11 - 1
    points.clear();
    points << 720 + c0 << 410 << 170;
    vals.clear();
    vals << tr("Total amount");
    vals << ui->leTotal->text();
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    switch (fDocType) {
    case DOC_TYPE_STORE_MOVE:
        p.ltext(tr("Passed"), 50);
        p.ltext(tr("Passed"), 50 + c0);
        p.ltext(tr("Accepted"), 1000);
        p.ltext(tr("Accepted"), 1000 + c0);
        p.br(p.fLineHeight + 20);
        p.ltext(ui->lePassedName->text(), 50);
        p.ltext(ui->leAcceptedName->text(), 1000);
        p.ltext(ui->lePassedName->text(), 50 + c0);
        p.ltext(ui->leAcceptedName->text(), 1000 + c0);
        break;
    default:
        p.ltext(tr("Accepted"), 50);
        p.ltext(tr("Passed"), 1000);
        p.ltext(tr("Accepted"), 50 + c0);
        p.ltext(tr("Passed"), 1000 + c0);
        p.br(p.fLineHeight + 20);
        p.ltext(ui->leAcceptedName->text(), 1000);
        p.ltext(ui->lePassedName->text(), 50);
        p.ltext(ui->leAcceptedName->text(), 1000 + c0);
        p.ltext(ui->lePassedName->text(), 50 + c0);
        break;
    }
    p.line(50, p.fTop, 500, p.fTop);
    p.line(1000, p.fTop, 1500, p.fTop);
    p.line(50 + c0, p.fTop, 500 + c0, p.fTop);
    p.line(1000 + c0, p.fTop, 1500 + c0, p.fTop);
    p.setFontBold(false);
    p.line(c0, 0, c0, p.fTop);

    C5PrintPreview pp(&p, fDBParams);
    pp.exec();
}

double C5StoreDoc::additionalCost()
{
    double c = 0;
    for (int i = 0; i < ui->tblAdd->rowCount(); i++) {
        c+= ui->tblAdd->lineEdit(i, 2)->getDouble();
    }
    return c;
}

double C5StoreDoc::additionalCostForEveryGoods()
{
    if (ui->leTotalQty->getDouble() > 0) {
        return additionalCost() / ui->leTotalQty->getDouble();
    }
    return 0;
}

void C5StoreDoc::calcPrice2(int row)
{
    double add = additionalCostForEveryGoods();
    ui->tblCalcPrice->setDouble(row, 1, ui->tblGoods->lineEdit(row, 5)->getDouble());
    ui->tblCalcPrice->setDouble(row, 3, ui->tblGoods->lineEdit(row, 7)->getDouble());
    ui->tblCalcPrice->setDouble(row, 4, ui->tblCalcPrice->getDouble(row, 3) + add);
    ui->tblCalcPrice->lineEdit(row, 7)->setDouble(ui->tblCalcPrice->getDouble(row, 4) + ui->tblCalcPrice->lineEdit(row, 6)->getDouble());
    if (ui->tblCalcPrice->getDouble(row, 4)) {
        ui->tblCalcPrice->lineEdit(row, 5)->setDouble(ui->tblCalcPrice->lineEdit(row, 6)->getDouble() / ui->tblCalcPrice->getDouble(row, 4) * 100);
    }
}

void C5StoreDoc::calcTotalSale()
{
    double total = 0;
    for (int i = 0; i < ui->tblCalcPrice->rowCount(); i++) {
        total += ui->tblCalcPrice->getDouble(i, 1) * ui->tblCalcPrice->lineEdit(i, 7)->getDouble();
    }
    ui->leTotalSale->setDouble(total);
}

void C5StoreDoc::writeAStoreSale(int storei, int storeo)
{
    C5StoreDraftWriter::writeASaleStore(storei, storeo);
}

void C5StoreDoc::lineEditKeyPressed(const QChar &key)
{
    switch (key.toLatin1()) {
    case '+':
        on_btnAddGoods_clicked();
        break;
    case '-':
        on_btnRemoveGoods_clicked();
        break;
    case '*':
        double v;
        if (Calculator::get(fDBParams, v)) {
            C5LineEdit *l = static_cast<C5LineEdit*>(sender());
            l->setDouble(v);
            l->textEdited(float_str(v, 2));
        }
        break;
    }
}

void C5StoreDoc::focusNextChildren()
{
    focusNextChild();
}

void C5StoreDoc::exportToExcel()
{
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
    case DOC_TYPE_COMPLECTATION:
        docTypeText = tr("Store complectation");
        break;
    }
    QString storeInName, storeOutName;
    if (ui->leStoreInput->getInteger() > 0) {
        storeInName = ui->leStoreInputName->text();
    }
    if (ui->leStoreOutput->getInteger() > 0) {
        storeOutName = ui->leStoreOutputName->text();
    }
    int fXlsxFitToPage = 0;
    QString fXlsxPageOrientation = xls_page_orientation_portrait;
    int fXlsxPageSize = xls_page_size_a4;

    XlsxDocument d;
    XlsxSheet *s = d.workbook()->addSheet("Sheet1");
    s->setupPage(fXlsxPageSize, fXlsxFitToPage, fXlsxPageOrientation);
    /* HEADER */
    QColor color = Qt::white;
    QFont headerFont(qApp->font());
    d.style()->addFont("header", headerFont);
    d.style()->addBackgrounFill("header", color);
    d.style()->addHAlignment("header", xls_alignment_center);
    d.style()->addBorder("header", XlsxBorder());


    s->setColumnWidth(1, 10);
    s->setColumnWidth(2, 15);
    s->setColumnWidth(3, 50);
    s->setColumnWidth(4, 20);
    s->setColumnWidth(5, 20);
    s->setColumnWidth(6, 20);
    s->setColumnWidth(7, 20);

    int col = 1, row = 1;
    s->addCell(row, col, QString("%1 N%2").arg(docTypeText, ui->leDocNum->text()),
                                           d.style()->styleNum("header"));

    row++;
    s->addCell(row, col, QString("%1 %2").arg(tr("Reason"), ui->leReasonName->text()),
                                           d.style()->styleNum("header"));
    row++;

    if (!ui->leComment->isEmpty()) {
        s->addCell(row, col, ui->leComment->text(), d.style()->styleNum("header"));
        row++;
    }

    QList<int> cols;
    QStringList vals;
    col = 1;
    cols << col++;
    vals << tr("Date");
    if (!storeInName.isEmpty()) {
        vals << tr("Store, input");
        cols << col++;
    }
    if (!storeOutName.isEmpty()) {
        vals << tr("Store, output");
        cols << col++;
    }
    for (int i = 0; i < cols.count(); i++) {
        s->addCell(row, cols.at(i), vals.at(i), d.style()->styleNum("header"));
    }
    row++;

    vals.clear();
    vals << ui->deDate->text();
    if (!storeInName.isEmpty()) {
        vals << ui->leStoreInputName->text();
    }
    if (!storeOutName.isEmpty()) {
        vals << ui->leStoreOutputName->text();
    }
    for (int i = 0; i < cols.count(); i++) {
        s->addCell(row, cols.at(i), vals.at(i), d.style()->styleNum("header"));
    }
    row += 2;

    cols.clear();
    for (int i = 0; i < 7; i++) {
        cols << i + 1;
    }
    vals.clear();
    vals << tr("NN")
         << tr("Material code")
         << tr("Goods")
         << tr("Qty")
         << tr("Unit")
         << tr("Price")
         << tr("Total");
    for (int i = 0; i < cols.count(); i++) {
        s->addCell(row, cols.at(i), vals.at(i), d.style()->styleNum("header"));
    }
    row++;
    QMap<int, QString> bgFill;
    QMap<int, QString> bgFillb;
    QFont bodyFont(qApp->font());
    d.style()->addFont("body", bodyFont);
    d.style()->addBackgrounFill("body", QColor(Qt::white));
    d.style()->addVAlignment("body", xls_alignment_center);
    d.style()->addBorder("body", XlsxBorder());
    bgFill[QColor(Qt::white).rgb()] = "body";
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        vals.clear();
        vals << QString::number(i + 1);
        vals << ui->tblGoods->getString(i, 3);
        vals << ui->tblGoods->getString(i, 4);
        vals << QString::number(str_float(ui->tblGoods->lineEdit(i, 5)->text()));
        vals << ui->tblGoods->getString(i, 6);
        vals << QString::number(str_float(ui->tblGoods->lineEdit(i, 7)->text()));
        vals << QString::number(str_float(ui->tblGoods->lineEdit(i, 8)->text()));
        for (int i = 0; i < cols.count(); i++) {
            s->addCell(row, cols.at(i), vals.at(i), d.style()->styleNum("body"));
        }
        row++;
    }

    cols.clear();
    cols << 5 << 6 << 7;
    vals.clear();
    vals << tr("Total amount");
    vals << QString::number(str_float(ui->leTotal->text()));
    vals << ui->cbCurrency->currentText();
    for (int i = 0; i < cols.count(); i++) {
        s->addCell(row, cols.at(i), vals.at(i), d.style()->styleNum("header"));
    }
    row++;


    col = 1;
    s->setSpan(1, col, 1, col + 5);
    s->setSpan(2, col, 2, col + 5);
    s->setSpan(3, col, 3, col + 5);
    s->setSpan(4, col, 4, col + 5);




    QString err;
    if (!d.save(err, true)) {
        if (!err.isEmpty()) {
            C5Message::error(err);
        }
    }


//    if (fDocType == DOC_TYPE_COMPLECTATION) {
//        points.clear();
//        points << 50 << 100 << 800 << 250;
//        vals.clear();
//        vals.append(tr("NN"));
//        vals.append(tr("Other charges"));
//        vals.append(tr("Amount"));
//        p.setFontBold(true);
//        p.tableText(points, vals, p.fLineHeight + 20);
//        p.br(p.fLineHeight + 20);
//        for (int i = 0; i < ui->tblAdd->rowCount(); i++) {
//            vals.clear();
//            vals.append(QString::number(i + 1));
//            vals.append(ui->tblAdd->getString(i, 1));
//            vals.append(float_str(ui->tblAdd->lineEdit(i, 2)->getDouble(), 2));
//            p.setFontBold(false);
//            p.tableText(points, vals, p.fLineHeight + 20);
//            p.br(p.fLineHeight + 20);
//        }
//        p.br();

//        points.clear();
//        points << 50 << 100 << 200 << 600 << 250 << 250 << 250;
//        vals.clear();
//        vals.append(tr("NN"));
//        vals.append(tr("Code"));
//        vals.append(tr("Input material"));
//        vals.append(tr("Qty"));
//        vals.append(tr("Price"));
//        vals.append(tr("Amount"));
//        p.setFontBold(true);
//        p.tableText(points, vals, p.fLineHeight + 20);
//        p.br(p.fLineHeight + 20);
//        vals.clear();
//        vals.append("1");
//        vals.append(ui->leComplectationCode->text());
//        vals.append(ui->leComplectationName->text());
//        vals.append(ui->leComplectationQty->text());
//        vals.append(float_str(ui->leTotal->getDouble() / ui->leComplectationQty->getDouble(), 2));
//        vals.append(ui->leTotal->text());
//        p.setFontBold(false);
//        p.tableText(points, vals, p.fLineHeight + 20);
//        p.br(p.fLineHeight + 20);
//    }

//    QString goodsColName = tr("Goods");
//    if (fDocType == DOC_TYPE_COMPLECTATION) {
//        goodsColName = tr("Output material");
//    }





}

void C5StoreDoc::dirtyEdit()
{
    DlgDirtyStoreDoc(fInternalId, fDBParams).exec();
    QString err;
    auto *doc =__mainWindow->createTab<C5StoreDoc>(fDBParams);
    if (!doc->openDoc(fInternalId, err)) {
        doc->correctDebt();
        __mainWindow->removeTab(doc);
        C5Message::error(err);
        return;
    }
    __mainWindow->removeTab(this);
}

void C5StoreDoc::newDoc()
{
    if (fDocState != DOC_STATE_SAVED && ui->tblGoods->rowCount() > 0) {
        C5Message::error(tr("Save current document first"));
        return;
    }
    fInternalId.clear();
    fBasedOnSale = 0;
    fComplectationId.clear();
    fDocState = DOC_STATE_DRAFT;
    ui->leDocNum->clear();
    ui->lePartner->setValue(0);
    ui->leReasonPartner->setValue(0);
    ui->leComment->clear();
    ui->leStoreInput->setValue(0);
    ui->leStoreOutput->setValue(0);
    ui->tblGoods->clearContents();
    ui->tblGoods->setRowCount(0);
    ui->tblDishes->clearContents();
    ui->tblCalcPrice->clearContents();
    ui->tblCalcPrice->setRowCount(0);
    ui->tblDishes->setRowCount(0);
    ui->leInvoiceNumber->clear();
    ui->leComplectationCode->setValue("");
    ui->leComplectationQty->clear();
    ui->tblAdd->clearContents();
    ui->tblAdd->setRowCount(0);
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
    if (__c5config.getRegValue("storedoc_storeinput").toBool()) {
        ui->leStoreInput->setValue(__c5config.getRegValue("storedoc_storeinput_id").toInt());
    }
    if (__c5config.getRegValue("storedoc_storeinput").toBool()) {
        ui->leStoreInput->setValue(__c5config.getRegValue("storedoc_storeinput_id").toInt());
    }
    countTotal();
}

void C5StoreDoc::getInput()
{
    QList<QVariant> vals;
    if (!C5Selector::getValueOfColumn(fDBParams, cache_goods, vals, 3)) {
        return;
    }
    if (vals.at(1).toInt() == 0) {
        C5Message::error(tr("Could not add goods without code"));
        return;
    }
    int row = addGoods(vals.at(1).toInt(), vals.at(3).toString(), -1, vals.at(4).toString(), -1, -1, "");
    if (__c5config.getValue(param_input_doc_fix_price).toInt() > 0) {
        ui->tblGoods->lineEdit(row, 7)->setDouble(vals.at(6).toDouble());
    }
    ui->tblGoods->lineEdit(row, 7)->setPlaceholderText(float_str(vals.at(6).toDouble(), 2));
    ui->tblGoods->lineEdit(row, 5)->setFocus();
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
                          "where s.f_store=%1 and d.f_date<=%2 and d.f_state=1 "
                          "group by 1, 2, 3, 4 "
                          "having sum(s.f_qty*s.f_type) > 0.00001 ")
            .arg(ui->leStoreOutput->getInteger())
            .arg(ui->deDate->toMySQLDate());
    QList<QVariant> vals;
    QHash<QString, QString> trans;
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
    ui->tblGoods->setString(row, 3, vals.at(1).toString());
    ui->tblGoods->setString(row, 4, vals.at(3).toString() + " " + vals.at(7).toString());
    ui->tblGoods->setString(row, 6, vals.at(4).toString());
    ui->tblGoods->lineEdit(row, 5)->setFocus();
}

void C5StoreDoc::saveDoc()
{
    writeDocumentWithState(DOC_STATE_SAVED);
    if (__c5config.getRegValue("storedoc_storeinput").toBool()) {
        __c5config.setRegValue("storedoc_storeinput_id", ui->leStoreInput->getInteger());
    }
    int cache_id = C5Cache::idForTable("c_goods");
    C5Selector::resetCache(fDBParams, cache_id);
}

void C5StoreDoc::draftDoc()
{
    writeDocumentWithState(DOC_STATE_DRAFT);
}

void C5StoreDoc::removeDocument()
{
    if (removeDoc(fDBParams, fInternalId, true)) {
        __mainWindow->removeTab(this);
    }
}

void C5StoreDoc::createStoreInput()
{
    C5Database db(fDBParams);
    QHash<int, double> prices1;
    db.exec("select f_id, f_lastinputprice from c_goods");
    while (db.nextRow()) {
        prices1[db.getInt(0)] = db.getDouble(1);
    }

    C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
    sd->setMode(C5StoreDoc::sdInput);
    sd->setStore(0, 0);
    sd->setReason(DOC_REASON_INPUT);
    sd->setComment(ui->leComment->text());
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        int row = sd->addGoodsRow();
        sd->ui->tblGoods->setString(row, 3, ui->tblGoods->getString(row, 3));
        sd->ui->tblGoods->setString(row, 4, ui->tblGoods->getString(row, 4));
        sd->ui->tblGoods->lineEdit(row, 5)->setDouble(ui->tblGoods->lineEdit(row, 5)->getDouble());
        sd->ui->tblGoods->setString(row, 6, ui->tblGoods->getString(row, 6));
        sd->ui->tblGoods->lineEdit(row, 7)->setDouble(prices1[ui->tblGoods->getInteger(row, 3)]);
        emit sd->ui->tblGoods->lineEdit(row, 7)->textEdited(sd->ui->tblGoods->lineEdit(row, 7)->text());
    }
}

void C5StoreDoc::printDoc()
{
    if (fInternalId == nullptr) {
        C5Message::error(tr("Document is not saved"));
        return;
    }
    C5StoreDocSelectPrintTemplate d;
    switch (d.exec()) {
    case 1:
        printV1();
        break;
    case 2:
        printV2();
        break;
    default:
        return;
    }
}

void C5StoreDoc::printBarcode()
{
    C5StoreBarcode *b = __mainWindow->createTab<C5StoreBarcode>(fDBParams);
    C5Database db(fDBParams);
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        db[":f_id"] = ui->tblGoods->getInteger(i, 3);
        db.exec("select f_name, f_scancode from c_goods where f_id=:f_id");
        if (db.nextRow()) {
            b->addRow(db.getString("f_name"), db.getString("f_scancode"), ui->tblGoods->lineEdit(i, 5)->getInteger(),
                      ui->cbCurrency->currentData().toInt(), "");
        }
    }
}

void C5StoreDoc::checkInvoiceDuplicate()
{
    ui->leInvoiceNumber->setStyleSheet("");
    C5Database db(fDBParams);
    db[":f_invoice"] = ui->leInvoiceNumber->text();
    db[":f_id"] = fInternalId;
    db.exec("select count(f_id) from a_header_store where f_invoice=:f_invoice and f_id<>:f_id");
    if (db.nextRow()) {
        if (db.getInt(0) > 0) {
            ui->leInvoiceNumber->setStyleSheet("background:red;");
        } else {
            ui->leInvoiceNumber->setStyleSheet("");
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

void C5StoreDoc::tblDishQtyChanged(const QString &arg1)
{
    int r, c;
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    if (!ui->tblDishes->findWidget(l, r, c) ) {
        return;
    }
    QJsonArray ja = QJsonDocument::fromJson(ui->tblDishes->getString(r, 4).toUtf8()).array();
    for (int i = 0; i < ja.count(); i++) {
        bool found = false;
        QJsonObject jo = ja.at(i).toObject();
        for (int j = 0; j < ui->tblGoods->rowCount(); j++) {
            qDebug() << jo << ui->tblGoods->getInteger(j, 3);
            if (ui->tblGoods->getInteger(j, 3) == jo["f_goods"].toInt()) {
                ui->tblGoods->lineEdit(j, 5)->setDouble(ui->tblGoods->lineEdit(j, 5)->getDouble() - (jo["f_qty"].toDouble() * jo["f_prevqty"].toDouble()));
                jo["f_prevqty"] = arg1.toDouble();
                ui->tblGoods->lineEdit(j, 5)->setDouble(ui->tblGoods->lineEdit(j, 5)->getDouble() + (jo["f_qty"].toDouble() * jo["f_prevqty"].toDouble()));
                ja[i] = jo;
                found = true;
                break;
            }
        }
        if (!found) {
            int r = addGoodsRow();
            ui->tblGoods->setString(r, 3, jo["f_goods"].toString());
            ui->tblGoods->lineEdit(r, 5)->setDouble(jo["f_qty"].toDouble() * jo["f_prevqty"].toDouble());

        }
    }
    ui->tblDishes->setString(r, 4, QJsonDocument(ja).toJson());
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
    calcPrice2(row);
    calcTotalSale();
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
    double d1 = lqty->getDouble();
    double d2 = lprice->getDouble();
    ltotal->setDouble(d1 * d2);

    countTotal();
    calcPrice2(row);
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
    calcPrice2(row);
}

void C5StoreDoc::tblCalcMarginPercentChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;
    if (!ui->tblCalcPrice->findWidget(static_cast<QWidget*>(sender()), row, col)) {
        return;
    }
    C5LineEdit *lmarginPercent = ui->tblCalcPrice->lineEdit(row, 5);
    C5LineEdit *lmargin = ui->tblCalcPrice->lineEdit(row, 6);
    C5LineEdit *lfinal = ui->tblCalcPrice->lineEdit(row, 7);
    lmargin->setDouble(ui->tblCalcPrice->getDouble(row, 4) * (lmarginPercent->getDouble() / 100));
    lfinal->setDouble(lmargin->getDouble() + ui->tblCalcPrice->getDouble(row, 4));
    calcTotalSale();
}

void C5StoreDoc::tblCalcMarginChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;
    if (!ui->tblCalcPrice->findWidget(static_cast<QWidget*>(sender()), row, col)) {
        return;
    }
    C5LineEdit *lmarginPercent = ui->tblCalcPrice->lineEdit(row, 5);
    C5LineEdit *lmargin = ui->tblCalcPrice->lineEdit(row, 6);
    C5LineEdit *lfinal = ui->tblCalcPrice->lineEdit(row, 7);
    if (ui->tblCalcPrice->getDouble(row, 4)) {
        lmarginPercent->setDouble((lmargin->getDouble() / ui->tblCalcPrice->getDouble(row, 4)) * 100);
    }
    lfinal->setDouble(lmargin->getDouble() + ui->tblCalcPrice->getDouble(row, 4));
    calcTotalSale();
}

void C5StoreDoc::tblCalcFinalChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;
    if (!ui->tblCalcPrice->findWidget(static_cast<QWidget*>(sender()), row, col)) {
        return;
    }
    C5LineEdit *lmarginPercent = ui->tblCalcPrice->lineEdit(row, 5);
    C5LineEdit *lmargin = ui->tblCalcPrice->lineEdit(row, 6);
    C5LineEdit *lfinal = ui->tblCalcPrice->lineEdit(row, 7);
    lmargin->setDouble(lfinal->getDouble() - ui->tblCalcPrice->getDouble(row, 4));
    if (ui->tblCalcPrice->getDouble(row, 4)) {
        lmarginPercent->setDouble((lmargin->getDouble() / ui->tblCalcPrice->getDouble(row, 4)) * 100);
    }
    calcTotalSale();
}

void C5StoreDoc::tblAddChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    for (int i = 0; i < ui->tblCalcPrice->rowCount(); i++) {
        calcPrice2(i);
    }
    calcTotalSale();
    countTotal();
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
    case DOC_TYPE_DECOMPLECTATION:
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
    if (C5Message::question(tr("Confirm to remove") + "<br>" + ui->tblGoods->item(row, 4)->text()) != QDialog::Accepted) {
        return;
    }
    ui->tblGoods->removeRow(row);
    ui->tblCalcPrice->removeRow(row);
    if (ui->tblDishes->rowCount() > 0) {
        for (int i = 0; i < ui->tblDishes->rowCount(); i++) {
            QJsonArray ja = QJsonDocument::fromJson(ui->tblDishes->getString(i, 4).toUtf8()).array();
            for (int j = ja.count() - 1; j > -1; j--) {
                QJsonObject jo = ja[j].toObject();
                if (jo["f_row"].toInt() == row) {
                    ja.removeAt(j);
                    continue;
                }
                if (jo["f_row"].toInt() > row) {
                    jo["f_row"] = jo["f_row"].toInt() - 1;
                    ja[j] = jo;
                }
            }
            ui->tblDishes->setString(i, 4, QJsonDocument(ja).toJson());
        }
    }
    countTotal();
}

void C5StoreDoc::on_leStoreInput_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    bool v = C5Config::getRegValue("showhidegoods").toBool();
    if (v) {
        setGoodsPanelHidden(v);
    }
    setUserId(false, ui->leDocNum->getInteger());
}

void C5StoreDoc::on_leStoreOutput_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    bool v = C5Config::getRegValue("showhidegoods").toBool();
    if (v) {
        setGoodsPanelHidden(v);
    }
    setUserId(false, ui->leDocNum->getInteger());
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
    ui->tblGoods->setInteger(row, 3, ui->tblGoodsStore->getInteger(r, 0));
    ui->tblGoods->setString(row, 4, ui->tblGoodsStore->getString(r, 2));
    ui->tblGoods->setString(row, 6, ui->tblGoodsStore->getString(r, 4));
    ui->tblGoods->lineEdit(row, 5)->setFocus();
    markGoodsComplited();
}

void C5StoreDoc::on_btnNewPartner_clicked()
{
    CE5Partner *ep = new CE5Partner(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
       // ui->lePartner->setValue(data.at(0)["f_id"].toString());
    }
    delete e;
}

void C5StoreDoc::on_btnNewGoods_clicked()
{
    CE5Goods *ep = new CE5Goods(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        if (data.at(0)["f_id"].toInt() == 0) {
            C5Message::error(tr("Cannot add goods without code"));
            return;
        }
        int row = addGoodsRow();
        ui->tblGoods->setData(row, 3, data.at(0)["f_id"]);
        ui->tblGoods->setData(row, 4, data.at(0)["f_name"].toString() + " " + data.at(0)["f_scancode"].toString());
        ui->tblGoods->setData(row, 6, data.at(0)["f_unitname"]);
        ui->tblGoods->lineEdit(row, 5)->setFocus();
    }
    delete e;
}

void C5StoreDoc::on_leScancode_returnPressed()
{
    QString qty  = ui->chLeaveFocusOnBarcode->isChecked() ? "1" : "";
    addByScancode(ui->leScancode->text(), qty, "");
    if (ui->chLeaveFocusOnBarcode->isChecked()) {
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

void C5StoreDoc::on_leComplectationName_textChanged(const QString &arg1)
{
    ui->tblGoods->clearContents();
    ui->tblGoods->setRowCount(0);
    fBaseQtyOfComplectation.clear();
    fBaseComplectOutput = 0;
    C5Cache *c = C5Cache::cache(fDBParams, cache_goods);
    int gr = c->find(ui->leComplectationCode->getInteger());
    if (gr > -1) {
        ui->lbComplectUnit->setText(c->getString(gr, tr("Unit")));
        fBaseComplectOutput = c->getString(gr, tr("Complect output")).toDouble();
        ui->leComplectationScancodeCode->setText(c->getString(gr, tr("Scancode")));
    } else {
        ui->lbComplectUnit->clear();
    }
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
        fBaseQtyOfComplectation.insert(row, db.getDouble("f_qty"));
        ui->tblGoods->setInteger(row, 3, db.getInt("f_goods"));
        ui->tblGoods->setString(row, 4, db.getString("f_goodsname"));
        ui->tblGoods->lineEdit(row, 5)->setDouble(db.getDouble("f_qty"));
        ui->tblGoods->setString(row, 6, db.getString("f_unitname"));
        ui->tblGoods->lineEdit(row, 7)->setDouble(db.getDouble("f_price"));
        ui->tblGoods->lineEdit(row, 8)->setDouble(db.getDouble("f_total"));
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
    countTotal();
    for (int i =0; i < ui->tblCalcPrice->rowCount(); i++) {
        calcPrice2(i);
    }
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
    for (int i =0; i < ui->tblCalcPrice->rowCount(); i++) {
        calcPrice2(i);
    }
}

void C5StoreDoc::on_btnInsertLast_clicked()
{
    C5Database db(fDBParams);
    db[":f_name"] = "last_store_additions";
    db.exec("select f_value from s_draft where f_name=:f_name");
    if (db.nextRow()) {
        db[":f_header"] = db.getString("f_id");
        db.exec("select * from a_complectation_additions where f_header=:f_header order by f_row");
        while (db.nextRow()) {
            int row = ui->tblAdd->rowCount();
            ui->tblAdd->setRowCount(row + 1);
            C5LineEdit *l = ui->tblAdd->createLineEdit(row, 1);
            l->setText(db.getString("f_name"));
            l = ui->tblAdd->createLineEdit(row, 2);
            l->setDouble(db.getDouble("f_amount"));
            l->setValidator(new QDoubleValidator());
            connect(l, SIGNAL(textEdited(QString)), this, SLOT(tblAddChanged(QString)));
        }
    }
}

void C5StoreDoc::on_btnEditGoods_clicked()
{
    int row = ui->tblGoods->currentRow();
    if (row < 0) {
        return;
    }
    if (ui->tblGoods->getInteger(row, 3) == 0) {
        return;
    }
    CE5Goods *ep = new CE5Goods(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    ep->setId(ui->tblGoods->getInteger(row, 3));
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        if (data.at(0)["f_id"].toInt() == 0) {
            C5Message::error(tr("Cannot change goods without code"));
            return;
        }
        ui->tblGoods->setData(row, 3, data.at(0)["f_id"]);
        ui->tblGoods->setData(row, 4, data.at(0)["f_name"]);
        ui->tblGoods->setData(row, 6, data.at(0)["f_unitname"]);
        ui->tblGoods->lineEdit(row, 5)->setFocus();
    }
    delete e;
}

void C5StoreDoc::on_btnCalculator_clicked()
{
    double v;
    Calculator::get(fDBParams, v);
}

void C5StoreDoc::inputOfService()
{
    if (ui->leStoreInput->getInteger() == 0) {
        C5Message::error(tr("Store must be defined"));
        return;
    }
    QDate d1, d2;
    if (!C5DateRange::dateRange(d1, d2)) {
        return;
    }
    C5Database db(fDBParams);
    db[":date1"] = d1;
    db[":date2"] = d2;
    db[":f_store"] = ui->leStoreInput->getInteger();
    db.exec("select og.f_id, oh.f_prefix, oh.f_hallid, oh.f_datecash, g.f_name, g.f_scancode, og.f_qty, "
            "u.f_name as f_unitname, og.f_goods "
            "from o_goods og "
            "inner join o_header oh on oh.f_id=og.f_header "
            "inner join c_goods g on g.f_id=og.f_goods "
            "inner join c_units u on u.f_id=g.f_unit "
            "where oh.f_datecash between :date1 and :date2 "
            "and og.f_store=:f_store and og.f_storerec is null ");
    while (db.nextRow()) {
        int row = addGoodsRow();
        ui->tblGoods->setData(row, 3, db.getInt("f_goods"));
        ui->tblGoods->setData(row, 4, db.getString("f_name") + " " + db.getString("f_scancode"));
        ui->tblGoods->setData(row, 6, db.getString("f_unitname"));
        ui->tblGoods->lineEdit(row, 5)->setDouble(db.getDouble("f_qty"));
        ui->tblGoods->lineEdit(row, 9)->setText(QString("%1, %2%3, %4").arg(db.getDate("f_datecash").toString(FORMAT_DATE_TO_STR)).arg(db.getString("f_prefix")).arg(db.getInt("f_hallid")).arg(db.getString("f_id")));
    }
}

void C5StoreDoc::outputOfService()
{
    if (fDocState != DOC_STATE_SAVED) {
        C5Message::error(tr("Document must be saved"));
        return;
    }
    auto *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
    sd->setMode(sdOutput);
    sd->setStore(0, ui->leStoreInput->getInteger());
    sd->setReason(DOC_REASON_SALE);
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        sd->addGoods(ui->tblGoods->getInteger(i, 3),
                     ui->tblGoods->getString(i, 4),
                     ui->tblGoods->lineEdit(i, 5)->getDouble(),
                     ui->tblGoods->getString(i, 6),
                     ui->tblGoods->lineEdit(i, 7)->getDouble(),
                     ui->tblGoods->lineEdit(i, 8)->getDouble(),
                     ui->tblGoods->lineEdit(i, 9)->text());
    }
    sd->setFlag("outputservice", 1);
}

void C5StoreDoc::duplicateOutput()
{
    if (fDocState != DOC_STATE_SAVED) {
        C5Message::error(tr("Document must be saved"));
        return;
    }
    auto *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
    sd->setMode(sdOutput);
    sd->setStore(0, ui->leStoreInput->getInteger());
    sd->setReason(DOC_REASON_OUT);
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        sd->addGoods(ui->tblGoods->getInteger(i, 3),
                     ui->tblGoods->getString(i, 4),
                     ui->tblGoods->lineEdit(i, 5)->getDouble(),
                     ui->tblGoods->getString(i, 6),
                     ui->tblGoods->lineEdit(i, 7)->getDouble(),
                     ui->tblGoods->lineEdit(i, 8)->getDouble(),
                     ui->tblGoods->lineEdit(i, 9)->text());
    }
}

void C5StoreDoc::duplicateAsInput()
{
    if (fDocState != DOC_STATE_SAVED) {
        C5Message::error(tr("Document must be saved"));
        return;
    }
    auto *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
    sd->setMode(sdInput);
    sd->setStore(0, ui->leStoreInput->getInteger());
    sd->setReason(DOC_REASON_INPUT);
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        sd->addGoods(ui->tblGoods->getInteger(i, 3),
                     ui->tblGoods->getString(i, 4),
                     ui->tblGoods->lineEdit(i, 5)->getDouble(),
                     ui->tblGoods->getString(i, 6),
                     ui->tblGoods->lineEdit(i, 7)->getDouble(),
                     ui->tblGoods->lineEdit(i, 8)->getDouble(),
                     ui->tblGoods->lineEdit(i, 9)->text());
    }
}

void C5StoreDoc::on_btnAddDish_clicked()
{
    QList<QVariant> vals;
    if (!C5Selector::getValue(fDBParams, cache_dish, vals)) {
        return;
    }
    if (vals.at(1).toInt() == 0) {
        C5Message::error(tr("Could not add goods without code"));
        return;
    }
    addGoodsByCalculation(vals.at(1).toInt(), vals.at(2).toString(), 0);
}

void C5StoreDoc::on_btnRemoveRows_clicked()
{
    int row = ui->tblDishes->currentRow();
    if (row < 0) {
        return;
    }
    if (C5Message::question(tr("Confirm to remove") + "<br>" + ui->tblDishes->item(row, 2)->text()) != QDialog::Accepted) {
        return;
    }
    QJsonArray ja = QJsonDocument::fromJson(ui->tblDishes->getString(row, 4).toUtf8()).array();
    for (int i = 0; i < ja.count(); i++) {
        QJsonObject jo = ja[i].toObject();
        for (int j = 0; j < ui->tblGoods->rowCount(); j++) {
            if (jo["f_goods"].toInt() == ui->tblGoods->getInteger(j, 3)) {
                ui->tblGoods->lineEdit(j, 5)->setDouble(ui->tblGoods->lineEdit(j, 5)->getDouble() - (jo["f_prevqty"].toDouble() * jo["f_qty"].toDouble()));
                if (ui->tblGoods->lineEdit(j, 5)->getDouble() < 0.001) {
                    ui->tblGoods->removeRow(j);
                }
                break;
            }
        }
    }
    ui->tblDishes->removeRow(row);
    countTotal();
}

void C5StoreDoc::on_btnRememberStoreIn_clicked(bool checked)
{
    __c5config.setRegValue("storedoc_storeinput", checked);
}

void C5StoreDoc::on_btnCopyUUID_clicked()
{
    qApp->clipboard()->setText(fInternalId);
}

void C5StoreDoc::on_leSearchInDoc_textChanged(const QString &arg1)
{
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        bool hidden = !arg1.isEmpty();
        if (!arg1.isEmpty()) {
            hidden = !(ui->tblGoods->getString(i, 4).contains(arg1, Qt::CaseInsensitive) || ui->tblGoods->getString(i, 3).contains(arg1, Qt::CaseInsensitive));
        }
        ui->tblGoods->setRowHidden(i, hidden);
    }
}

void C5StoreDoc::on_btnCloseSearch_clicked()
{
    ui->wSearchInDocs->setVisible(false);
    on_leSearchInDoc_textChanged("");
}


void C5StoreDoc::on_btnFillRemote_clicked(bool checked)
{
    if (checked) {
        ui->btnFillRemote->setIcon(QIcon(":/wifi.png"));
        __mainWindow->addBroadcastListener(this);
    } else {
        ui->btnFillRemote->setIcon(QIcon(":/wifib.png"));
        __mainWindow->removeBroadcastListener(this);
    }
}

void C5StoreDoc::on_btnAddPackages_clicked()
{
    QList<QVariant> vals;
    if (!C5Selector::getValue(fDBParams, cache_dish_package, vals)) {
        return;
    }
    if (vals.at(1).toInt() == 0) {
        C5Message::error(tr("Could not add package without code"));
        return;
    }
    bool ok = false;
    double qty = QInputDialog::getDouble(this, tr("Qty of package"), "", 0, 1, 999999, 2, &ok);
    if (!ok) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_package"] = vals.at(1);
    db.exec("select d.f_name, p.f_dish, p.f_qty from d_package_list p left join d_dish d on d.f_id=p.f_dish where f_package=:f_package");
    while (db.nextRow()) {
        addGoodsByCalculation(db.getInt("f_dish"), db.getString("f_name"), db.getDouble("f_qty") * qty);
    }
}

void C5StoreDoc::on_btnSetMargin_clicked()
{
    for (int i = 0; i < ui->tblCalcPrice->rowCount(); i++) {
        ui->tblCalcPrice->lineEdit(i, 6)->setDouble(ui->leAllMargin->getDouble());
        calcPrice2(i);
    }
    calcTotalSale();
}

void C5StoreDoc::on_btnSetAllMarginPercent_clicked()
{
    for (int i = 0; i < ui->tblCalcPrice->rowCount(); i++) {
        ui->tblCalcPrice->lineEdit(i, 6)->setDouble(ui->tblCalcPrice->getDouble(i, 4) * (ui->leAllMarginPercent->getDouble() / 100));
        calcPrice2(i);
    }
    calcTotalSale();
}

void C5StoreDoc::on_leTotalSale_textChanged(const QString &arg1)
{
    ui->leProfit->setDouble(str_float(arg1) - ui->leTotal->getDouble());
}

void C5StoreDoc::on_btnAutoFillSalePrice_clicked()
{
    C5Database db(fDBParams);
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        db[":f_id"] = ui->tblGoods->getInteger(i, 3);
        db.exec("select f_saleprice from c_goods where f_id=:f_id");
        if (db.nextRow()) {
            ui->tblCalcPrice->lineEdit(i, 7)->setDouble(db.getDouble("f_saleprice"));
            emit ui->tblCalcPrice->lineEdit(i, 7)->textEdited(ui->tblCalcPrice->lineEdit(i, 7)->text());
        }
    }
    calcTotalSale();
}

void C5StoreDoc::on_leReason_textChanged(const QString &arg1)
{
    setReasonPartnerName();
}
