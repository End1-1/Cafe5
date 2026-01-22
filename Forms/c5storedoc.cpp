#include "c5storedoc.h"
#include "ui_c5storedoc.h"
#include "c5selector.h"
#include "c5cache.h"
#include "c5cashdoc.h"
#include "c5editor.h"
#include "c5mainwindow.h"
#include "c5daterange.h"
#include "ce5partner.h"
#include "dlgdirtystoredoc.h"
#include "ce5goods.h"
#include "c5storebarcode.h"
#include "c5config.h"
#include "c5user.h"
#include "c5message.h"
#include "c5storedraftwriter.h"
#include "calculator.h"
#include "c5utils.h"
#include "bclientdebts.h"
#include "c5storedocselectprinttemplate.h"
#include "c5permissions.h"
#include "c5htmlprint.h"
#include <QMenu>
#include <QHash>
#include <QClipboard>
#include <QInputDialog>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPrintPreviewDialog>
#include <QShortcut>
#include <QSqlQuery>
#include <QFileDialog>
#include <QDesktopServices>
#include <QXlsx/header/xlsxdocument.h>
#include <QPrinter>

#define col_rec_in_id 0
#define col_rec_out_id 1
#define col_rec_base_id 2
#define col_goods_id 3
#define col_goods_name 4
#define col_adgt 5
#define col_goods_qty 6
#define col_goods_unit 7
#define col_price 8
#define col_total 9
#define col_valid_date 10
#define col_comment 11
#define col_remain 12

C5StoreDoc::C5StoreDoc(QWidget *parent) :
    C5Document(parent),
    ui(new Ui::C5StoreDoc)
{
    ui->setupUi(this);
    fIconName = ":/goods.png";
    fLabel = tr("Store document");
    ui->leCash->setSelector(ui->leCashName, cache_cash_names);
    ui->leStoreInput->setSelector(ui->leStoreInputName, cache_goods_store);
    ui->leStoreOutput->setSelector(ui->leStoreOutputName, cache_goods_store);
    ui->leStoreInput->addEventKeys("+-");
    ui->leStoreOutput->addEventKeys("+-");
    ui->lbComplectUnit->setText("");
    ui->wSearchInDocs->setVisible(false);
    connect(ui->leStoreInput, SIGNAL(keyPressed(QChar)), this, SLOT(lineEditKeyPressed(QChar)));
    connect(ui->leStoreOutput, SIGNAL(keyPressed(QChar)), this, SLOT(lineEditKeyPressed(QChar)));
    ui->leReason->setSelector(ui->leReasonName, cache_store_reason);
    ui->lePartner->setSelector(ui->lePartnerName, cache_goods_partners, 1, 4);
    ui->lePartner->setCallbackWidget(this);
    ui->leComplectationCode->setSelector(ui->leComplectationName, cache_goods, 1, 3);
    disconnect(ui->leComplectationName, SIGNAL(textChanged(QString)), this,
               SLOT(on_leComplectationName_textChanged(QString)));
    connect(ui->leComplectationName, SIGNAL(textChanged(QString)), this, SLOT(on_leComplectationName_textChanged(QString)));
#ifdef QT_DEBUG
    ui->tblDishes->setColumnWidths(ui->tblDishes->columnCount(), 0, 0, 400, 80, 400);
#else
    ui->tblDishes->setColumnWidths(ui->tblDishes->columnCount(), 0, 0, 400, 80, 0);
#endif
    QMap<int, int> colwidths = {{col_rec_in_id, 0},
        {col_rec_out_id, 0},
        {col_rec_base_id, 0},
        {col_goods_id, 0},
        {col_goods_name, 350},
        {col_goods_qty, 80},
        {col_goods_unit, 80},
        {col_price, 80},
        {col_total, 80},
        {col_valid_date, 80},
        {col_comment, 300},
        {col_remain, 80},
        {col_adgt, 50}
    };

    for(auto [col, width] : colwidths.asKeyValueRange()) {
        ui->tblGoods->setColumnWidth(col, width);
    }

    ui->btnNewPartner->setEnabled(mUser->check(cp_t7_partners));
    ui->btnNewGoods->setEnabled(mUser->check(cp_t6_goods));
    ui->btnEditGoods->setEnabled(mUser->check(cp_t6_goods));
    ui->leAccepted->setSelector(ui->leAcceptedName, cache_users);
    ui->lePassed->setSelector(ui->lePassedName, cache_users);
    ui->lbCashDoc->setVisible(!C5Config::noCashDocStore());
    ui->leCash->setVisible(!C5Config::noCashDocStore());
    ui->leCashName->setVisible(!C5Config::noCashDocStore());
    ui->deCashDate->setVisible(!C5Config::noCashDocStore());
    ui->chPaid->setVisible(!C5Config::noCashDocStore());
    ui->leScancode->setVisible(!C5Config::noScanCodeStore());
    fInternalId = C5Database::uuid();
    fDocState = DOC_STATE_DRAFT;
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
    connect(f2, &QShortcut::activated, [this]() {
        ui->leScancode->setFocus();
    });
    fFocusNextChild = false;
    fCanChangeFocus = true;
    ui->tblAdd->setColumnWidths(ui->tblAdd->columnCount(), 0, 300, 80);
    ui->leComplectationQty->setValidator(new QDoubleValidator(0, 999999999, 3));
    ui->deDate->setEnabled(mUser->check(cp_t1_allow_change_store_doc_date));
    ui->btnRememberStoreIn->setChecked(__c5config.getRegValue("storedoc_storeinput").toBool());

    if(__c5config.getRegValue("storedoc_storeinput").toBool()) {
        ui->leStoreInput->setValue(__c5config.getRegValue("storedoc_storeinput_id").toInt());
    }

    if(__c5config.getRegValue("storedoc_fixpartner").toInt() > 0) {
        ui->lePartner->setValue(__c5config.getRegValue("storedoc_fixpartner").toInt());
        ui->btnFixPartner->setChecked(true);
    }

    adjustSize();
    C5Database db;
    db.exec("select * from e_currency order by f_id");

    while(db.nextRow()) {
        ui->cbCurrency->addItem(db.getString("f_name"), db.getInt("f_id"));
    }

    ui->cbCurrency->setCurrentIndex(ui->cbCurrency->findData(__c5config.getValue(param_default_currency).toInt()));
}

C5StoreDoc::~C5StoreDoc()
{
    __mainWindow->removeBroadcastListener(this);
    delete ui;
}

bool C5StoreDoc::openDoc(QString id, QString &err)
{
    ui->leDocNum->setPlaceholderText("");
    C5Database db;
    //    C5StoreDraftWriter dw(db);
    //    if (!dw.readAHeader(id)) {
    //        err = dw.fErrorMsg;
    //        return false;
    //    }
    db.exec(QString("select sf_open_store_document('{\"f_id\":\"%1\"}')").arg(id));

    if(!db.nextRow()) {
        C5Message::error(tr("Document not exists"));
        return false;
    }

    QJsonObject reply = QJsonDocument::fromJson(db.getString(0).toUtf8()).object();

    if(reply["status"].toInt() != 1) {
        C5Message::error(reply["data"].toString());
        return false;
    }

    reply = reply["data"].toObject();
    QJsonObject doc_header = reply["doc_header"].toObject();
    QJsonArray doc_body = reply["doc_body"].toArray();
    QJsonObject doc_client = reply["doc_client"].toObject();
    QJsonObject doc_header_store = doc_header["f_body"].toObject();

    if(doc_header_store.isEmpty()) {
        doc_header_store = reply["doc_header_store"].toObject();
    }

    QJsonObject doc_remains = doc_header_store["remains"].toObject();
    fInternalId = id;
    ui->leDocNum->setText(doc_header["f_userid"].toString());
    ui->deDate->setDate(QDate::fromString(doc_header["f_date"].toString(), FORMAT_DATE_TO_STR_MYSQL));
    fDocType = doc_header["f_type"].toInt();
    fDocState = doc_header["f_state"].toInt();
    ui->lePartner->setValue(doc_header["f_partner"].toInt());
    ui->leComment->setText(doc_header["f_comment"].toString());
    ui->leTotal->setDouble(doc_header["f_amount"].toDouble());
    ui->cbCurrency->setCurrentIndex(ui->cbCurrency->findData(doc_header["f_currency"].toInt()));
    setMode(static_cast<STORE_DOC>(fDocType));
    ui->tblGoods->setRowCount(0);

    for(int i = 0; i < doc_body.size(); i++) {
        const QJsonObject &jo = doc_body.at(i).toObject();
        int row = -1;

        if(jo["f_type"].toInt() == 1) {
            if(doc_header["f_type"].toInt() == DOC_TYPE_COMPLECTATION) {
                continue;
            }

            if(doc_header["f_type"].toInt() == DOC_TYPE_STORE_MOVE) {
                continue;
            }

            //            for (int j = 0; j < ui->tblGoods->rowCount(); j++) {
            //                if (jo["f_id"].toString() == ui->tblGoods->getString(j, 2)) {
            //                    row = j;
            //                    break;
            //                }
            //            }
        } else {
            for(int j = 0; j < ui->tblGoods->rowCount(); j++) {
                if(jo["f_base"].toString().isEmpty()) {
                    break;
                }

                if(jo["f_base"].toString() == ui->tblGoods->getString(j, 0)) {
                    row = j;
                    break;
                }
            }
        }

        if(row < 0) {
            row = addGoodsRow();
        }

        //if (ui->leReason->getInteger() == 0) {
        ui->leReason->setValue(jo["f_reason"].toInt());

        //}
        if(jo["f_type"].toInt() == 1) {
            ui->tblGoods->setString(row, col_rec_in_id, jo["f_id"].toString());
        } else {
            ui->tblGoods->setString(row, col_rec_out_id, jo["f_id"].toString());
            ui->tblGoods->setString(row, col_rec_base_id, jo["f_base"].toString());
        }

        ui->tblGoods->setInteger(row, col_goods_id, jo["f_goods"].toInt());
        ui->tblGoods->setString(row, col_goods_name, jo["f_goodsname"].toString());
        ui->tblGoods->lineEdit(row, col_goods_qty)->setDouble(jo["f_qty"].toDouble());
        ui->tblGoods->setString(row, col_goods_unit, jo["f_unitname"].toString());
        ui->tblGoods->lineEdit(row, col_price)->setDouble(jo["f_price"].toDouble());
        ui->tblGoods->lineEdit(row, col_total)->setDouble(jo["f_total"].toDouble());
        ui->tblGoods->getWidget<C5DateEdit>(row, col_valid_date)->setDate(QDate::fromString(jo["f_validdate"].toString(),
                FORMAT_DATE_TO_STR));
        ui->tblGoods->lineEdit(row, col_comment)->setText(jo["f_comment"].toString());
        ui->tblGoods->setString(row, col_adgt, jo["f_adgt"].toString());
    }

    if(fDocType == DOC_TYPE_STORE_INPUT) {
        for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
            ui->tblGoods->setDouble(i, col_remain, doc_remains[QString::number(ui->tblGoods->getInteger(i,
                                    col_goods_id))].toDouble());
        }
    }

    disconnect(ui->leComplectationName, SIGNAL(textChanged(QString)), this,
               SLOT(on_leComplectationName_textChanged(QString)));
    disconnect(ui->leComplectationQty, SIGNAL(textChanged(QString)), this,
               SLOT(on_leComplectationQty_textChanged(QString)));
    ui->leStoreInput->setValue(doc_header_store["f_storein"].toInt());
    ui->leStoreOutput->setValue(doc_header_store["f_storeout"].toInt());
    fBasedOnSale = doc_header_store["f_baseonsale"].toInt();
    ui->lePassed->setValue(doc_header_store["f_passed"].toInt());
    ui->leAccepted->setValue(doc_header_store["f_accepted"].toInt());
    ui->leComplectationCode->setValue(doc_header_store["f_complectation"].toString());
    ui->leComplectationQty->setDouble(doc_header_store["f_complectationqty"].toDouble());
    ui->deInvoiceDate->setDate(QDate::fromString(doc_header_store["f_invoicedate"].toString(), FORMAT_DATE_TO_STR_MYSQL));
    ui->leInvoiceNumber->setText(doc_header_store["f_invoice"].toString());
    fCashDocUuid = doc_header["f_payment"].toString();
    ui->chPaid->setChecked(!fCashDocUuid.isEmpty());

    if(!fCashDocUuid.isEmpty()) {
        on_chPaid_clicked(true);
        C5CashDoc cd;
        cd.openDoc(fCashDocUuid);
        ui->deCashDate->setDate(cd.date());
        ui->leCash->setValue(cd.outputCash());
    } else {
        on_chPaid_clicked(false);
    }

    if(fDocType == DOC_TYPE_COMPLECTATION) {
        db[":f_document"] = id;
        db[":f_type"] = 1;
        db.exec("select d.f_id, d.f_goods, concat(g.f_name, if(g.f_scancode is null, '', concat(' ', g.f_scancode))) as f_goodsname, \
                d.f_qty, u.f_name, d.f_price, d.f_total, d.f_reason, g.f_scancode \
                from a_store_draft d \
                left join c_goods g on g.f_id=d.f_goods \
                left join c_units u on u.f_id=g.f_unit \
                where d.f_document=:f_document and f_type=:f_type");

        if(db.nextRow()) {
            fComplectationId = db.getString("f_id");
            ui->leComplectationCode->setValue(db.getInt("f_goods"));
            ui->leComplectationScancodeCode->setText(db.getString("f_scancode"));
            ui->leComplectationQty->setDouble(db.getDouble("f_qty"));
        } else {
            err = tr("Error in complectation document");
            return false;
        }
    }

    db[":f_header"] = id;
    db.exec("select f_name, f_amount from a_complectation_additions where f_header=:f_header order by f_row");
    ui->tblAdd->setRowCount(0);

    while(db.nextRow()) {
        int row = ui->tblAdd->rowCount();
        ui->tblAdd->setRowCount(row + 1);
        C5LineEdit *l = ui->tblAdd->createLineEdit(row, 1);
        l->setText(db.getString(0));
        l = ui->tblAdd->createLineEdit(row, 2);
        connect(l, SIGNAL(textEdited(QString)), this, SLOT(tblAddChanged(QString)));
        l->setValidator(new QDoubleValidator());
        l->setDouble(db.getDouble(1));
    }

    connect(ui->leComplectationName, SIGNAL(textChanged(QString)), this, SLOT(on_leComplectationName_textChanged(QString)));
    connect(ui->leComplectationQty, SIGNAL(textChanged(QString)), this, SLOT(on_leComplectationQty_textChanged(QString)));
    setDocEnabled(fDocState == DOC_STATE_DRAFT);
    countTotal();

    if(fToolBar && fDocType == sdOutput) {
        fToolBar->addAction(QIcon(":/storeinput.png"), tr("Create store input"), this, SLOT(createStoreInput()));
    }

    return true;
}

void C5StoreDoc::correctDebt()
{
    double amount = ui->leTotal->getDouble() * -1;
    C5Database db;
    C5StoreDraftWriter dw(db);

    if(fDocState == DOC_STATE_SAVED) {
        if(ui->lePartner->getInteger() > 0) {
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
    ui->tblGoods->setColumnHidden(col_remain, sd != DOC_TYPE_STORE_INPUT);

    switch(fDocType) {
    case DOC_TYPE_STORE_INPUT:
        ui->chPaid->setVisible(paymentVisible && true);

        if(fToolBar) {
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

    switch(sd) {
    case sdInput:
        ui->leStoreOutput->setVisible(false);
        ui->leStoreOutputName->setVisible(false);
        ui->lbStoreOutput->setVisible(false);
        ui->leReason->setValue(DOC_REASON_INPUT);
        ui->lePartner->setVisible(true);
        ui->lePartnerName->setVisible(true);
        ui->lbPartner->setVisible(true);
        ui->btnNewPartner->setVisible(true);
        ui->btnFixPartner->setVisible(true);
        ui->btnChangePartner->setVisible(true);
        break;

    case sdOutput:
        ui->leStoreInput->setVisible(false);
        ui->leStoreInputName->setVisible(false);
        ui->btnRememberStoreIn->setVisible(false);
        ui->lePartner->setVisible(true);
        ui->lePartnerName->setVisible(true);
        ui->lePartnerDebt->setVisible(false);
        ui->leInvoiceNumber->setVisible(false);
        ui->deInvoiceDate->setVisible(false);
        ui->lbStoreInput->setVisible(false);
        ui->lbInvoiceDate->setVisible(false);
        ui->lbInvoiceNumber->setVisible(false);
        ui->lbPartner->setVisible(true);
        ui->btnNewPartner->setVisible(false);
        ui->btnFixPartner->setVisible(false);
        ui->leReason->setValue(DOC_REASON_OUT);
        ui->btnChangePartner->setVisible(true);
        break;

    case sdMovement:
        ui->lePartner->setVisible(false);
        ui->lePartnerName->setVisible(false);
        ui->leInvoiceNumber->setVisible(false);
        ui->deInvoiceDate->setVisible(false);
        ui->lbInvoiceDate->setVisible(false);
        ui->lbInvoiceNumber->setVisible(false);
        ui->lbPartner->setVisible(false);
        ui->lePartnerDebt->setVisible(false);
        ui->btnNewPartner->setVisible(false);
        ui->btnFixPartner->setVisible(false);
        ui->leReason->setValue(DOC_REASON_MOVE);
        ui->btnChangePartner->setVisible(false);
        break;

    case sdComplectation:
        ui->lePartner->setVisible(false);
        ui->lePartnerName->setVisible(false);
        ui->leInvoiceNumber->setVisible(false);
        ui->deInvoiceDate->setVisible(false);
        ui->lbInvoiceDate->setVisible(false);
        ui->lbInvoiceNumber->setVisible(false);
        ui->lbPartner->setVisible(false);
        ui->lePartnerDebt->setVisible(false);
        ui->btnNewPartner->setVisible(false);
        ui->btnFixPartner->setVisible(false);
        ui->leReason->setValue(DOC_REASON_COMPLECTATION);
        ui->btnChangePartner->setVisible(false);
        break;

    case sdDeComplectation:
        ui->lePartner->setVisible(false);
        ui->lePartnerName->setVisible(false);
        ui->leInvoiceNumber->setVisible(false);
        ui->deInvoiceDate->setVisible(false);
        ui->lbInvoiceDate->setVisible(false);
        ui->lbInvoiceNumber->setVisible(false);
        ui->lbPartner->setVisible(false);
        ui->lePartnerDebt->setVisible(false);
        ui->btnNewPartner->setVisible(false);
        ui->btnFixPartner->setVisible(false);
        ui->leReason->setValue(DOC_REASON_DECOMPLECTATION);
        ui->btnChangePartner->setVisible(false);
        break;

    default:
        break;
    }

    if(sd != DOC_TYPE_COMPLECTATION && sd != DOC_TYPE_STORE_INPUT) {
        ui->tw->removeTab(1);
    }

    //ui->twCalcPrice->setVisible(sd == DOC_TYPE_STORE_INPUT);
}

void C5StoreDoc::setLastInputPrices()
{
    C5Database db;
    db[":f_id"] = fInternalId;
    db.exec("select b.f_goods, g.f_lastinputprice "
            "from a_store_draft b "
            "left join c_goods g on g.f_id=b.f_goods "
            "where b.f_document=:f_id");

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        for(int j = 0; j < db.rowCount(); j++) {
            if(ui->tblGoods->getInteger(i, col_goods_id) == db.getInt(j, "f_goods")) {
                ui->tblGoods->lineEdit(i, col_price)->setDouble(db.getDouble(j, "f_lastinputprice"));
                ui->tblGoods->lineEdit(i, col_total)->setDouble(ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble() *db.getDouble(j, "f_lastinputprice"));
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

QToolBar* C5StoreDoc::toolBar()
{
    if(!fToolBar) {
        fToolBar = createStandartToolbar(QList<ToolBarButtons>());
        fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDoc()));
        fToolBar->addAction(QIcon(":/draft.png"), tr("Draft"), this, SLOT(draftDoc()));
        fToolBar->addAction(QIcon(":/new.png"), tr("New\ndocument"), this, SLOT(newDoc()));
        fToolBar->addAction(QIcon(":/recycle.png"), tr("Remove"), this, SLOT(removeDocument()));
        fToolBar->addAction(QIcon(":/print.png"), tr("Print"), this, SLOT(printDoc()));
        fToolBar->addAction(QIcon(":/excel.png"), tr("Export\nto Excel"),  this, SLOT(exportToExcel()));
        fToolBar->addAction(QIcon(":/barcode.png"), tr("Print\nbarcode"), this, SLOT(printBarcode()));
        fToolBar->addAction(QIcon(":/goods_store.png"), tr("Duplicate as output"), this, SLOT(duplicateOutput()));
        fToolBar->addAction(QIcon(":/goods_store.png"), tr("Duplicate as input"), this, SLOT(duplicateAsInput()));
    }

    return fToolBar;
}

bool C5StoreDoc::removeDoc(QString id, bool showmessage)
{
    if(showmessage) {
        if(C5Message::question(tr("Confirm to remove document")) != QDialog::Accepted) {
            return false;
        }
    }

    QString err;
    C5Database db;
    QJsonObject jo;
    jo["removeid"] = id;
    db.exec(QString("select sf_remove_store_doc('%1')").arg(json_str(jo)));

    if(db.nextRow()) {
        jo = str_json(db.getString(0));
        return jo["status"] == 1;
    }

    return false;
}

QVariant C5StoreDoc::docProperty(const QString &field) const
{
    if(field == "date") {
        return ui->deDate->date();
    } else if(field == "uuid") {
        return fInternalId;
    } else if(field == "docnumber") {
        if(ui->leDocNum->text().isEmpty()) {
            return ui->leDocNum->placeholderText();
        } else {
            return ui->leDocNum->text();
        }
    } else if(field == "inputstorename") {
        return ui->leStoreInputName->text();
    } else if(field == "outputstorename") {
        return ui->leStoreOutputName->text();
    } else if(field == "typename") {
        int row = C5Cache::cache(cache_doc_type)->find(fDocType);
        return C5Cache::cache(cache_doc_type)->getString(row, 1);
        //        DbStoreDocType dt(fDocType);
        //        return dt.typeName();
    } else if(field == "typeid") {
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
    C5Database db;

    if(ui->chWholeGroup->isChecked()) {
        db[":f_scancode"] = code;
        db.exec("select f_group from c_goods where f_scancode=:f_scancode");

        if(!db.nextRow()) {
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

        while(db.nextRow()) {
            int row = addGoodsRow();
            ui->tblGoods->setInteger(row, col_goods_id, db.getInt("f_code"));
            ui->tblGoods->setString(row, col_goods_name, db.getString("f_goods"));
            ui->tblGoods->lineEdit(row, col_goods_qty)->setDouble(db.getDouble("f_qty"));
            ui->tblGoods->setString(row, col_goods_unit, db.getString("f_unitname"));
            ui->tblGoods->item(row, col_goods_name)->setSelected(true);
            fCanChangeFocus = false;

            if(fDocType == DOC_TYPE_STORE_INPUT) {
                fHttp->createHttpQuery("/engine/worker/check-qty.php", QJsonObject{
                    {"method", "checkStore"}, {"goodsid", ui->tblGoods->getInteger(row, col_goods_id)}
                }, SLOT(slotCheckQtyResponse(QJsonObject)), QVariant(), false);
            }
        }

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

    if(db.nextRow()) {
        int row = addGoodsRow();
        ui->tblGoods->setInteger(row, col_goods_id, db.getInt("f_id"));
        ui->tblGoods->setString(row, col_goods_name, db.getString("f_name"));
        ui->tblGoods->setString(row, col_goods_unit, db.getString("f_unitname"));
        ui->tblGoods->item(row, col_goods_name)->setSelected(true);
        fCanChangeFocus = false;
        ui->tblGoods->lineEdit(row, col_goods_qty)->setText(qty);
        ui->tblGoods->lineEdit(row, col_goods_qty)->setFocus();
        ui->tblGoods->lineEdit(row, col_price)->setText(price);
        ui->tblGoods->lineEdit(row, col_price)->setPlaceholderText(float_str(db.getDouble("f_lastinputprice"), 2));

        if(__c5config.getValue(param_input_doc_fix_price).toInt() > 0) {
            if(price.toDouble() < 0.001) {
                if(db.getDouble("f_lastinputprice") > 0.001) {
                    ui->tblGoods->lineEdit(row, col_price)->setDouble(db.getDouble("f_lastinputprice"));
                    price = db.getString("f_lastinputprice");
                }
            }
        }

        emit ui->tblGoods->lineEdit(row, col_price)->textEdited(price);

        if(fDocType == DOC_TYPE_STORE_INPUT) {
            fHttp->createHttpQuery("/engine/worker/check-qty.php", QJsonObject{
                {"method", "checkStore"}, {"goodsid", ui->tblGoods->getInteger(row, col_goods_id)}
            }, SLOT(slotCheckQtyResponse(QJsonObject)), QVariant(), false);
        }
    } else {
        db[":f_scancode"] = code;
        db.exec("select gg.f_scancode, gg.f_id, concat(gg.f_name, ' ', gg.f_scancode) as f_name, gu.f_name as f_unitname, gg.f_saleprice, "
                "gr.f_taxdept, gr.f_adgcode, gg.f_lastinputprice "
                "from c_goods gg  "
                "left join c_groups gr on gr.f_id=gg.f_group "
                "left join c_units gu on gu.f_id=gg.f_unit "
                "where gg.f_id in (select f_goods from c_goods_multiscancode where f_id=:f_scancode)");

        if(db.nextRow()) {
            int row = addGoodsRow();
            ui->tblGoods->setInteger(row, col_goods_id, db.getInt("f_id"));
            ui->tblGoods->setString(row, col_goods_name, db.getString("f_name"));
            ui->tblGoods->setString(row, col_goods_unit, db.getString("f_unitname"));
            ui->tblGoods->item(row, col_goods_name)->setSelected(true);
            fCanChangeFocus = false;
            ui->tblGoods->lineEdit(row, col_goods_qty)->setText(qty);
            ui->tblGoods->lineEdit(row, col_goods_qty)->setFocus();
            ui->tblGoods->lineEdit(row, col_price)->setText(price);
            ui->tblGoods->lineEdit(row, col_price)->setPlaceholderText(float_str(db.getDouble("f_lastinputprice"), 2));

            if(__c5config.getValue(param_input_doc_fix_price).toInt() > 0) {
                if(price.toDouble() < 0.001) {
                    if(db.getDouble("f_lastinputprice") > 0.001) {
                        ui->tblGoods->lineEdit(row, col_price)->setDouble(db.getDouble("f_lastinputprice"));
                        price = db.getString("f_lastinputprice");
                    }
                }
            }

            emit ui->tblGoods->lineEdit(row, col_price)->textEdited(price);

            if(fDocType == DOC_TYPE_STORE_INPUT) {
                fHttp->createHttpQuery("/engine/worker/check-qty.php", QJsonObject{
                    {"method", "checkStore"}, {"goodsid", ui->tblGoods->getInteger(row, col_goods_id)}
                }, SLOT(slotCheckQtyResponse(QJsonObject)), QVariant(), false);
            }
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
    if(key.toLower() == "ctrl+f") {
        ui->wSearchInDocs->setVisible(true);
    } else {
        C5Document::hotKey(key);
    }
}

bool C5StoreDoc::openDraft(const QString &id, QString &err)
{
    C5Database db;
    db[":f_id"] = id;
    db.exec("select * from a_header where f_id=:f_id");

    if(db.nextRow()) {
        return openDoc(id, err);
    }

    fInternalId = id;
    db[":f_id"] = id;
    db.exec("select * from o_draft_sale where f_id=:f_id");
    db.nextRow();
    ui->deDate->setDate(db.getDate("f_date"));
    ui->leReason->setValue(DOC_REASON_BACK_FROM_PARTNER);
    ui->lePartner->setValue(db.getInt("f_partner"));
    ui->leComment->setText(tr("Back from") + " " + ui->lePartnerName->text());
    db[":f_header"] = id;
    db.exec("select b.f_goods, g.f_name as f_goodsname, b.f_qty, u.f_name as f_unitname, b.f_store,"
            "if(length(coalesce(g.f_adg, '')>0), g.f_adg, gr.f_adgcode) as f_adgt "
            "from o_draft_sale_body b "
            "left join c_goods g on g.f_id=b.f_goods "
            "left join c_units u on u.f_id=g.f_unit "
            "left join c_groups gr on gr.f_id=g.f_group "
            "where b.f_header=:f_header");

    while(db.nextRow()) {
        ui->leStoreInput->setValue(db.getInt("f_store"));
        addGoods(db.getInt("f_goods"), db.getString("f_goodsname"), db.getDouble("f_qty"), db.getString("f_unitname"),
                 0, 0, "", db.getString("f_adgt"));
    }

    return true;
}

void C5StoreDoc::nextChild()
{
    if(fCanChangeFocus) {
        // focusNextChild();
    } else {
        fCanChangeFocus = true;
    }
}

bool C5StoreDoc::writeDocument(int state, QString &err)
{
    if(!docCheck(err, state)) {
        return false;
    }

    if(ui->leDocNum->isEmpty() || (!ui->leDocNum->text().isEmpty() && ui->leDocNum->text().toInt() < 1)) {
        setUserId(true, 0);
        ui->leDocNum->setText(ui->leDocNum->placeholderText());
    } else {
        setUserId(true, ui->leDocNum->getInteger());
    }

    QJsonObject jdoc;
    QJsonObject jheader;
    jheader["f_id"] = fInternalId;
    jheader["f_userid"] = ui->leDocNum->text();
    jheader["f_state"] = state;
    jheader["f_type"] = fDocType;
    jheader["f_date"] = ui->deDate->date().toString(FORMAT_DATE_TO_STR_MYSQL);
    jheader["f_operator"] = mUser->id();
    jheader["f_partner"] = ui->lePartner->getInteger();
    jheader["f_amount"] = ui->leTotal->getDouble();
    jheader["f_currency"] = ui->cbCurrency->currentData().toInt();
    jheader["f_storein"] = ui->leStoreInput->getInteger();
    jheader["f_storeout"] = ui->leStoreOutput->getInteger();
    jheader["f_reason"] = ui->leReason->getInteger();
    jheader["f_comment"] = ui->leComment->text();
    jheader["f_payment"] = state == DOC_STATE_SAVED ?  QJsonValue(fCashDocUuid) : QJsonValue::Null;
    jheader["f_paid"] = ui->chPaid->isChecked() ? 1 : 0;
    jdoc["header"] = jheader;
    QJsonObject jbody;
    jbody["f_accepted"] = ui->leAccepted->getInteger();
    jbody["f_passed"] = ui->lePassed->getInteger();
    jbody["f_invoice"] = ui->leInvoiceNumber->text();
    jbody["f_invoicedate"] = ui->deInvoiceDate->date().toString(FORMAT_DATE_TO_STR_MYSQL);
    jbody["f_reason"] = ui->leReason->getInteger();
    jbody["f_storein"] = ui->leStoreInput->getInteger();
    jbody["f_storeout"] = ui->leStoreOutput->getInteger();
    jbody["f_complectationcode"] = ui->leComplectationCode->getInteger();
    jbody["f_complectationqty"] = ui->leComplectationQty->getDouble();
    jbody["f_cashdoc"] = fCashDocUuid;
    jbody["f_basedonsale"] = fBasedOnSale;
    QJsonArray jadd;

    for(int i = 0; i < ui->tblAdd->rowCount(); i++) {
        QJsonObject jo;
        jo["f_name"] = ui->tblAdd->lineEdit(i, 1)->text();
        jo["f_amount"] = ui->tblAdd->lineEdit(i, 2)->getDouble();
        jo["f_row"] = i + 1;
        jadd.append(jo);
    }

    jdoc["add"] = jadd;
    QJsonObject jcomplect;
    jcomplect["f_goods"] = ui->leComplectationCode->getInteger();
    jcomplect["f_qty"] = ui->leComplectationQty->getDouble();
    jdoc["complect"] = jcomplect;
    QJsonArray jgoods;
    QJsonObject jremains;

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        QJsonObject jitem;
        jitem["f_goods"] = ui->tblGoods->getInteger(i, col_goods_id);
        jitem["f_qty"] = ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble();
        jitem["f_price"] = ui->tblGoods->lineEdit(i, col_price)->getDouble();
        jitem["f_validto"] = ui->tblGoods->getWidget<C5DateEdit>(i, col_valid_date)->date().toString(FORMAT_DATE_TO_STR_MYSQL);
        jitem["f_comment"] = ui->tblGoods->lineEdit(i, col_comment)->text();
        jitem["f_row"] = i;
        jgoods.append(jitem);
        jremains[QString::number(ui->tblGoods->getInteger(i, col_goods_id))] = ui->tblGoods->getDouble(i, col_remain);
    }

    jbody["remains"] = jremains;
    jdoc["goods"] = jgoods;
    QJsonObject jpartner;
    jpartner["partner"] = ui->lePartner->getInteger();
    jpartner["paid"] = ui->chPaid->isChecked() ? 1 : 0;
    jpartner["cash"] = ui->leCash->getInteger();
    jdoc["partner"] = jpartner;
    jdoc["body"] = jbody;
    QString session = C5Database::uuid();
    jdoc["session"] = session;
    QJsonDocument json(jdoc);
    C5Database db;

    if(!db.execNetwork(QString("call sf_create_store_document('%1')").arg(QString(json.toJson(QJsonDocument::Compact))))) {
        err = db.fLastError;
        return false;
    }

    db[":f_session"] = session;
    db.exec("select f_result from a_result where f_session=:f_session");

    if(db.nextRow()) {
        QJsonObject js = QJsonDocument::fromJson(db.getString(0).toUtf8()).object();

        if(js["status"].toInt() == 0) {
            err = js["message"].toString();
            return false;
        }
    } else {
        err = db.fLastError;
        return false;
    }

    setDocEnabled(state == DOC_STATE_DRAFT);
    fDocState = state;
    return true;
}

void C5StoreDoc::setUserId(bool withUpdate, int value)
{
    if(!ui->leDocNum->text().isEmpty() && ui->leDocNum->text().toInt() > 0) {
        return;
    }

    int storeId = 0;

    switch(fDocType) {
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

    if(storeId == 0) {
        ui->leDocNum->setPlaceholderText("");
        return;
    }

    C5Database db;
    C5StoreDraftWriter dw(db);
    ui->leDocNum->setPlaceholderText(dw.storeDocNum(fDocType, storeId, withUpdate, value));
}

void C5StoreDoc::correctDishesRows(int row, int count)
{
    for(int i = row; i < ui->tblDishes->rowCount(); i++) {
        QJsonArray ja = QJsonDocument::fromJson(ui->tblDishes->getString(i, 4).toUtf8()).array();

        for(int j = 0; j < ja.count(); j++) {
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

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        total += ui->tblGoods->lineEdit(i, col_total)->getDouble();
        totalQty += ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble();
    }

    for(int i = 0; i < ui->tblAdd->rowCount(); i++) {
        total += ui->tblAdd->lineEdit(i, 2)->getDouble();
    }

    ui->leTotal->setDouble(total);
    ui->leTotalQty->setDouble(totalQty);
}

void C5StoreDoc::countQtyOfComplectation()
{
    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        ui->tblGoods->lineEdit(i, col_goods_qty)->setDouble((fBaseQtyOfComplectation[i] *ui->leComplectationQty->getDouble()) /
            fBaseComplectOutput);
    }
}

bool C5StoreDoc::docCheck(QString &err, int state)
{
    rowsCheck(err);

    switch(fDocType) {
    case sdInput:
        ui->leStoreOutput->setValue(0);

        if(ui->leStoreInput->getInteger() == 0) {
            err += tr("Input store is not defined") + "<br>";
        }

        if(ui->chPaid->isChecked() && ui->leCash->getInteger() == 0) {
            err += tr("Cash must be selected") + "<br>";
        }

        if(state == 1 && ui->lePartner->getInteger() == 0) {
            err += tr("Partner not specified");
        }

        break;

    case sdOutput:
        ui->leStoreInput->setValue(0);

        if(ui->leStoreOutput->getInteger() == 0) {
            err += tr("Output store is not defined") + "<br>";
        }

        break;

    case sdMovement:
        if(ui->leStoreInput->getInteger() == 0) {
            err += tr("Input store is not defined") + "<br>";
        }

        if(ui->leStoreOutput->getInteger() == 0) {
            err += tr("Output store is not defined") + "<br>";
        }

        if(ui->leStoreInput->getInteger() == ui->leStoreOutput->getInteger() && ui->leStoreInput->getInteger() != 0) {
            err += tr("Input store and output store cannot be same") + "<br>";
        }

        break;

    case sdComplectation:
        if(ui->leStoreInput->getInteger() == 0) {
            err += tr("Input store is not defined") + "<br>";
        }

        if(ui->leStoreOutput->getInteger() == 0) {
            err += tr("Output store is not defined") + "<br>";
        }

        if(ui->leComplectationQty->getDouble() < 0.00001) {
            err += tr("The quantity of complectation cannot be zero") + "<br>";
        }

        if(ui->leComplectationCode->getInteger() == 0) {
            err += tr("The goods of input cannot be unknown") + "<br>";
        }

        break;
    }

    if(ui->tblGoods->rowCount() == 0) {
        err += tr("Cannot save an emtpy document") + "<br>";
    }

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if(ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble() < 0.00001) {
            err += QString("%1 #%2, %3, %4").arg(tr("Row")).arg(i + 1).arg(ui->tblGoods->getString(i, col_goods_name)).arg(tr("missing quantity")) + "<br>";
        }
    }

    if(ui->leReason->getInteger() == 0) {
        err += tr("The reason of document cannot be empty");
    }

    if(ui->chPaid->isChecked()) {
        if(ui->lePartner->getInteger() == 0) {
            err += tr("Supplier not specified") + "<br>";
        }
    }

    return err.isEmpty();
}

void C5StoreDoc::rowsCheck(QString &err)
{
    if(ui->tblGoods->rowCount() == 0) {
        err += tr("Empty document") + "<br>";
        return;
    }

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if(ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble() < 0.0001) {
            err += tr("Row #") + QString::number(i + 1) + tr(" empty qty") + "<br>";
        }
    }
}

void C5StoreDoc::writeDocumentWithState(int state)
{
    QString err;
    writeDocument(state, err);

    if(!err.isEmpty()) {
        C5Message::error(err);
        return;
    }

    C5Database db;

    if(err.isEmpty()) {
        if(property("fromdraft").toString().isEmpty() == false) {
            db[":f_id"] = fInternalId;
            db[":f_state"] = 6;
            db.exec("update o_draft_sale set f_state=:f_state where f_id=:f_id");
        }

#pragma message( " DONT FORGET DO THIS FOR STORE UPDATE IN SHOP")
#pragma message( " DONT FORGET DO THIS FOR STORE UPDATE IN SHOP")
#pragma message( " DONT FORGET DO THIS FOR STORE UPDATE IN SHOP")
#pragma message( " DONT FORGET DO THIS FOR STORE UPDATE IN SHOP")
#pragma message( " DONT FORGET DO THIS FOR STORE UPDATE IN SHOP")
#pragma message( " DONT FORGET DO THIS FOR STORE UPDATE IN SHOP")
        C5Message::info(tr("Saved"));

        if(fDocType == DOC_TYPE_STORE_OUTPUT
                || fDocType == DOC_TYPE_STORE_MOVE
                || fDocType == DOC_TYPE_COMPLECTATION) {
            openDoc(fInternalId, err);
        }
    } else {
        if(fDocType == DOC_TYPE_STORE_OUTPUT) {
            if(ui->btnAutoDecrease->isChecked()) {
                QStringList rows = err.split("\n");
                QMap<QString, double> decrease;

                for(const QString &s : rows) {
                    QStringList r1 = s.split(" -: ", Qt::SkipEmptyParts);

                    if(r1.count() == 2) {
                        decrease[r1.at(0).trimmed()] = decrease[r1.at(0).trimmed()] + r1.at(1).toDouble();
                    }
                }

                for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
                    QString gname = ui->tblGoods->getString(i, col_goods_name).trimmed();

                    if(decrease.contains(gname)) {
                        ui->tblGoods->lineEdit(i, col_goods_qty)->setDouble(ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble() - decrease[gname]);
                    }
                }
            }
        }

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
    C5LineEdit *lqty = ui->tblGoods->createLineEdit(row, col_goods_qty);
    lqty->setValidator(new QDoubleValidator(0, 1000000, 4));
    lqty->fDecimalPlaces = 4;
    lqty->addEventKeys("+-*");
    connect(lqty, SIGNAL(keyPressed(QChar)), this, SLOT(lineEditKeyPressed(QChar)));
    connect(lqty, SIGNAL(returnPressed()), this, SLOT(focusNextChildren()));
    ui->tblGoods->setItem(row, col_goods_unit, new QTableWidgetItem());
    C5LineEdit *lprice = ui->tblGoods->createLineEdit(row, col_price);
    lprice->setValidator(new QDoubleValidator(0, 100000000, col_goods_id));
    lprice->fDecimalPlaces = 3;
    lprice->addEventKeys("+-*");
    connect(lprice, SIGNAL(keyPressed(QChar)), this, SLOT(lineEditKeyPressed(QChar)));
    connect(lprice, SIGNAL(returnPressed()), this, SLOT(focusNextChildren()));
    C5LineEdit *ltotal = ui->tblGoods->createLineEdit(row, col_total);
    ltotal->setValidator(new QDoubleValidator(0, 100000000, 2));
    ltotal->addEventKeys("+-*");
    connect(ltotal, SIGNAL(keyPressed(QChar)), this, SLOT(lineEditKeyPressed(QChar)));
    connect(ltotal, SIGNAL(returnPressed()), this, SLOT(focusNextChildren()));
    ui->tblGoods->createWidget<C5DateEdit>(row, col_valid_date);
    ui->tblGoods->createLineEdit(row, col_comment);
    connect(lqty, SIGNAL(textEdited(QString)), this, SLOT(tblQtyChanged(QString)));
    connect(lprice, SIGNAL(textEdited(QString)), this, SLOT(tblPriceChanged(QString)));
    connect(ltotal, SIGNAL(textEdited(QString)), this, SLOT(tblTotalChanged(QString)));
    ui->tblGoods->setItem(row, col_adgt, new QTableWidgetItem());
    return row;
}

int C5StoreDoc::addGoods(int goods, const QString &name, double qty, const QString &unit, double price, double total,
                         const QString &comment, const QString &adgt)
{
    int row = addGoodsRow();
    ui->tblGoods->setData(row, col_goods_id, goods);
    ui->tblGoods->setData(row, col_goods_name, name);
    ui->tblGoods->setData(row, col_goods_unit, unit);

    if(qty > 0) {
        ui->tblGoods->lineEdit(row, col_goods_qty)->setDouble(qty);
    }

    if(price > 0) {
        ui->tblGoods->lineEdit(row, col_price)->setDouble(price);
        ui->tblGoods->lineEdit(row, col_total)->setDouble(total);
    }

    ui->tblGoods->lineEdit(row, col_comment)->setText(comment);
    ui->tblGoods->setString(row, col_adgt, adgt);
    ui->tblGoods->setCurrentCell(row, 0);
    return row;
}

void C5StoreDoc::setDocEnabled(bool v)
{
    ui->deDate->setEnabled(v && mUser->check(cp_t1_allow_change_store_doc_date));
    ui->leStoreInput->setEnabled(v);
    ui->leStoreOutput->setEnabled(v);
    ui->wtoolbar->setEnabled(v);

    for(int r = 0; r < ui->tblGoods->rowCount(); r++) {
        for(int c = 0; c < ui->tblGoods->columnCount(); c++) {
            C5LineEdit *l = dynamic_cast<C5LineEdit*>(ui->tblGoods->cellWidget(r, c));

            if(l) {
                l->setEnabled(v);
            }
        }
    }

    for(int r = 0; r < ui->tblAdd->rowCount(); r++) {
        for(int c = 1; c < ui->tblAdd->columnCount(); c++) {
            C5LineEdit *l = dynamic_cast<C5LineEdit*>(ui->tblAdd->cellWidget(r, c));

            if(l) {
                l->setEnabled(v);
            }
        }
    }

    if(fToolBar) {
        fToolBar->actions().at(1)->setEnabled(!v);
        fToolBar->actions().at(0)->setEnabled(v);
    }

    ui->grComplectation->setEnabled(v);
    ui->btnChangePartner->setEnabled(!v);
}

void C5StoreDoc::addGoodsByCalculation(int goods, const QString &name, double qty)
{
    QJsonArray ja;
    C5Database db;
    db[":f_dish"] = goods;
    db.exec("select f_goods, f_qty from d_recipes where f_dish=:f_dish");
    int r;
    auto *c = C5Cache::cache(cache_goods);

    while(db.nextRow()) {
        QJsonArray vg = c->getValuesForId(db.getInt("f_goods"));
        r = -1;

        for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
            if(ui->tblGoods->getInteger(i, col_goods_id) == db.getInt("f_goods")) {
                r = i;
                break;
            }
        }

        if(r < 0) {
            r = addGoodsRow();
        }

        ui->tblGoods->setString(r, col_goods_id, vg.at(0).toString());
        ui->tblGoods->setString(r, col_goods_name, vg.at(2).toString());
        ui->tblGoods->setString(r, col_goods_unit, vg.at(3).toString());
        ui->tblGoods->lineEdit(r, col_price)->setPlaceholderText(float_str(vg.at(5).toDouble(), 2));
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

QString C5StoreDoc::makeGoodsTableHtml(const QStringList &headers,
                                       const QList<QStringList>& rows,
                                       const QSet<int>& rightCols)
{
    QString h;
    QTextStream s(&h);
    s << "<table>";
    s << "<thead><tr>";

    for(int i = 0; i < headers.size(); ++i) {
        s << "<th>" << htmlEscape(headers[i]) << "</th>";
    }

    s << "</tr></thead>";
    s << "<tbody>";

    for(const auto &r : rows) {
        s << "<tr>";

        for(int c = 0; c < headers.size(); ++c) {
            const QString cell = (c < r.size() ? r[c] : "");
            const bool isRight = rightCols.contains(c);
            s << "<td" << (isRight ? " class='right'" : "") << ">"
              << htmlEscape(cell)
              << "</td>";
        }

        s << "</tr>";
    }

    s << "</tbody></table>";
    return h;
}

QString C5StoreDoc::makeOtherChargesHtml(C5TableWidget *tbl, const QStringList &hdr)
{
    QString h;
    QTextStream s(&h);
    s << "<div class='mt10'><table>";
    s << "<thead><tr>";

    for(auto &x : hdr)
        s << "<th>" << htmlEscape(x) << "</th>";

    s << "</tr></thead><tbody>";

    for(int i = 0; i < tbl->rowCount(); ++i) {
        s << "<tr>";
        s << "<td class='center'>" << (i + 1) << "</td>";
        s << "<td>" << htmlEscape(tbl->getString(i, 1)) << "</td>";
        s << "<td class='right'>" << htmlEscape(float_str(tbl->lineEdit(i, 2)->getDouble(), 2)) << "</td>";
        s << "</tr>";
    }

    s << "</tbody></table></div>";
    return h;
}

QString C5StoreDoc::makeComplectationInputHtml(const C5LineEdit *code,
        const C5LineEdit *name,
        const C5LineEdit *qty,
        double total,
        double qtyVal,
        const QStringList &hdr)
{
    const double unitPrice = (qtyVal != 0.0 ? (total / qtyVal) : 0.0);
    QString h;
    QTextStream s(&h);
    s << "<div class='mt10'><table>";
    s << "<thead><tr>";

    for(auto &x : hdr)
        s << "<th>" << htmlEscape(x) << "</th>";

    s << "</tr></thead><tbody>";
    s << "<tr>";
    s << "<td class='center'>1</td>";
    s << "<td>" << htmlEscape(code->text()) << "</td>";
    s << "<td>" << htmlEscape(name->text()) << "</td>";
    s << "<td class='right'>" << htmlEscape(qty->text()) << "</td>";
    s << "<td class='right'>" << htmlEscape(float_str(unitPrice, 2)) << "</td>";
    s << "<td class='right'>" << htmlEscape(float_str(total, 2)) << "</td>";
    s << "</tr>";
    s << "</tbody></table></div>";
    return h;
}

void C5StoreDoc::printV1()
{
    QString tpl = loadTemplate("store_doc_v1.html");

    if(tpl.isEmpty()) {
        C5Message::error(tr("Template not found: store_doc_v1.html"));
        return;
    }

    // 1) Заголовок (docTypeText)
    QString docTypeText;

    switch(fDocType) {
    case DOC_TYPE_STORE_INPUT:    docTypeText = tr("Store input"); break;

    case DOC_TYPE_STORE_OUTPUT:   docTypeText = tr("Store output"); break;

    case DOC_TYPE_STORE_MOVE:     docTypeText = tr("Store movement"); break;

    case DOC_TYPE_COMPLECTATION:  docTypeText = tr("Store complectation"); break;

    default: docTypeText = tr("Store document"); break;
    }

    // 2) Stores
    QString storeInName, storeOutName;

    if(ui->leStoreInput->getInteger() > 0)
        storeInName  = ui->leStoreInputName->text();

    if(ui->leStoreOutput->getInteger() > 0)
        storeOutName = ui->leStoreOutputName->text();

    // 3) Комментарий
    QString commentBlock;

    if(!ui->leComment->isEmpty()) {
        commentBlock = "<div class='note'>" + htmlEscape(ui->leComment->text()) + "</div>";
    }

    // 4) Header (date/stores) — динамические колонки
    QString thStoreIn, tdStoreIn, thStoreOut, tdStoreOut;

    if(!storeInName.isEmpty()) {
        thStoreIn = "<th>" + htmlEscape(tr("Store, input")) + "</th>";
        tdStoreIn = "<td>" + htmlEscape(storeInName) + "</td>";
    }

    if(!storeOutName.isEmpty()) {
        thStoreOut = "<th>" + htmlEscape(tr("Store, output")) + "</th>";
        tdStoreOut = "<td>" + htmlEscape(storeOutName) + "</td>";
    }

    // 5) Supplier/Invoice block — как у тебя через tableText(points)
    QString supplierInvoiceBlock;
    {
        bool hasSupplier = ui->lePartner->getInteger() > 0;
        bool hasInvoice = !ui->leInvoiceNumber->isEmpty();

        if(hasSupplier || hasInvoice) {
            QString t;
            QTextStream s(&t);
            s << "<table class='mt6'>"
              << "<tr>";

            if(hasSupplier)
                s << "<th style='width:55%'>" << htmlEscape(tr("Supplier")) << "</th>";

            if(hasInvoice)
                s << "<th>" << htmlEscape(tr("Purchase document")) << "</th>";

            s << "</tr><tr>";

            if(hasSupplier)
                s << "<td>" << htmlEscape(ui->lePartnerName->text()) << "</td>";

            if(hasInvoice)
                s << "<td>" << htmlEscape(ui->leInvoiceNumber->text() + " / " + ui->deInvoiceDate->text()) << "</td>";

            s << "</tr></table>";
            supplierInvoiceBlock = t;
        } else {
            supplierInvoiceBlock = "";
        }
    }
    // 6) Complectation blocks (если тип = комплектация)
    QString complectationBlock;

    if(fDocType == DOC_TYPE_COMPLECTATION) {
        QStringList hdrCharges { tr("NN"), tr("Other charges"), tr("Amount") };
        QString charges = makeOtherChargesHtml(ui->tblAdd, hdrCharges);
        QStringList hdrInput { tr("NN"), tr("Code"), tr("Input material"), tr("Qty"), tr("Price"), tr("Amount") };
        QString inputMat = makeComplectationInputHtml(
                               ui->leComplectationCode,
                               ui->leComplectationName,
                               ui->leComplectationQty,
                               ui->leTotal->getDouble(),
                               ui->leComplectationQty->getDouble(),
                               hdrInput
                           );
        complectationBlock = charges + inputMat;
    }

    // 7) Формируем goodsGroupData (оставляем твою логику 1:1, только без печати)
    QMap<int, int> goodsGroupMap;
    QList<QStringList> goodsGroupData;

    for(int i = 0; i < ui->tblGoods->rowCount(); ++i) {
        QStringList val  = {"", "", "", "0", "", "", "0", ""}; // [NN, code, name, qty, unit, price, total, remain]

        if(ui->chLeaveFocusOnBarcode->isChecked()) {
            int key = ui->tblGoods->getInteger(i, col_goods_id);

            if(goodsGroupMap.contains(key)) {
                val = goodsGroupData[goodsGroupMap[key]];
            } else {
                val[0] = QString::number(goodsGroupData.count() + 1);
            }
        } else {
            val[0] = QString::number(i + 1);
        }

        val[1] = ui->tblGoods->getString(i, col_goods_id); // material code?
        val[2] = ui->tblGoods->getString(i, col_goods_name); // goods name

        if(ui->chLeaveFocusOnBarcode->isChecked()) {
            val[3] = float_str(str_float(val[3]) + ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble(), 3);
        } else {
            val[3] = ui->tblGoods->lineEdit(i, col_goods_qty)->text();
        }

        val[4] = ui->tblGoods->getString(i, col_goods_unit); // unit
        val[5] = "0";
        val[6] = "0";
        // !!! у тебя потом price пересчитывается из total/qty:
        val[5] = float_str(str_float(val[6]) / qMax(0.000001, str_float(val[3])), 2);
        val[7] = float_str(ui->tblGoods->getDouble(i, col_remain), 2);

        if(ui->chLeaveFocusOnBarcode->isChecked()) {
            int key = ui->tblGoods->getInteger(i, col_goods_id);

            if(goodsGroupMap.contains(key)) {
                goodsGroupData[goodsGroupMap[key]] = val;
            } else {
                goodsGroupData.append(val);
                goodsGroupMap[key] = goodsGroupData.count() - 1;
            }
        } else {
            goodsGroupData.append(val);
        }
    }

    // 8) Заголовок колонки Goods зависит от типа документа
    QString goodsColName = tr("Goods");

    if(fDocType == DOC_TYPE_COMPLECTATION) {
        goodsColName = tr("Output material");
    }

    // 9) Goods table HTML
    QStringList goodsHdr {
        tr("NN"),
        tr("Material code"),
        goodsColName,
        tr("Qty"),
        tr("Unit"),
        tr("Price"),
        tr("Total"),
        tr("Remain")
    };
    // right align: Qty(3), Price(5), Total(6), Remain(7)
    QSet<int> rightCols {3, 5, 6, 7};
    QString goodsTable = makeGoodsTableHtml(goodsHdr, goodsGroupData, rightCols);
    // 10) Подписи (в зависимости от типа)
    QString signLeftTitle, signRightTitle, signLeftName, signRightName;

    if(fDocType == DOC_TYPE_STORE_MOVE) {
        signLeftTitle  = tr("Passed");
        signRightTitle = tr("Accepted");
        signLeftName   = ui->lePassedName->text();
        signRightName  = ui->leAcceptedName->text();
    } else {
        signLeftTitle  = tr("Accepted");
        signRightTitle = tr("Passed");
        signLeftName   = ui->leAcceptedName->text();
        signRightName  = ui->lePassedName->text();
    }

    // 11) Переменные шаблона
    QMap<QString, QString> v;
    v["doc_title"] = htmlEscape(QString("%1 N%2").arg(docTypeText, ui->leDocNum->text()));
    v["reason"] = htmlEscape(ui->leReasonName->text());
    v["comment_block"] = commentBlock;
    v["lbl_date"] = htmlEscape(tr("Date"));
    v["date"] = htmlEscape(ui->deDate->text());
    v["th_store_in"] = thStoreIn;
    v["td_store_in"] = tdStoreIn;
    v["th_store_out"] = thStoreOut;
    v["td_store_out"] = tdStoreOut;
    v["supplier_invoice_block"] = supplierInvoiceBlock;
    v["complectation_block"] = complectationBlock;
    v["goods_table"] = goodsTable;
    v["lbl_total_amount"] = htmlEscape(tr("Total amount"));
    v["total_amount"] = htmlEscape(ui->leTotal->text());
    v["sign_left_title"] = htmlEscape(signLeftTitle);
    v["sign_right_title"] = htmlEscape(signRightTitle);
    v["sign_left_name"] = htmlEscape(signLeftName);
    v["sign_right_name"] = htmlEscape(signRightName);
    QString html = applyTemplate(tpl, v);
    // 12) Preview/Print
    QTextDocument doc;
    doc.setHtml(html);
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize::A4);
    printer.setFullPage(false);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, [&](QPrinter * prn) { doc.print(prn); });
    preview.exec();
}

void C5StoreDoc::printV2()
{
    const QString tplSheet = loadTemplate("store_doc_v2_two_up.html");
    const QString tplOne   = loadTemplate("store_doc_v2_one_copy.html");

    if(tplSheet.isEmpty() || tplOne.isEmpty()) {
        C5Message::error(tr("Template not found"));
        return;
    }

    // ---- docTypeText
    QString docTypeText;

    switch(fDocType) {
    case DOC_TYPE_STORE_INPUT:    docTypeText = tr("Store input"); break;

    case DOC_TYPE_STORE_OUTPUT:   docTypeText = tr("Store output"); break;

    case DOC_TYPE_STORE_MOVE:     docTypeText = tr("Store movement"); break;

    case DOC_TYPE_COMPLECTATION:  docTypeText = tr("Store complectation"); break;

    default: docTypeText = tr("Store document"); break;
    }

    // ---- stores
    QString storeInName, storeOutName;

    if(ui->leStoreInput->getInteger() > 0)
        storeInName  = ui->leStoreInputName->text();

    if(ui->leStoreOutput->getInteger() > 0)
        storeOutName = ui->leStoreOutputName->text();

    // ---- comment
    QString commentBlock;

    if(!ui->leComment->isEmpty()) {
        commentBlock = "<div class='note'>" + htmlEscape(ui->leComment->text()) + "</div>";
    }

    // ---- dynamic store columns
    QString thStoreIn, tdStoreIn, thStoreOut, tdStoreOut;

    if(!storeInName.isEmpty()) {
        thStoreIn = "<th>" + htmlEscape(tr("Store, input")) + "</th>";
        tdStoreIn = "<td>" + htmlEscape(storeInName) + "</td>";
    }

    if(!storeOutName.isEmpty()) {
        thStoreOut = "<th>" + htmlEscape(tr("Store, output")) + "</th>";
        tdStoreOut = "<td>" + htmlEscape(storeOutName) + "</td>";
    }

    // ---- supplier/invoice block
    QString supplierInvoiceBlock;
    {
        bool hasSupplier = ui->lePartner->getInteger() > 0;
        bool hasInvoice  = !ui->leInvoiceNumber->isEmpty();

        if(hasSupplier || hasInvoice) {
            QString t;
            QTextStream s(&t);
            s << "<table class='mt6'><tr>";

            if(hasSupplier)
                s << "<th style='width:55%'>" << htmlEscape(tr("Supplier")) << "</th>";

            if(hasInvoice)
                s << "<th>" << htmlEscape(tr("Purchase document")) << "</th>";

            s << "</tr><tr>";

            if(hasSupplier)
                s << "<td>" << htmlEscape(ui->lePartnerName->text()) << "</td>";

            if(hasInvoice)
                s << "<td>" << htmlEscape(ui->leInvoiceNumber->text() + " / " + ui->deInvoiceDate->text()) << "</td>";

            s << "</tr></table>";
            supplierInvoiceBlock = t;
        }
    }
    // ---- complectation block (если надо)
    QString complectationBlock;

    if(fDocType == DOC_TYPE_COMPLECTATION) {
        QStringList hdrCharges { tr("NN"), tr("Other charges"), tr("Amount") };
        QString charges = makeOtherChargesHtml(ui->tblAdd, hdrCharges);
        QStringList hdrInput { tr("NN"), tr("Code"), tr("Input material"), tr("Qty"), tr("Price"), tr("Amount") };
        QString inputMat = makeComplectationInputHtml(
                               ui->leComplectationCode,
                               ui->leComplectationName,
                               ui->leComplectationQty,
                               ui->leTotal->getDouble(),
                               ui->leComplectationQty->getDouble(),
                               hdrInput
                           );
        complectationBlock =
            "<div class='section-title'>" + htmlEscape(tr("Complectation")) + "</div>" +
            charges + inputMat;
    }

    // ---- goods table
    QString goodsColName = tr("Goods");

    if(fDocType == DOC_TYPE_COMPLECTATION) {
        goodsColName = tr("Output material");
    }

    // ВАЖНО: V2 печатает из ui->tblGoods построчно, без группировки по barcode как V1.
    // Оставим ту же логику, что была в V2.
    QList<QStringList> rows;

    for(int i = 0; i < ui->tblGoods->rowCount(); ++i) {
        QStringList r;
        r << QString::number(i + 1);
        r << ui->tblGoods->getString(i, col_goods_id);
        r << ui->tblGoods->getString(i, col_goods_name);
        r << ui->tblGoods->lineEdit(i, col_goods_qty)->text();
        r << ui->tblGoods->getString(i, col_goods_unit);
        r << "0";
        r << "0";
        rows << r;
    }

    QStringList hdr {
        tr("NN"),
        tr("Material code"),
        goodsColName,
        tr("Qty"),
        tr("Unit"),
        tr("Price"),
        tr("Total")
    };
    // right align: Qty(3), Price(5), Total(6)
    QSet<int> rightCols {3, 5, 6};
    QString goodsTable = makeGoodsTableHtml(hdr, rows, rightCols);
    // ---- signatures
    QString signLeftTitle, signRightTitle, signLeftName, signRightName;

    if(fDocType == DOC_TYPE_STORE_MOVE) {
        signLeftTitle  = tr("Passed");
        signRightTitle = tr("Accepted");
        signLeftName   = ui->lePassedName->text();
        signRightName  = ui->leAcceptedName->text();
    } else {
        signLeftTitle  = tr("Accepted");
        signRightTitle = tr("Passed");
        signLeftName   = ui->leAcceptedName->text();
        signRightName  = ui->lePassedName->text();
    }

    // ---- vars for one copy
    QMap<QString, QString> v;
    v["doc_title"] = htmlEscape(QString("%1 N%2").arg(docTypeText, ui->leDocNum->text()));
    v["reason"] = htmlEscape(ui->leReasonName->text());
    v["comment_block"] = commentBlock;
    v["lbl_date"] = htmlEscape(tr("Date"));
    v["date"] = htmlEscape(ui->deDate->text());
    v["th_store_in"] = thStoreIn;
    v["td_store_in"] = tdStoreIn;
    v["th_store_out"] = thStoreOut;
    v["td_store_out"] = tdStoreOut;
    v["supplier_invoice_block"] = supplierInvoiceBlock;
    v["complectation_block"] = complectationBlock;
    v["goods_table"] = goodsTable;
    v["lbl_total_amount"] = htmlEscape(tr("Total amount"));
    v["total_amount"] = htmlEscape(ui->leTotal->text());
    v["sign_left_title"] = htmlEscape(signLeftTitle);
    v["sign_right_title"] = htmlEscape(signRightTitle);
    v["sign_left_name"] = htmlEscape(signLeftName);
    v["sign_right_name"] = htmlEscape(signRightName);
    const QString oneCopyHtml = applyTemplate(tplOne, v);
    // ---- sheet vars
    QMap<QString, QString> svars;
    svars["one_copy"] = oneCopyHtml;
    const QString html = applyTemplate(tplSheet, svars);
    // ---- preview/print
    QTextDocument doc;
    doc.setHtml(html);
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize::A4);
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setFullPage(false);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested,
    [&](QPrinter * p) { doc.print(p); });
    preview.exec();
}

double C5StoreDoc::additionalCost()
{
    double c = 0;

    for(int i = 0; i < ui->tblAdd->rowCount(); i++) {
        c += ui->tblAdd->lineEdit(i, 2)->getDouble();
    }

    return c;
}

double C5StoreDoc::additionalCostForEveryGoods()
{
    if(ui->leTotalQty->getDouble() > 0) {
        return additionalCost() / ui->leTotalQty->getDouble();
    }

    return 0;
}

void C5StoreDoc::selectorCallback(int row, const QJsonArray &values)
{
    switch(row) {
    case cache_goods_partners:
        C5Database db;
        db[":f_costumer"] = values.at(0).toInt();
        db.exec(R"(
                    SELECT GROUP_CONCAT(round(f_amount, 0), ' ', f_short SEPARATOR ', ') as f_amounts
                    FROM (
                    SELECT  c.f_short, sum(f_amount*if(f_source=1, 1, -1)) AS f_amount
                    from b_clients_debts b
                    inner JOIN e_currency c ON c.f_id=b.f_currency
                    where f_costumer=:f_costumer
                    GROUP BY 1)
                    AS sub
                )");
        db.nextRow();
        ui->lePartnerDebt->setText(db.getString("f_amounts"));
        break;
    }
}

void C5StoreDoc::lineEditKeyPressed(const QChar &key)
{
    switch(key.toLatin1()) {
    case '+':
        on_btnAddGoods_clicked();
        break;

    case '-':
        on_btnRemoveGoods_clicked();
        break;

    case '*':
        double v;

        if(Calculator::get(v, mUser)) {
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

void C5StoreDoc::changeCurrencyResponse(const QJsonObject &jdoc)
{
    Q_UNUSED(jdoc);
    fHttp->httpQueryFinished(sender());
}

void C5StoreDoc::slotCheckQtyResponse(const QJsonObject &jdoc)
{
    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if(ui->tblGoods->getInteger(i, col_goods_id) == jdoc["goodsid"].toInt()) {
            ui->tblGoods->setDouble(i, col_remain, jdoc["remain"].toDouble());
        }
    }
}

void C5StoreDoc::exportToExcel()
{
    QString docTypeText;

    switch(fDocType) {
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

    if(ui->leStoreInput->getInteger() > 0) {
        storeInName = ui->leStoreInputName->text();
    }

    if(ui->leStoreOutput->getInteger() > 0) {
        storeOutName = ui->leStoreOutputName->text();
    }

    QXlsx::Document d;
    d.addSheet("Sheet1");
    /* HEADER */
    QColor color = Qt::white;
    QFont headerFont(qApp->font());
    QXlsx::Format hf;
    hf.setFont(headerFont);
    hf.setFontColor(Qt::black);
    hf.setBorderStyle(QXlsx::Format::BorderThin);
    hf.setPatternBackgroundColor(color);
    d.setColumnWidth(1, 10);
    d.setColumnWidth(2, 15);
    d.setColumnWidth(3, 15);
    d.setColumnWidth(4, 50);
    d.setColumnWidth(5, 20);
    d.setColumnWidth(6, 20);
    d.setColumnWidth(7, 20);
    d.setColumnWidth(8, 20);
    int col = 1, row = 1;
    d.write(row, col, QString("%1 N%2").arg(docTypeText, ui->leDocNum->text()), hf);
    row++;
    d.write(row, col, QString("%1 %2").arg(tr("Reason"), ui->leReasonName->text()), hf);
    row++;

    if(!ui->leComment->isEmpty()) {
        d.write(row, col, tr("Comment") + " " + ui->leComment->text(), hf);
        row++;
    }

    QList<int> cols;
    QStringList vals;
    col = 1;
    cols << col++;
    vals << tr("Date");

    if(!storeInName.isEmpty()) {
        vals << tr("Store, input");
        cols << col++;
    }

    if(!storeOutName.isEmpty()) {
        vals << tr("Store, output");
        cols << col++;
    }

    for(int i = 0; i < cols.count(); i++) {
        d.write(row, cols.at(i), vals.at(i), hf);
    }

    row++;
    vals.clear();
    vals << ui->deDate->text();

    if(!storeInName.isEmpty()) {
        vals << ui->leStoreInputName->text();
    }

    if(!storeOutName.isEmpty()) {
        vals << ui->leStoreOutputName->text();
    }

    for(int i = 0; i < cols.count(); i++) {
        d.write(row, cols.at(i), vals.at(i), hf);
    }

    row += 2;
    cols.clear();

    for(int i = 0; i < 7; i++) {
        cols << i + 1;
    }

    vals.clear();
    vals << tr("NN")
         << tr("Material code")
         << tr("Adgt")
         << tr("Goods")
         << tr("Qty")
         << tr("Unit")
         << tr("Price")
         << tr("Total");

    for(int i = 0; i < cols.count(); i++) {
        d.write(row, cols.at(i), vals.at(i), hf);
    }

    row++;
    QFont bodyFont(qApp->font());
    QXlsx::Format bf;
    bf.setFontColor(Qt::black);
    bf.setFont(bodyFont);
    bf.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    bf.setBorderStyle(QXlsx::Format::BorderThin);

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        vals.clear();
        vals << QString::number(i + 1);
        vals << ui->tblGoods->getString(i, col_goods_id);
        vals << ui->tblGoods->getString(i, col_adgt);
        vals << ui->tblGoods->getString(i, col_goods_name);
        vals << QString::number(str_float(ui->tblGoods->lineEdit(i, col_goods_qty)->text()));
        vals << ui->tblGoods->getString(i, col_goods_unit);
        vals << QString::number(str_float(ui->tblGoods->lineEdit(i, col_price)->text()));
        vals << QString::number(str_float(ui->tblGoods->lineEdit(i, col_total)->text()));

        for(int i = 0; i < cols.count(); i++) {
            d.write(row, cols.at(i), vals.at(i), bf);
        }

        row++;
    }

    cols.clear();
    cols << 5 << 6 << 7;
    vals.clear();
    vals << tr("Total amount");
    vals << QString::number(str_float(ui->leTotal->text()));
    vals << ui->cbCurrency->currentText();

    for(int i = 0; i < cols.count(); i++) {
        d.write(row, cols.at(i), vals.at(i), hf);
    }

    row++;
    d.mergeCells("A1:G1");
    d.mergeCells("A2:G2");
    d.mergeCells("A3:G3");
    QString filename = QFileDialog::getSaveFileName(nullptr, "", "", "*.xlsx");

    if(filename.isEmpty()) {
        return;
    }

    d.saveAs(filename);
    QDesktopServices::openUrl(filename);
}

void C5StoreDoc::dirtyEdit()
{
    DlgDirtyStoreDoc(mUser, fInternalId).exec();
    QString err;
    auto *doc = __mainWindow->createTab<C5StoreDoc>();

    if(!doc->openDoc(fInternalId, err)) {
        doc->correctDebt();
        __mainWindow->removeTab(doc);
        C5Message::error(err);
        return;
    }

    __mainWindow->removeTab(this);
}

void C5StoreDoc::newDoc()
{
    //    if (fDocState != DOC_STATE_SAVED && ui->tblGoods->rowCount() > 0) {
    //        C5Message::error(tr("Save current document first"));
    //        return;
    //    }
    //    fInternalId.clear();
    fInternalId = C5Database::uuid();
    fBasedOnSale = 0;
    fComplectationId.clear();
    fDocState = DOC_STATE_DRAFT;
    ui->leDocNum->clear();
    ui->lePartner->setValue(0);
    ui->leComment->clear();
    ui->leStoreInput->setValue(0);
    ui->leStoreOutput->setValue(0);
    ui->tblGoods->clearContents();
    ui->tblGoods->setRowCount(0);
    ui->tblDishes->clearContents();
    ui->tblDishes->setRowCount(0);
    ui->leInvoiceNumber->clear();
    ui->leComplectationCode->setValue("");
    ui->leComplectationQty->clear();
    ui->tblAdd->clearContents();
    ui->tblAdd->setRowCount(0);
    setDocEnabled(true);
    C5Database db;
    db[":f_id"] = (fDocType);
    db.exec("select f_counter from a_type where f_id=:f_id for update");
    db.nextRow();

    switch(fDocType) {
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

    if(__c5config.getRegValue("storedoc_storeinput").toBool()) {
        ui->leStoreInput->setValue(__c5config.getRegValue("storedoc_storeinput_id").toInt());
    }

    if(__c5config.getRegValue("storedoc_storeinput").toBool()) {
        ui->leStoreInput->setValue(__c5config.getRegValue("storedoc_storeinput_id").toInt());
    }

    countTotal();
}

void C5StoreDoc::getInput()
{
    QJsonArray vals;

    if(!C5Selector::getValueOfColumn(mUser, cache_goods, vals, 3)) {
        return;
    }

    if(vals.at(1).toInt() == 0) {
        C5Message::error(tr("Could not add goods without code"));
        return;
    }

    int row = addGoods(vals.at(1).toInt(), vals.at(3).toString(), -1, vals.at(4).toString(), -1, -1, "", vals.at(11).toString());

    if(__c5config.getValue(param_input_doc_fix_price).toInt() > 0) {
        ui->tblGoods->lineEdit(row, col_price)->setDouble(vals.at(6).toDouble());
    }

    ui->tblGoods->lineEdit(row, col_price)->setPlaceholderText(float_str(vals.at(6).toDouble(), 2));
    ui->tblGoods->lineEdit(row, col_goods_qty)->setFocus();
    fHttp->createHttpQuery("/engine/worker/check-qty.php", {
        {"method", "checkStore"}, {"goodsid", vals.at(1).toInt()}
    }, SLOT(slotCheckQtyResponse(QJsonObject)), QVariant(), false);
}

void C5StoreDoc::getOutput()
{
    QString err;

    if(ui->leStoreOutput->getInteger() == 0) {
        err += tr("Output store must be defined") + "<br>";
    }

    if(!err.isEmpty()) {
        C5Message::error(err);
        return;
    }

    QString showAmount = "sum(s.f_total*s.f_type) as f_amount,";
    QString query = QString("select s.f_goods, gg.f_name as f_groupname, g.f_name as f_goodsname, u.f_name as f_unitname, "
                            "sum(s.f_qty*s.f_type) as f_qty, %3 g.f_scancode "
                            "from a_store s "
                            "inner join a_header d on d.f_id=s.f_document "
                            "inner join c_goods g on g.f_id=s.f_goods "
                            "inner join c_groups gg on gg.f_id=g.f_group "
                            "inner join c_units u on u.f_id=g.f_unit "
                            "where s.f_store=%1 and d.f_date<=%2 and d.f_state=1 "
                            "group by 1, 2, 3, 4 "
                            "having sum(s.f_qty*s.f_type) > 0.00001 ")
                    .arg(ui->leStoreOutput->getInteger())
                    .arg(ui->deDate->toMySQLDate())
                    .arg(showAmount);
    QJsonArray vals;
    QHash<QString, QString> trans;
    trans["f_goods"] = tr("Code");
    trans["f_groupname"] = tr("Group");
    trans["f_goodsname"] = tr("Name");
    trans["f_unitname"] = tr("Unit");
    trans["f_qty"] = tr("Qty");
    trans["f_amount"] = tr("Amount");
    trans["f_scancode"] = tr("Scancode");

    if(!C5Selector::getValues(mUser, query, vals, trans)) {
        return;
    }

    int row = addGoodsRow();
    ui->tblGoods->setInteger(row, col_goods_id, vals.at(1).toInt());
    ui->tblGoods->setString(row, col_goods_name, vals.at(3).toString() + " " + vals.at(7).toString());
    ui->tblGoods->setString(row, col_goods_unit, vals.at(4).toString());
    ui->tblGoods->lineEdit(row, col_goods_qty)->setFocus();
}

void C5StoreDoc::saveDoc()
{
    writeDocumentWithState(DOC_STATE_SAVED);

    if(__c5config.getRegValue("storedoc_storeinput").toBool()) {
        __c5config.setRegValue("storedoc_storeinput_id", ui->leStoreInput->getInteger());
    }

    __c5config.setRegValue("storedoc_fixpartner", ui->btnFixPartner->isChecked() ? ui->lePartner->getInteger() : 0);
    int cache_id = C5Cache::idForTable("c_goods");
    C5Selector::resetCache(cache_id);
}

void C5StoreDoc::draftDoc()
{
    writeDocumentWithState(DOC_STATE_DRAFT);
}

void C5StoreDoc::removeDocument()
{
    if(removeDoc(fInternalId, true)) {
        __mainWindow->removeTab(this);
    }
}

void C5StoreDoc::createStoreInput()
{
    C5Database db;
    QHash<int, double> prices1;
    db.exec("select f_id, f_lastinputprice from c_goods");

    while(db.nextRow()) {
        prices1[db.getInt(0)] = db.getDouble(1);
    }

    C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>();
    sd->setMode(C5StoreDoc::sdInput);
    sd->setStore(0, 0);
    sd->setReason(DOC_REASON_INPUT);
    sd->setComment(ui->leComment->text());

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        int row = sd->addGoodsRow();
        sd->ui->tblGoods->setString(row, col_goods_id, ui->tblGoods->getString(row, col_goods_id));
        sd->ui->tblGoods->setString(row, col_goods_name, ui->tblGoods->getString(row, col_goods_name));
        sd->ui->tblGoods->lineEdit(row, col_goods_qty)->setDouble(ui->tblGoods->lineEdit(row, col_goods_qty)->getDouble());
        sd->ui->tblGoods->setString(row, col_goods_unit, ui->tblGoods->getString(row, col_goods_unit));
        sd->ui->tblGoods->lineEdit(row, col_price)->setDouble(prices1[ui->tblGoods->getInteger(row, col_goods_id)]);
        emit sd->ui->tblGoods->lineEdit(row, col_price)->textEdited(sd->ui->tblGoods->lineEdit(row, col_price)->text());
    }
}

void C5StoreDoc::printDoc()
{
    if(fInternalId == nullptr) {
        C5Message::error(tr("Document is not saved"));
        return;
    }

    C5StoreDocSelectPrintTemplate d;

    switch(d.exec()) {
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
    C5StoreBarcode *b = __mainWindow->createTab<C5StoreBarcode>();
    C5Database db;

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        db[":f_id"] = ui->tblGoods->getInteger(i, col_goods_id);
        db.exec("select f_name, f_scancode from c_goods where f_id=:f_id");

        if(db.nextRow()) {
            b->addRow(db.getString("f_name"), db.getString("f_scancode"),
                      ui->tblGoods->lineEdit(i, col_goods_qty)->getInteger() < 1 ? 1 : ui->tblGoods->lineEdit(i, col_goods_qty)->getInteger(),
                      ui->cbCurrency->currentData().toInt(), "");
        }
    }
}

void C5StoreDoc::checkInvoiceDuplicate()
{
    ui->leInvoiceNumber->setStyleSheet("");
    C5Database db;
    db[":f_invoice"] = ui->leInvoiceNumber->text();
    db[":f_id"] = fInternalId;
    db.exec("select count(f_id) from a_header_store where f_invoice=:f_invoice and f_id<>:f_id");

    if(db.nextRow()) {
        if(db.getInt(0) > 0) {
            ui->leInvoiceNumber->setStyleSheet("background:red;");
        } else {
            ui->leInvoiceNumber->setStyleSheet("");
        }
    }
}

void C5StoreDoc::tblAddChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    countTotal();
}

void C5StoreDoc::tblDishQtyChanged(const QString &arg1)
{
    int r, c;
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());

    if(!ui->tblDishes->findWidget(l, r, c)) {
        return;
    }

    QJsonArray ja = QJsonDocument::fromJson(ui->tblDishes->getString(r, 4).toUtf8()).array();

    for(int i = 0; i < ja.count(); i++) {
        bool found = false;
        QJsonObject jo = ja.at(i).toObject();

        for(int j = 0; j < ui->tblGoods->rowCount(); j++) {
            if(ui->tblGoods->getInteger(j, col_goods_id) == jo["f_goods"].toInt()) {
                ui->tblGoods->lineEdit(j, col_goods_qty)->setDouble(ui->tblGoods->lineEdit(j, col_goods_qty)->getDouble() - (jo["f_qty"].toDouble() *jo["f_prevqty"].toDouble()));
                jo["f_prevqty"] = arg1.toDouble();
                ui->tblGoods->lineEdit(j, col_goods_qty)->setDouble(ui->tblGoods->lineEdit(j, col_goods_qty)->getDouble() + (jo["f_qty"].toDouble() *jo["f_prevqty"].toDouble()));
                ja[i] = jo;
                found = true;
                break;
            }
        }

        if(!found) {
            int r = addGoodsRow();
            ui->tblGoods->setString(r, col_goods_id, jo["f_goods"].toString());
            ui->tblGoods->lineEdit(r, col_goods_qty)->setDouble(jo["f_qty"].toDouble() *jo["f_prevqty"].toDouble());
        }
    }

    ui->tblDishes->setString(r, 4, QJsonDocument(ja).toJson());
}

void C5StoreDoc::tblQtyChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;

    if(!ui->tblGoods->findWidget(static_cast<QWidget* >(sender()), row, col)) {
        return;
    }

    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col_goods_qty);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col_price);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col_total);
    ltotal->setDouble(lqty->getDouble() *lprice->getDouble());
    countTotal();
}

void C5StoreDoc::tblPriceChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;

    if(!ui->tblGoods->findWidget(static_cast<QWidget* >(sender()), row, col)) {
        return;
    }

    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col_goods_qty);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col_price);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col_total);
    double d1 = lqty->getDouble();
    double d2 = lprice->getDouble();
    ltotal->setDouble(d1 * d2);
    countTotal();
}

void C5StoreDoc::tblTotalChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;

    if(!ui->tblGoods->findWidget(static_cast<QWidget* >(sender()), row, col)) {
        return;
    }

    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col_goods_qty);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col_price);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col_total);

    if(lqty->getDouble() > 0.001) {
        lprice->setDouble(ltotal->getDouble() / lqty->getDouble());
    }

    countTotal();
}

void C5StoreDoc::on_btnAddGoods_clicked()
{
    switch(fDocType) {
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
}

void C5StoreDoc::on_btnRemoveGoods_clicked()
{
    int row = ui->tblGoods->currentRow();

    if(row < 0) {
        return;
    }

    if(C5Message::question(tr("Confirm to remove") + "<br>" + ui->tblGoods->item(row, col_goods_name)->text()) != QDialog::Accepted) {
        return;
    }

    ui->tblGoods->removeRow(row);

    if(ui->tblDishes->rowCount() > 0) {
        for(int i = 0; i < ui->tblDishes->rowCount(); i++) {
            QJsonArray ja = QJsonDocument::fromJson(ui->tblDishes->getString(i, col_goods_name).toUtf8()).array();

            for(int j = ja.count() - 1; j > -1; j--) {
                QJsonObject jo = ja[j].toObject();

                if(jo["f_row"].toInt() == row) {
                    ja.removeAt(j);
                    continue;
                }

                if(jo["f_row"].toInt() > row) {
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
    setUserId(false, ui->leDocNum->getInteger());
}

void C5StoreDoc::on_leStoreOutput_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    setUserId(false, ui->leDocNum->getInteger());
}

void C5StoreDoc::on_btnNewPartner_clicked()
{
    CE5Partner *ep = new CE5Partner();
    C5Editor *e = C5Editor::createEditor(mUser, ep, 0);
    QList<QMap<QString, QVariant> > data;

    if(e->getResult(data)) {
        // ui->lePartner->setValue(data.at(0)["f_id"].toString());
    }

    delete e;
}

void C5StoreDoc::on_btnNewGoods_clicked()
{
    CE5Goods *ep = new CE5Goods();
    C5Editor *e = C5Editor::createEditor(mUser, ep, 0);
    QJsonObject data;

    if(e->getJsonObject(data)) {
        QJsonObject j = data["goods"].toObject();

        if(j["f_id"].toInt() == 0) {
            C5Message::error(tr("Cannot add goods without code"));
            return;
        }

        int row = addGoodsRow();
        ui->tblGoods->setData(row, col_goods_id, j["f_id"].toInt());
        ui->tblGoods->setData(row, col_goods_name, j["f_name"].toString() + " " + j["f_scancode"].toString());
        ui->tblGoods->setData(row, col_goods_unit, data["f_unitname"].toString());
        ui->tblGoods->setData(row, col_adgt, j["f_adg"].toString());
        ui->tblGoods->lineEdit(row, col_goods_qty)->setFocus();
    }

    delete e;
}

void C5StoreDoc::on_leScancode_returnPressed()
{
    QString qty  = ui->chLeaveFocusOnBarcode->isChecked() ? "1" : "";
    addByScancode(ui->leScancode->text(), qty, "");

    if(ui->chLeaveFocusOnBarcode->isChecked()) {
        ui->leScancode->setFocus();
    }
}

TableCell::TableCell(QWidget *parent, QTableWidgetItem *item) :
    QLabel(parent, Qt::FramelessWindowHint)
{
    setText(item->text());
    fOldItem = item;
}

void C5StoreDoc::on_leComplectationName_textChanged(const QString &arg1)
{
    ui->tblGoods->clearContents();
    ui->tblGoods->setRowCount(0);
    fBaseQtyOfComplectation.clear();
    fBaseComplectOutput = 0;
    C5Cache *c = C5Cache::cache(cache_goods);
    int gr = c->find(ui->leComplectationCode->getInteger());

    if(gr > -1) {
        ui->lbComplectUnit->setText(c->getString(gr, tr("Unit")));
        fBaseComplectOutput = c->getDouble(gr, tr("Complect output"));
        ui->leComplectationScancodeCode->setText(c->getString(gr, tr("Scancode")));
    } else {
        ui->lbComplectUnit->clear();
    }

    if(arg1.isEmpty()) {
        countTotal();
        return;
    }

    C5Database db;
    db[":f_base"] = ui->leComplectationCode->getInteger();
    db.exec("select c.f_id, c.f_goods, g.f_name as f_goodsname, "
            "c.f_qty, u.f_name as f_unitname, "
            "c.f_price, c.f_qty*c.f_price as f_total "
            "from c_goods_complectation c "
            "left join c_goods g on g.f_id=c.f_goods "
            "left join c_units u on u.f_id=g.f_unit "
            "where c.f_base=:f_base");

    while(db.nextRow()) {
        int row = addGoodsRow();
        fBaseQtyOfComplectation.insert(row, db.getDouble("f_qty"));
        ui->tblGoods->setInteger(row, col_goods_id, db.getInt("f_goods"));
        ui->tblGoods->setString(row, col_goods_name, db.getString("f_goodsname"));
        ui->tblGoods->lineEdit(row, col_goods_qty)->setDouble(db.getDouble("f_qty"));
        ui->tblGoods->setString(row, col_goods_unit, db.getString("f_unitname"));
        ui->tblGoods->lineEdit(row, col_price)->setDouble(db.getDouble("f_price"));
        ui->tblGoods->lineEdit(row, col_total)->setDouble(db.getDouble("f_total"));
    }

    countQtyOfComplectation();
    countTotal();
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
}

void C5StoreDoc::on_btnRemoveAdd_clicked()
{
    int row = ui->tblAdd->currentRow();

    if(row < 0) {
        return;
    }

    if(C5Message::question(tr("Confirm to remove") + "<br>" + ui->tblAdd->lineEdit(row, 2)->text()) != QDialog::Accepted) {
        return;
    }

    ui->tblAdd->removeRow(row);
    countTotal();
}

void C5StoreDoc::on_btnEditGoods_clicked()
{
    int row = ui->tblGoods->currentRow();

    if(row < 0) {
        return;
    }

    if(ui->tblGoods->getInteger(row, col_goods_id) == 0) {
        return;
    }

    CE5Goods *ep = new CE5Goods();
    C5Editor *e = C5Editor::createEditor(mUser, ep, 0);
    ep->setId(ui->tblGoods->getInteger(row, col_goods_id));
    QJsonObject data;

    if(e->getJsonObject(data)) {
        QJsonObject j = data["goods"].toObject();

        if(j["f_id"].toInt() == 0) {
            C5Message::error(tr("Cannot change goods without code"));
            return;
        }

        ui->tblGoods->setData(row, col_goods_id, j["f_id"].toInt());
        ui->tblGoods->setData(row, col_goods_name, j["f_name"].toString() + " " + j["f_scancode"].toString());
        ui->tblGoods->setData(row, col_goods_unit, data["f_unitname"].toString());
        ui->tblGoods->setData(row, col_adgt, j["f_adg"].toString());
        ui->tblGoods->lineEdit(row, col_goods_qty)->setFocus();
    }

    delete e;
}

void C5StoreDoc::on_btnCalculator_clicked()
{
    double v;
    Calculator::get(v, mUser);
}

void C5StoreDoc::duplicateOutput()
{
    if(fDocState != DOC_STATE_SAVED) {
        C5Message::error(tr("Document must be saved"));
        return;
    }

    auto *sd = __mainWindow->createTab<C5StoreDoc>();
    sd->setMode(sdOutput);
    sd->setStore(0, ui->leStoreInput->getInteger());
    sd->setReason(DOC_REASON_OUT);

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        sd->addGoods(ui->tblGoods->getInteger(i, col_goods_id),
                     ui->tblGoods->getString(i, col_goods_name),
                     ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble(),
                     ui->tblGoods->getString(i, col_goods_unit),
                     ui->tblGoods->lineEdit(i, col_price)->getDouble(),
                     ui->tblGoods->lineEdit(i, col_total)->getDouble(),
                     ui->tblGoods->lineEdit(i, col_valid_date)->text(),
                     ui->tblGoods->getString(i, col_adgt));
    }
}

void C5StoreDoc::duplicateAsInput()
{
    if(fDocState != DOC_STATE_SAVED) {
        C5Message::error(tr("Document must be saved"));
        return;
    }

    auto *sd = __mainWindow->createTab<C5StoreDoc>();
    sd->setMode(sdInput);
    sd->setStore(0, ui->leStoreInput->getInteger());
    sd->setReason(DOC_REASON_INPUT);

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        sd->addGoods(ui->tblGoods->getInteger(i, col_goods_id),
                     ui->tblGoods->getString(i, col_goods_name),
                     ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble(),
                     ui->tblGoods->getString(i, col_goods_unit),
                     ui->tblGoods->lineEdit(i, col_price)->getDouble(),
                     ui->tblGoods->lineEdit(i, col_total)->getDouble(),
                     ui->tblGoods->lineEdit(i, col_comment)->text(),
                     ui->tblGoods->getString(i, col_adgt));
    }
}

void C5StoreDoc::on_btnAddDish_clicked()
{
    QJsonArray vals;

    if(!C5Selector::getValue(mUser, cache_dish, vals)) {
        return;
    }

    if(vals.at(1).toInt() == 0) {
        C5Message::error(tr("Could not add goods without code"));
        return;
    }

    addGoodsByCalculation(vals.at(1).toInt(), vals.at(2).toString(), 0);
}

void C5StoreDoc::on_btnRemoveRows_clicked()
{
    int row = ui->tblDishes->currentRow();

    if(row < 0) {
        return;
    }

    if(C5Message::question(tr("Confirm to remove") + "<br>" + ui->tblDishes->item(row, 2)->text()) != QDialog::Accepted) {
        return;
    }

    QJsonArray ja = QJsonDocument::fromJson(ui->tblDishes->getString(row, col_goods_name).toUtf8()).array();

    for(int i = 0; i < ja.count(); i++) {
        QJsonObject jo = ja[i].toObject();

        for(int j = 0; j < ui->tblGoods->rowCount(); j++) {
            if(jo["f_goods"].toInt() == ui->tblGoods->getInteger(j, col_goods_id)) {
                ui->tblGoods->lineEdit(j, col_goods_qty)->setDouble(ui->tblGoods->lineEdit(j, col_goods_qty)->getDouble() - (jo["f_prevqty"].toDouble() *jo["f_qty"].toDouble()));

                if(ui->tblGoods->lineEdit(j, col_goods_qty)->getDouble() < 0.001) {
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
    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        bool hidden = !arg1.isEmpty();

        if(!arg1.isEmpty()) {
            hidden = !(ui->tblGoods->getString(i, col_goods_name).contains(arg1, Qt::CaseInsensitive)
                       || ui->tblGoods->getString(i, col_goods_id).contains(arg1, Qt::CaseInsensitive));
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
    if(checked) {
        ui->btnFillRemote->setIcon(QIcon(":/wifi.png"));
        __mainWindow->addBroadcastListener(this);
    } else {
        ui->btnFillRemote->setIcon(QIcon(":/wifib.png"));
        __mainWindow->removeBroadcastListener(this);
    }
}

void C5StoreDoc::on_btnAddPackages_clicked()
{
    QJsonArray vals;

    if(!C5Selector::getValue(mUser, cache_dish_package, vals)) {
        return;
    }

    if(vals.at(1).toInt() == 0) {
        C5Message::error(tr("Could not add package without code"));
        return;
    }

    bool ok = false;
    double qty = QInputDialog::getDouble(this, tr("Qty of package"), "", 0, 1, 999999, 2, &ok);

    if(!ok) {
        return;
    }

    C5Database db;
    db[":f_package"] = vals.at(1);
    db.exec("select d.f_name, p.f_dish, p.f_qty from d_package_list p left join d_dish d on d.f_id=p.f_dish where f_package=:f_package");

    while(db.nextRow()) {
        addGoodsByCalculation(db.getInt("f_dish"), db.getString("f_name"), db.getDouble("f_qty") *qty);
    }
}

void C5StoreDoc::on_btnChangePartner_clicked()
{
    QJsonArray values;

    if(!C5Selector::getValue(mUser, cache_goods_partners, values)) {
        return;
    }

    if(values.count() == 0) {
        return;
    }

    ui->lePartner->setValue(values.at(1).toInt());
    C5Database db;
    db[":f_id"] = fInternalId;
    db[":f_partner"] = ui->lePartner->getInteger();
    db.exec("update a_header set f_partner=:f_partner where f_id=:f_id");
    db[":f_storedoc"] = fInternalId;
    db[":f_costumer"] = ui->lePartner->getInteger();
    db.exec("update b_clients_debts set f_costumer=:f_costumer where f_storedoc=:f_storedoc");
}

void C5StoreDoc::on_btnFixPartner_clicked(bool checked)
{
    __c5config.setRegValue("storedoc_fixpartner", checked ? ui->lePartner->getInteger() : 0);
}

void C5StoreDoc::on_btnCopyLastAdd_clicked()
{
    C5Database db;
    db.exec("select * from s_draft");

    if(db.nextRow()) {
        db[":f_header"] = db.getString(1);
        db.exec("select * from a_complectation_additions where f_header=:f_header order by f_row");

        while(db.nextRow()) {
            int row = ui->tblAdd->rowCount();
            ui->tblAdd->setRowCount(row + 1);
            C5LineEdit *l = ui->tblAdd->createLineEdit(row, 1);
            l->setFocus();
            l->setText(db.getString("f_name"));
            l = ui->tblAdd->createLineEdit(row, 2);
            l->setValidator(new QDoubleValidator());
            l->setDouble(db.getDouble("f_amount"));
            connect(l, SIGNAL(textEdited(QString)), this, SLOT(tblAddChanged(QString)));
        }
    }

    countTotal();
}

void C5StoreDoc::on_cbCurrency_currentIndexChanged(int index)
{
    Q_UNUSED(index);

    if(fDocState == DOC_STATE_SAVED && fDocType == DOC_TYPE_STORE_INPUT) {
        if(C5Message::question("Փոխել պահպանված և բոլոր կապակցված փասթաթղթերի տարադրամը՞")
                == QDialog::Accepted) {
            fHttp->createHttpQuery("/engine/goods/quick-change-currency.php",
            QJsonObject{{"id", fInternalId}, {"currency", ui->cbCurrency->currentData().toInt()}},
            SLOT(changeCurrencyResponse(QJsonObject)));
        }
    }
}

void C5StoreDoc::on_btnEditPassed_clicked()
{
    QJsonArray values;

    if(!C5Selector::getValue(mUser, cache_users, values)) {
        return;
    }

    if(values.count() == 0) {
        return;
    }

    if(fDocState == DOC_STATE_SAVED) {
        if(C5Message::question(tr("This action will result in immediate changes to the data.")) != QDialog::Accepted) {
            return;
        }

        C5Database db;
        db[":f_id"] = fInternalId;
        db[":f_userpass"] = values.at(1).toInt();
        db.exec("update a_header set f_body=json_set(f_body, '$.f_passed', :f_userpass) where f_id=:f_id");
    }

    ui->lePassed->setValue(values.at(1).toInt());
}

void C5StoreDoc::on_btnEditAccept_clicked()
{
    QJsonArray values;

    if(!C5Selector::getValue(mUser, cache_users, values)) {
        return;
    }

    if(values.count() == 0) {
        return;
    }

    if(fDocState == DOC_STATE_SAVED) {
        if(C5Message::question(tr("This action will result in immediate changes to the data.")) != QDialog::Accepted) {
            return;
        }

        C5Database db;
        db[":f_id"] = fInternalId;
        db[":f_useraccept"] = values.at(1).toInt();
        db.exec("update a_header set f_body=json_set(f_body, '$.f_accepted', :f_useraccept) where f_id=:f_id");
    }

    ui->leAccepted->setValue(values.at(1).toInt());
}

void C5StoreDoc::on_btnSaveComment_clicked()
{
    C5Database db;
    db[":f_comment"] = ui->leComment->text();
    db[":f_id"] = fInternalId;
    db.exec("update a_header set f_comment=:f_comment where f_id=:f_id");
}

void C5StoreDoc::on_btnSavePayment_clicked()
{
    if(fDocState == DOC_STATE_DRAFT) {
        C5Message::error(tr("Inpossible in draft mode"));
        return;
    }

    QString err;
    C5Database db;

    if(ui->chPaid->isChecked()) {
        if(ui->leCash->getInteger() == 0) {
            C5Message::error(tr("Cash must be selected"));
            return;
        }

        QJsonObject jo;
        jo["f_cashout"] = ui->leCash->getInteger();
        jo["f_currency"] = ui->cbCurrency->currentData().toInt();
        jo["f_amount"] = ui->leTotal->getDouble();
        jo["f_purpose"] = QString("%1 %2").arg(tr("Store input"), ui->leDocNum->text());
        jo["f_comment"] = QString("%1 %2").arg(tr("Store input"), ui->leDocNum->text());
        jo["f_operator"] = mUser->id();
        jo["f_date"] = ui->deCashDate->date().toString(FORMAT_DATE_TO_STR_MYSQL);
        db[":params"] = QString(QJsonDocument(jo).toJson());
        db.exec("select sf_create_cashdoc(:params)");
        db.nextRow();
        jo = QJsonDocument::fromJson(db.getString(0).toUtf8()).object();
        fCashDocUuid = jo["cashdoc"].toString();
        db[":f_id"] = fInternalId;
        db[":f_payment"] = fCashDocUuid;
        db.exec("update a_header set f_payment=:f_payment where f_id=:f_id");
        db[":f_storedoc"] = fInternalId;
        db.exec("delete from b_clients_debts where f_storedoc=:f_storedoc");
    } else {
        db[":cashdoc"] = fCashDocUuid;
        db.exec("select sf_remove_cashdoc(:cashdoc) ");
        BClientDebts b;
        b.date = ui->deDate->date();
        b.costumer = ui->lePartner->getInteger();
        b.currency = ui->cbCurrency->currentData().toInt();
        b.amount = -1 * ui->leTotal->getDouble();
        b.source = 1;
        b.store = fInternalId;
        b.write(db, err);
        b.comment = QString("%1 %2").arg(tr("Store input"), ui->leDocNum->text());
    }

    C5Message::info(tr("Payment saved"));
}
