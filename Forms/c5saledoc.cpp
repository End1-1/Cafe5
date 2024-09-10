#include "c5saledoc.h"
#include "c5checkbox.h"
#include "ui_c5saledoc.h"
#include "c5message.h"
#include "c5utils.h"
#include "c5cache.h"
#include "c5selector.h"
#include "c5user.h"
#include "c5printrecipta4.h"
#include "ce5goods.h"
#include "c5storedraftwriter.h"
#include "threadsendmessage.h"
#include "c5daterange.h"
#include "breezeconfig.h"
#include "armsoftexportoptions.h"
#include "dlglist2.h"
#include "../../Xlsx/src/xlsxall.h"
#include "httpquerydialog.h"
#include "c5mainwindow.h"
#include "ce5partner.h"
#include "outputofheader.h"
#include "../../NewTax/Src/printtaxn.h"
#include "oheader.h"
#include <QClipboard>
#include <QSqlQuery>
#include <QInputDialog>

#define col_uuid 0
#define col_checkbox 1
#define col_goods_code 2
#define col_store 3
#define col_barcode 4
#define col_name 5
#define col_qty 6
#define col_unit 7
#define col_price 8
#define col_discount_value 9
#define col_discount_amount 10
#define col_grandtotal 11
#define col_type 12
#define col_returnfrom 13

C5SaleDoc::C5SaleDoc(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5SaleDoc)
{
    ui->setupUi(this);
    fIcon = ":/pricing.png";
    fLabel = tr("Sale");
    ui->tblGoods->setColumnWidths(ui->tblGoods->columnCount(), 0, 50, 0, 150, 200, 300, 80, 80, 80, 80, 80, 80, 0, 0);
    ui->cbCurrency->setDBValues(dbParams, "select f_id, f_name from e_currency");
    ui->cbCurrency->setCurrentIndex(ui->cbCurrency->findData(__c5config.getValue(param_default_currency)));
    ui->cbStorage->setDBValues(dbParams, "select f_id, f_name from c_storages order by 2");
    ui->cbStorage->setCurrentIndex(-1);
    ui->cbHall->setDBValues(dbParams, "select f_id, f_name from h_halls order by 2");
    ui->cbHall->setCurrentIndex(ui->cbHall->findData(__c5config.getValue(param_default_hall).toInt()));
    ui->cbStorage->setCurrentIndex(ui->cbStorage->findData(__c5config.getValue(param_default_store).toInt()));
    ui->cbCashDesk->setCurrentIndex(ui->cbCashDesk->findData(__c5config.getValue(param_default_table).toInt()));
    ui->flag->setVisible(false);
    fOpenedFromDraft = false;
    connect(ui->leUuid, &C5LineEdit::doubleClicked, this, &C5SaleDoc::uuidDoubleClicked);
    connect(ui->leBankTransfer, &C5LineEdit::doubleClicked, this, &C5SaleDoc::amountDoubleClicked);
    connect(ui->leCash, &C5LineEdit::doubleClicked, this, &C5SaleDoc::amountDoubleClicked);
    connect(ui->leCard, &C5LineEdit::doubleClicked, this, &C5SaleDoc::amountDoubleClicked);
    connect(ui->lePrepaid, &C5LineEdit::doubleClicked, this, &C5SaleDoc::amountDoubleClicked);
    connect(ui->leDebt, &C5LineEdit::doubleClicked, this, &C5SaleDoc::amountDoubleClicked);
    connect(ui->leBankTransfer, &C5LineEdit::doubleClicked, this, &C5SaleDoc::amountDoubleClicked);
}

C5SaleDoc::~C5SaleDoc()
{
    delete ui;
}

void C5SaleDoc::setMode(int mode)
{
    fMode = mode;
    ui->leSaleType->setProperty("id", fMode);
    switch (fMode) {
        case 1:
            ui->leSaleType->setText(tr("Retail"));
            break;
        case 2:
            ui->leSaleType->setText(tr("Whosale"));
            break;
        case 3:
            fActionReturn->setVisible(false);
            ui->btnSearchTaxpayer->setEnabled(false);
            ui->btnEditPartner->setEnabled(false);
            ui->wtoolbar->setEnabled(false);
            break;
    }
}

bool C5SaleDoc::reportHandler(const QString &handleId, const QVariant &data)
{
    if (handleId == REPORT_HANDLER_SALE_DOC_OPEN_DRAFT) {
        if (!openDraft(data.toString())) {
            return false;
        }
    }
    return true;
}

QToolBar *C5SaleDoc::toolBar()
{
    if (!fToolBar) {
        createToolBar();
        fActionSave = fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDataChanges()));
        fActionDraft = fToolBar->addAction(QIcon(":/draft.png"), tr("Draft"), this, SLOT(saveAsDraft()));
        fActionCopy = fToolBar->addAction(QIcon(":/copy.png"), tr("Copy"), this, SLOT(saveCopy()));
        fRemoveAction  = fToolBar->addAction(QIcon(":/delete.png"), tr("Remove"), this, SLOT(removeDoc()));
        fToolBar->addAction(QIcon(":/print.png"), tr("Make store output"), this, SLOT(makeStoreOutput()));
        fToolBar->addAction(QIcon(":/AS.png"), tr("Export to AS\r\n invoice"), this, SLOT(createInvoiceAS()));
        fToolBar->addAction(QIcon(":/AS.png"), tr("Export to AS\r\n retail"), this, SLOT(createRetailAS()));
        fToolBar->addAction(QIcon(":/print.png"), tr("Print"), this, SLOT(printSale()));
        fPrintTax = fToolBar->addAction(QIcon(":/fiscal.png"), tr("Fiscal"), this, SLOT(fiscale()));
        fToolBar->addAction(QIcon(":/excel.png"), tr("Export to Excel"), this, SLOT(exportToExcel()));
        fActionReturn = fToolBar->addAction(QIcon(":/trading.png"), tr("Return"), this, SLOT(returnItems()));
    }
    return fToolBar;
}

bool C5SaleDoc::openDoc(const QString &uuid)
{
    C5Database db(fDBParams);
    //GOODS
    db[":f_header"] = uuid;
    db.exec(QString("select dsb.f_id as ogoodsid, dsb.f_store, dsb.f_qty, g.*, gu.f_name as f_unitname, dsb.f_price, dsb.f_discountfactor,  "
                    "dsb.f_returnfrom "
                    "from o_goods dsb "
                    "left join c_goods g on g.f_id=dsb.f_goods "
                    "left join c_units gu on gu.f_id=g.f_unit "
                    "where dsb.f_header=:f_header  "));
    while (db.nextRow()) {
        addGoods(db.getString("ogoodsid"), db.getInt("f_store"), db.getInt("f_id"), db.getString("f_scancode"),
                 db.getString("f_name"),
                 db.getString("f_unitname"), db.getDouble("f_qty"), db.getDouble("f_price"), db.getDouble("f_discountfactor") * 100, 0,
                 db.getString("f_returnfrom"));
    }
    //HEADER
    db[":f_id"] = uuid;
    db.exec("select * from o_header where f_id=:f_id");
    OHeader o;
    if (!o.getRecord(db)) {
        C5Message::error(tr("Invalid draft document id"));
        return false;
    }
    ui->cbHall->setCurrentIndex(ui->cbHall->findData(o.hall));
    ui->cbCashDesk->setCurrentIndex(ui->cbCashDesk->findData(o.table));
    setMode(db.getInt("f_saletype"));
    ui->flag->setVisible(db.getInt("f_flag") == 1);
    if (db.getInt("f_state") > 1) {
        fToolBar->actions().at(0)->setEnabled(false);
    }
    ui->leDocnumber->setText(QString("%1%2").arg(db.getString("f_prefix")).arg(db.getInt("f_hallid")));
    ui->leCash->setDouble(db.getDouble("f_amountcash"));
    ui->leCard->setDouble(db.getDouble("f_amountcard"));
    ui->leBankTransfer->setDouble(db.getDouble("f_amountbank"));
    ui->leDebt->setDouble(db.getDouble("f_amountdebt"));
    ui->lePrepaid->setDouble(db.getDouble("f_amountprepaid"));
    ui->leGrandTotal->setDouble(db.getDouble("f_amounttotal"));
    fPartner.queryRecordOfId(db, o.partner);
    setPartner(fPartner);
    ui->leDate->setDate(o.dateCash);
    ui->leTime->setText(o.timeClose.toString(FORMAT_TIME_TO_STR));
    ui->leComment->setText(o.comment);
    ui->leUuid->setText(o.id.toString());
    //DELIVERY INFO
    db[":f_id"] = uuid;
    db.exec("select * from o_draft_sale where f_id=:f_id");
    if (db.nextRow()) {
        ui->leDelivery->setText(db.getDate("f_datefor").toString(FORMAT_DATE_TO_STR));
        db[":f_id"] = db.getInt("f_staff");
        db.exec("select f_id, concat_ws(' ', f_last, f_first) as f_fullname from s_user where f_id=:f_id");
        if (db.nextRow()) {
            ui->leDeluveryMan->setProperty("id", db.getInt("f_id"));
            ui->leDeluveryMan->setText(db.getString("f_fullname"));
        }
    }
    fActionSave->setEnabled(o.state != ORDER_STATE_CLOSE);
    fActionDraft->setEnabled(o.state == ORDER_STATE_CLOSE);
    fActionCopy->setEnabled(true);
    for (int r = 0; r < ui->tblGoods->rowCount(); r++) {
        for (int c = 0; c < ui->tblGoods->columnCount(); c++) {
            QWidget *w = ui->tblGoods->cellWidget(r, c);
            if (w) {
                w->setEnabled(o.state != ORDER_STATE_CLOSE);
            }
        }
    }
    ui->wtoolbar->setEnabled(o.state != ORDER_STATE_CLOSE);
    ui->paymentFrame->setEnabled(o.state != ORDER_STATE_CLOSE);
    return true;
}

void C5SaleDoc::makeDraftResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
    openDraft(jdoc["id"].toString());
}

void C5SaleDoc::removeDocResponse(const QJsonObject &jdoc)
{
    Q_UNUSED(jdoc);
    fHttp->httpQueryFinished(sender());
    C5Message::info(tr("Removed"));
    __mainWindow->removeTab(this);
}

void C5SaleDoc::amountDoubleClicked()
{
    ui->leCash->clear();
    ui->leCard->clear();
    ui->leBankTransfer->clear();
    ui->leDebt->clear();
    ui->lePrepaid->clear();
    static_cast<C5LineEdit *>(sender())->setDouble(ui->leGrandTotal->getDouble());
}

void C5SaleDoc::printSale()
{
    C5PrintReciptA4 p(fDBParams, ui->leUuid->text(), this);
    QString err;
    p.print(err);
    if (!err.isEmpty()) {
        C5Message::error(err);
    }
}

void C5SaleDoc::fiscale()
{
    //    auto *d = new C5PrintTaxAnywhere(fDBParams, ui->leUuid->text());
    //    d->exec();
    //    d->deleteLater();
    //    return;
    QElapsedTimer t;
    t.start();
    C5Database db(fDBParams);
    db[":f_header"] = ui->leUuid->text();
    db.exec("select gr.f_adgcode, og.f_goods, gn.f_name, og.f_price, og.f_qty, og.f_discountfactor*100 as f_discount "
            "from o_goods og "
            "left join c_goods gn on gn.f_id=og.f_goods "
            "left join c_groups gr on gr.f_id=gn.f_group "
            "where og.f_header=:f_header");
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(),
                 C5Config::taxUseExtPos().isEmpty() ? "false" : C5Config::taxUseExtPos(), C5Config::taxCashier(), C5Config::taxPin(),
                 this);
    while (db.nextRow()) {
        pt.addGoods(1, //dep
                    db.getString("f_adgcode"), //adg
                    db.getString("f_goods"), //goods id
                    db.getString("f_name"), //name
                    db.getDouble("f_price"), //price
                    db.getDouble("f_qty"), //qty
                    db.getDouble("f_discount")); //discount
    }
    QString jsonIn, jsonOut, err;
    QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
    int result = 0;
    pt.fEmarks = fEmarks;
    result = pt.makeJsonAndPrint(ui->leCard->getDouble()
                                 + ui->leDebt->getDouble()
                                 + ui->leBankTransfer->getDouble(), 0, jsonIn, jsonOut, err);
    QJsonObject jtax;
    jtax["f_order"] = ui->leUuid->text();
    jtax["f_elapsed"] = t.elapsed();
    jtax["f_in"] = QJsonDocument::fromJson(jsonIn.toUtf8()).object();
    jtax["f_out"] = QJsonDocument::fromJson(jsonOut.toUtf8()).object();;
    jtax["f_err"] = err;
    jtax["f_result"] = result;
    jtax["f_state"] = result == pt_err_ok ? 1 : 0;
    QString jtaxStr = QString(QJsonDocument(jtax).toJson(QJsonDocument::Compact));
    jtaxStr = jtaxStr.replace("\'", "\\\'");
    db.exec(QString("call sf_create_shop_tax('%1')")
            .arg(jtaxStr));
    if (result != pt_err_ok) {
        C5Message::error(err);;
    }
}

void C5SaleDoc::createInvoiceAS()
{
    exportToAs(5);
}

void C5SaleDoc::createRetailAS()
{
    exportToAs(20);
}

void C5SaleDoc::makeStoreOutput()
{
    C5Database db(fDBParams);
    OutputOfHeader ooh;
    ooh.make(db, ui->leUuid->text());
}

void C5SaleDoc::exportToExcel()
{
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
    s->addCell(row, col, QString("%1 N%2").arg(tr("Order"), ui->leDocnumber->text()),
               d.style()->styleNum("header"));
    row++;
    if (!ui->leTaxpayerName->isEmpty()) {
        s->addCell(row, col, tr("Buyer") + " " + ui->leTaxpayerName->text(), d.style()->styleNum("header"));
        row++;
    }
    QList<int> cols;
    QStringList vals;
    col = 1;
    cols << col++;
    vals << tr("Date");
    for (int i = 0; i < cols.count(); i++) {
        s->addCell(row, cols.at(i), vals.at(i), d.style()->styleNum("header"));
    }
    row++;
    vals.clear();
    vals << ui->leDate->text() + " " + ui->leTime->text();
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
        vals << ui->tblGoods->getString(i, 4);
        vals << ui->tblGoods->getString(i, 5);
        vals << ui->tblGoods->lineEdit(i, 6)->text();
        vals << ui->tblGoods->getString(i, 7);
        vals << ui->tblGoods->lineEdit(i, 8)->text();
        vals << ui->tblGoods->lineEdit(i, 9)->text();
        for (int j = 0; j < cols.count(); j++) {
            s->addCell(row, cols.at(j), vals.at(j), d.style()->styleNum("body"));
        }
        row++;
    }
    cols.clear();
    cols << 6 << 7;
    vals.clear();
    vals << tr("Total amount");
    vals << QString::number(str_float(ui->leGrandTotal->text()));
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
}

void C5SaleDoc::returnItems()
{
    C5Database db(fDBParams);
    db.startTransaction();
    int hallid;
    QString prefix;
    db[":f_id"] = ui->cbHall->currentData();
    db.exec("select f_counter + 1, f_prefix as f_counter from h_halls where f_id=:f_id for update");
    if (db.nextRow()) {
        hallid = db.getInt(0);
        prefix = db.getString(1);
        db[":f_counter"] = db.getInt(0);
        db.update("h_halls", where_id(ui->cbHall->currentData().toInt()));
    } else {
        db.rollback();
        C5Message::error(tr("No hall with id") + "<br>" + ui->cbHall->currentText());
        return;
    }
    QString err;
    OHeader oh;
    oh.id;
    oh.prefix = prefix;
    oh.hallId = hallid;
    oh.partner = fPartner.id.toInt();
    oh.state = ORDER_STATE_OPEN;
    oh.hall = ui->cbHall->currentData().toInt();
    oh.table = ui->cbCashDesk->currentData().toInt();
    oh.dateOpen = QDate::currentDate();
    oh.dateClose = QDate::currentDate();
    oh.timeOpen = QTime::currentTime();
    oh.timeClose = QTime::currentTime();
    oh.dateCash = ui->leDate->date();
    oh.cashier = __user->id();
    oh.staff = ui->leDeluveryMan->property("id").toInt();
    oh.comment = QString("%1 %2").arg(tr("Return from"), ui->leDocnumber->text());
    oh.print = 0;
    oh.amountTotal = ui->leGrandTotal->getDouble();
    oh.amountCash = ui->leCash->getDouble();
    oh.amountCard = ui->leCard->getDouble();
    oh.amountDebt = ui->leDebt->getDouble();
    oh.amountBank = ui->leBankTransfer->getDouble();
    oh.source = 1;
    oh.saleType = SALE_RETURN;
    oh.currency = ui->cbCurrency->currentData().toInt();
    if (!oh.write(db, err)) {
        db.rollback();
        C5Message::error(err);
        return;
    }
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        OGoods g;
        g.header = oh._id();
        g.goods = ui->tblGoods->getInteger(i, col_goods_code);
        g.store = ui->tblGoods->comboBox(i, col_store)->currentData().toInt();
        g.qty = ui->tblGoods->lineEdit(i, col_qty)->getDouble();
        g.price = ui->tblGoods->lineEdit(i, col_grandtotal)->getDouble() / ui->tblGoods->lineEdit(i, col_qty)->getDouble();
        g.total = ui->tblGoods->lineEdit(i, col_grandtotal)->getDouble();
        g.sign = -1;
        g.row = i + 1;
        g.storeRec = "";
        g.returnFrom = ui->tblGoods->getString(i, col_uuid);
        if (!g.write(db, err)) {
            C5Message::error(err);
            db.rollback();
            return;
        }
    }
    db.commit();
    auto *a = __mainWindow->createTab<C5SaleDoc>(fDBParams);
    a->openDoc(oh._id());
}

void C5SaleDoc::messageResult(QJsonObject jo)
{
    sender()->deleteLater();
    if (jo["status"].toInt() > 0) {
        C5Message::error(jo["data"].toString());
    }
}

void C5SaleDoc::saveDataChanges()
{
    QString err;
    if (ui->leDate->date().isValid() == false) {
        err += tr("Date is not valid") + "<br>";
    }
    if (!ui->cbHall->currentData().isValid()) {
        err += tr("Hall is not valid") + "<br>";
    }
    if (!ui->cbCashDesk->currentData().isValid()) {
        err += tr("Cash desk is not valid") + "<br>";
    }
    if (ui->cbStorage->currentData().toInt() < 1) {
        err += tr("Storage is not defined");
    }
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (ui->tblGoods->comboBox(i, col_store)->currentData().isValid() == false) {
            err += tr("Store not defined on row: ") + QString::number(i + 1) + "<br>";
        }
        if (ui->tblGoods->lineEdit(i, col_qty)->getDouble() < 0.001) {
            err += tr("Quantity not defined on row: ") + QString::number(i + 1) + "<br>";
        }
    }
    if (ui->leGrandTotal->getDouble() < 0) {
        err += tr("Invalid total amount");
    }
    if (ui->leCash->getDouble()
            + ui->leCard->getDouble()
            + ui->lePrepaid->getDouble()
            + ui->leDebt->getDouble()
            + ui->leBankTransfer->getDouble()
            < ui->leGrandTotal->getDouble()) {
        err += tr("Incomplete payment") + "<br>";
    }
    if (ui->leCash->getDouble()
            + ui->leCard->getDouble()
            + ui->lePrepaid->getDouble()
            + ui->leDebt->getDouble()
            + ui->leBankTransfer->getDouble()
            > ui->leGrandTotal->getDouble()) {
        err += tr("Incomplete payment") + "<br>";
    }
    if (err.isEmpty() == false) {
        C5Message::error(err);
        return;
    }
    if (fMode == 4) {
        saveReturnItems();
        return;
    }
    QString uuid = ui->leUuid->text();
    C5Database db(fDBParams);
    db.startTransaction();
    bool isnew = fOpenedFromDraft || ui->leUuid->isEmpty();
    int hallid;
    QString prefix;
    db[":f_id"] = ui->cbHall->currentData();
    db.exec("select f_counter + 1, f_prefix as f_counter from h_halls where f_id=:f_id for update");
    if (db.nextRow()) {
        hallid = db.getInt(0);
        prefix = db.getString(1);
        db[":f_counter"] = db.getInt(0);
        db.update("h_halls", where_id(ui->cbHall->currentData().toInt()));
        ui->leDocnumber->setText(QString("%1%2").arg(prefix, QString::number(hallid)));
    } else {
        C5Message::error(tr("No hall with id") + "<br>" + ui->cbHall->currentText());
        return;
    }
    db.commit();
    db.startTransaction();
    db[":f_hallid"] = hallid;
    db[":f_prefix"] = prefix;
    db[":f_state"] = ORDER_STATE_CLOSE;
    db[":f_hall"] = ui->cbHall->currentData();
    db[":f_table"] = ui->cbCashDesk->currentData();
    db[":f_dateclose"] = QDate::currentDate();
    db[":f_timeclose"] = QTime::currentTime();
    db[":f_datecash"] = ui->leDate->date();
    db[":f_cashier"] = __user->id();
    db[":f_staff"] = fDraftSale.staff;
    db[":f_comment"] = ui->leComment->text();
    db[":f_print"] = 0;
    db[":f_amounttotal"] = ui->leGrandTotal->getDouble();
    db[":f_amountcash"] = ui->leCash->getDouble();
    db[":f_amountcard"] = ui->leCard->getDouble();
    db[":f_amountprepaid"] = ui->lePrepaid->getDouble();
    db[":f_amountbank"] = ui->leBankTransfer->getDouble();
    db[":f_amountother"] = 0;
    db[":f_amountservice"] = 0;
    db[":f_amountdiscount"] = 0;
    db[":f_amountdebt"] = ui->leDebt->getDouble();
    db[":f_servicefactor"] = 0;
    db[":f_discountfactor"] = 0;
    db[":f_creditcardid"] = 0;
    db[":f_otherid"] = ui->leDebt->getDouble();
    db[":f_shift"] = 1;
    db[":f_source"] = 2;
    db[":f_saletype"] = fMode;
    db[":f_partner"] = fPartner.id ;
    db[":f_currentstaff"] = 0;
    db[":f_guests"] = 0;
    db[":f_precheck"] = 0;
    db[":f_amountidram"] = 0;
    db[":f_amountpayx"] = 0;
    db[":f_cash"] = ui->leCountCash->getDouble();
    db[":f_change"] = ui->leChange->getDouble();
    if (ui->leUuid->isEmpty() || fOpenedFromDraft) {
        if (fOpenedFromDraft) {
            db[":f_id"] = uuid;
            db[":f_dateopen"] = ui->leDate->date();
            db[":f_timeopen"] = QTime::fromString(ui->leTime->text(), FORMAT_TIME_TO_STR);
        } else {
            uuid = db.uuid();
            ui->leUuid->setText(uuid);
            db[":f_id"] = uuid;
        }
        db.insert("o_header");
    } else {
        db.update("o_header", "f_id", ui->leUuid->text());
    }
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        db[":f_header"]  = uuid;
        db[":f_body"] = 0;
        db[":f_store"] = ui->tblGoods->comboBox(i, col_store)->currentData();
        db[":f_goods"] = ui->tblGoods->getInteger(i, col_goods_code);
        db[":f_qty"] = ui->tblGoods->lineEdit(i, col_qty)->getDouble();
        db[":f_price"] = ui->tblGoods->lineEdit(i, col_price)->getDouble();
        db[":f_total"] = ui->tblGoods->getDouble(i, col_grandtotal);
        db[":f_tax"] = 0;
        db[":f_sign"] = 1;
        db[":f_taxdept"] = 1;
        db[":f_row"] = i;
        db[":f_discountfactor"] = ui->tblGoods->lineEdit(i, col_discount_value)->getDouble() / 100;
        db[":f_discountmode"] = 0;
        db[":f_discountamount"] = ui->tblGoods->lineEdit(i, col_discount_amount)->getDouble();
        db[":f_return"] = 0;
        db[":f_returnfrom"] = "";
        QString guuid;
        if (ui->tblGoods->getString(i, col_uuid).isEmpty()) {
            guuid = db.uuid();
            db[":f_id"] = guuid;
            db[":f_storerec"] = guuid;
            db.insert("o_goods");
            ui->tblGoods->setString(i, col_uuid, guuid);
        } else {
            db.update("o_goods", "f_id", ui->tblGoods->getString(i, col_uuid));
        }
    }
    db[":f_guests"] = 0;
    db[":f_splitted"] = 0;
    db[":f_deliveryman"] = 0;
    db[":f_currency"] = 0;
    if (isnew) {
        db[":f_id"] = uuid;
        db.insert("o_header_options");
    } else {
        db.update("o_header_options", "f_id", uuid);
    }
    db[":f_1"] = 0;
    db[":f_2"] = 0;
    db[":f_3"] = 0;
    db[":f_4"] = 0;
    db[":f_5"] = 0;
    if (isnew) {
        db[":f_id"] = uuid;
        db.insert("o_header_flags");
    } else {
        db.update("o_header_flags", "f_id", uuid);
    }
    //if (fOpenedFromDraft) {
    db[":f_id"] = uuid;
    db.exec("select * from o_draft_sale where f_id=:f_id");
    if (!db.nextRow()) {
        db[":f_id"] = uuid;
        db[":f_datefor"] = QDate::fromString("dd/MM/yyyy", ui->leDelivery->text());
        db.exec("INSERT INTO o_draft_sale (f_id, f_date, f_time, f_partner, f_saletype, f_state, f_STAFF, f_cashier, f_amount,  f_datefor) "
                "SELECT f_id, f_datecash, current_time(), f_partner, f_saletype, 2, f_staff, f_cashier, f_amounttotal, :f_datefor FROM o_header o "
                "WHERE o.f_id=:f_id");
    }
    db[":f_state"] = 2;
    db[":f_datefor"] = QDate::fromString(ui->leDelivery->text(), FORMAT_DATE_TO_STR);
    db.update("o_draft_sale", "f_id", uuid);
    //}
    QSet<int> usedStores;
    QStringList outDocIds;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (ui->tblGoods->getInteger(i, col_type) == 1) {
            continue;
        }
        //        if (ui->tblGoods->comboBox(i, col_store)->currentData().toInt() == ui->cbStorage->currentData().toInt()) {
        //            continue;
        //        }
        usedStores.insert(ui->tblGoods->comboBox(i, col_store)->currentData().toInt());
    }
    //Output of storage not equal to current storage
    QMap<QString, QString> outInPrices;
    C5StoreDraftWriter dw(db);
    if (!isnew) {
        db[":f_saleuuid"] = uuid;
        db.exec("select f_id from a_header_store where f_saleuuid=:f_saleuuid");
        if (db.nextRow()) {
            dw.removeStoreDocument(db, db.getString(0), err);
        }
    }
    for (int store : usedStores) {
        QString outStoreDocComment = QString("%1 %2%3").arg(tr("Output of sale"), prefix, QString::number(hallid));
        QString outStoreDocId;
        QString outStoredocUserNum;
        outStoredocUserNum = dw.storeDocNum(DOC_TYPE_STORE_OUTPUT, store, true, 0);
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            if (ui->tblGoods->getInteger(i, col_type) == 1) {
                continue;
            }
            if (ui->tblGoods->comboBox(i, col_store)->currentData().toInt() != store) {
                continue;
            }
            if (outStoreDocId.isEmpty()) {
                dw.writeAHeader(outStoreDocId, outStoredocUserNum, DOC_STATE_DRAFT, DOC_TYPE_STORE_OUTPUT, __user->id(),
                                ui->leDate->date(),
                                QDate::currentDate(), QTime::currentTime(), 0, 0, outStoreDocComment, 0, ui->cbCurrency->currentData().toInt());
                dw.writeAHeaderStore(outStoreDocId, __user->id(), __user->id(), "", QDate::currentDate(), 0,
                                     ui->tblGoods->comboBox(i, col_store)->currentData().toInt(),
                                     1, "", 0, 0, uuid);
                outDocIds.append(outStoreDocId);
            }
            QString outDraftid;
            dw.writeAStoreDraft(outDraftid, outStoreDocId,
                                ui->tblGoods->comboBox(i, col_store)->currentData().toInt(), -1,
                                ui->tblGoods->getInteger(i, col_goods_code), ui->tblGoods->lineEdit(i, col_qty)->getDouble(),
                                0, 0, DOC_REASON_SALE, outDraftid, i + 1, "");
            db[":f_storerec"] = outDraftid;
            db.update("o_goods", "f_id", ui->tblGoods->getString(i, col_uuid));
        }
    }
    for (const QString &id : outDocIds) {
        //WRITE OTHER OUTPUT
        if (!dw.writeOutput(id, err)) {
            db.rollback();
            C5Message::error(err + "#1");
            return;
        }
        dw.updateField("a_header", "f_state", DOC_STATE_SAVED, "f_id", id);
    }
    for (QMap<QString, QString>::const_iterator it = outInPrices.constBegin(); it != outInPrices.constEnd(); it++) {
        db[":f_id"] = it.value();
        db.exec("select f_price from a_store_draft where f_id=:f_id");
        if (db.nextRow()) {
            db[":f_id"] = it.key();
            db[":f_price"] = db.getDouble("f_price");
            db.exec("update a_store_draft set f_price=:f_price, f_total=:f_price*f_qty where f_id=:f_id");
        } else {
            db.rollback();
            C5Message::error(tr("Error in document #1"));
            return;
        }
    }
    //    for (const QString &id: inDocsIds) {
    //        //WRITE OTHER OUTPUT
    //        if (!dw.writeInput(id, err)) {
    //            db.rollback();
    //            C5Message::error(err + "#2");
    //            return;
    //        }
    //        dw.updateField("a_header", "f_state", DOC_STATE_SAVED, "f_id", id);
    //    }
    //END OF OUTPUT FOR OTHER STORAGES
    //    //OUTPUT OF STORAGE
    //    QString storeDocComment = QString("%1 %2%3").arg(tr("Output of sale"), prefix, QString::number(hallid));
    //    QString storeDocId;
    //    QString storedocUserNum;
    //    storedocUserNum = dw.storeDocNum(DOC_TYPE_STORE_OUTPUT, ui->cbStorage->currentData().toInt(), true, 0);
    //    dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_DRAFT, DOC_TYPE_STORE_OUTPUT, __user->id(), QDate::currentDate(),
    //                    QDate::currentDate(), QTime::currentTime(), 0, 0, storeDocComment, 0, ui->cbCurrency->currentData().toInt());
    //    dw.writeAHeaderStore(storeDocId, __user->id(), __user->id(), "", QDate(), 0,
    //                         ui->cbStorage->currentData().toInt(),
    //                         1, "", 0, 0, uuid);
    //    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
    //        QString draftid;
    //        dw.writeAStoreDraft(draftid, storeDocId,
    //                            ui->cbStorage->currentData().toInt(), -1,
    //                            ui->tblGoods->getInteger(i, col_goods_code), ui->tblGoods->lineEdit(i, col_qty)->getDouble(),
    //                            0, 0, DOC_REASON_SALE, draftid, i + 1, "");
    //        db[":f_storerec"] = draftid;
    //        db.update("o_goods", "f_id", ui->tblGoods->getString(i, col_uuid));
    //    }
    //    if (!dw.writeOutput(storeDocId, err)) {
    //        db.rollback();
    //        C5Message::error(err + "#3");
    //        return;
    //    }
    //    dw.updateField("a_header", "f_state", DOC_STATE_SAVED, "f_id", storeDocId);
    //    //END OUTPUT OF STORAGE
    db.commit();
    ui->leUuid->setText(uuid);
    ui->leDocnumber->setText(QString("%1%2").arg(prefix, QString::number(hallid)));
    fOpenedFromDraft = false;
    db[":f_order"] = ui->leUuid->text();
    db.exec("delete from b_clients_debts where f_order=:f_order");
    if (fPartner.id.toInt() > 0) {
        db[":f_costumer"] = fPartner.id;
        db[":f_order"] = ui->leUuid->text();
        db[":f_amount"] = -1 * ui->leGrandTotal->getDouble();
        db[":f_date"] = ui->leDate->date();
        db[":f_currency"] = 1;
        db[":f_source"] = 1;
        db[":f_flag"] = ui->cbHall->currentData();
        db.insert("b_clients_debts", false);
    }
    if (isnew && !__c5config.httpServerIP().isEmpty()) {
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            QJsonObject jo;
            jo["action"] = 1; //MSG_GOODS_RESERVE;
            jo["goods"] = ui->tblGoods->getInteger(i, col_goods_code);
            jo["qty"] = ui->tblGoods->getDouble(i, col_qty);
            jo["goodsname"] = ui->tblGoods->getString(i, col_name);
            jo["scancode"] = ui->tblGoods->getString(i, col_barcode);
            jo["unit"] = ui->tblGoods->getString(i, col_unit);
            jo["usermessage"] = tr("Online shop") + " " + ui->leDocnumber->text();
            jo["enddate"] = ui->leDate->date().addDays(7).toString(FORMAT_DATE_TO_STR);
            QJsonDocument jdoc(jo);
            auto *t = new ThreadSendMessage();
            connect(t, SIGNAL(result(QJsonObject)), this, SLOT(messageResult(QJsonObject)));
            t->send(ui->tblGoods->comboBox(i, col_store)->currentData().toInt(),
                    jdoc.toJson(QJsonDocument::Compact));
            if (ui->tblGoods->comboBox(i, col_store)->currentData().toInt() != ui->cbStorage->currentData().toInt()) {
                jo["usermessage"] = ui->tblGoods->comboBox(i, col_store)->currentText() + " " + ui->leDocnumber->text();
                jdoc = QJsonDocument(jo);
                auto *t = new ThreadSendMessage();
                connect(t, SIGNAL(result(QJsonObject)), this, SLOT(messageResult(QJsonObject)));
                t->send(ui->cbStorage->currentData().toInt(),
                        jdoc.toJson(QJsonDocument::Compact));
            }
        }
    }
    for (int r = 0; r < ui->tblGoods->rowCount(); r++) {
        for (int c = 0; c < ui->tblGoods->columnCount(); c++) {
            QWidget *w = ui->tblGoods->cellWidget(r, c);
            if (w) {
                w->setEnabled(false);
            }
        }
    }
    ui->wtoolbar->setEnabled(false);
    ui->paymentFrame->setEnabled(false);
    fActionSave->setEnabled(false);
    fActionDraft->setEnabled(true);
    fActionCopy->setEnabled(true);
    C5Message::info(tr("Saved"));
}

void C5SaleDoc::uuidDoubleClicked()
{
    qApp->clipboard()->setText(ui->leUuid->text());
}

void C5SaleDoc::on_PriceTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    countGrandTotal();
}

void C5SaleDoc::on_QtyTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    countGrandTotal();
}

void C5SaleDoc::on_discountValueChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    countGrandTotal();
}

void C5SaleDoc::on_leCmd_returnPressed()
{
    QString code = ui->leCmd->text().replace("?", "").replace(";", "");
    ui->leCmd->clear();
    C5Database db(fDBParams);
    db[":f_scancode"] = code;
    db.exec("select f_id from c_goods where f_scancode=:f_scancode");
    if (db.nextRow()) {
        addGoods(db.getInt(0), db);
        return;
    }
}

int C5SaleDoc::addGoods(int goodsId, C5Database &db)
{
    QString priceField, priceDiscount;
    switch (fMode) {
        case PRICEMODE_RETAIL:
            priceField = "f_price1";
            priceDiscount = "f_price1disc";
            break;
        case PRICEMODE_WHOSALE:
            priceField = "f_price2";
            priceDiscount = "f_price2disc";
            break;
        default:
            priceField = "f_price1";
            priceDiscount = "f_price1disc";
            break;
    }
    db[":f_id"] = goodsId;
    db[":f_currency"] = ui->cbCurrency->currentData();
    db.exec(QString("select g.*, gu.f_name as f_unitname, if(gpr.%1>0, gpr.%1, gpr.%2) as f_price "
                    "from c_goods g "
                    "left join c_goods_prices gpr on gpr.f_goods=g.f_id "
                    "left join c_units gu on gu.f_id=g.f_unit "
                    "where g.f_id=:f_id and gpr.f_currency=:f_currency").arg(priceDiscount, priceField));
    if (db.nextRow()) {
        return addGoods("", ui->cbStorage->currentData().toInt(), goodsId, db.getString("f_scancode"), db.getString("f_name"),
                        db.getString("f_unitname"), 0, db.getDouble("f_price"), 0, db.getInt("f_service"), db.getString("f_returnfrom"));
    } else {
        C5Message::error(tr("Invalid goods id"));
        return false;
    }
}

int C5SaleDoc::addGoods(const QString &uuid, int store, int goodsId, const QString &barcode, const QString &name,
                        const QString &unitname,
                        double qty, double price, double discount, int isService, const QString &returnFrom)
{
    int r = ui->tblGoods->addEmptyRow();
    ui->tblGoods->setString(r, col_uuid, uuid);
    ui->tblGoods->createCheckbox(r, col_checkbox);
    ui->tblGoods->setInteger(r, col_goods_code, goodsId);
    C5ComboBox *cs = ui->tblGoods->createComboBox(r, col_store);
    ui->tblGoods->setString(r, col_barcode, barcode);
    ui->tblGoods->setString(r, col_name, name);
    C5LineEdit *l = ui->tblGoods->createLineEdit(r, col_qty);
    l->setValidator(new QDoubleValidator(0, 999999999, 3));
    l->setDouble(qty);
    ui->tblGoods->setString(r, col_unit, unitname);
    ui->tblGoods->createLineEdit(r, col_price)->setDouble(price);
    l = ui->tblGoods->createLineEdit(r, col_discount_value);
    l->setValidator(new QDoubleValidator(0, 999999999, 3));
    l->setDouble(fPartner.permanentDiscount);
    l->setDouble(discount);
    l = ui->tblGoods->createLineEdit(r, col_discount_amount);
    l->setValidator(new QDoubleValidator(0, 999999999, 3));
    l->setReadOnly(true);
    l = ui->tblGoods->createLineEdit(r, col_grandtotal);
    l->setValidator(new QDoubleValidator(0, 999999999, 3));
    l->setDouble(qty *price);
    l->setReadOnly(true);
    connect(ui->tblGoods->lineEdit(r, col_qty), &C5LineEdit::textEdited, this, &C5SaleDoc::on_QtyTextChanged);
    connect(ui->tblGoods->lineEdit(r, col_price), &C5LineEdit::textEdited, this, &C5SaleDoc::on_PriceTextChanged);
    connect(ui->tblGoods->lineEdit(r, col_discount_value), &C5LineEdit::textEdited, this,
            &C5SaleDoc::on_discountValueChanged);
    for (int i = 0; i < ui->cbStorage->count(); i++) {
        cs->addItem(ui->cbStorage->itemText(i), ui->cbStorage->itemData(i));
    }
    cs->setCurrentIndex(cs->findData(store));
    if (fSpecialPrices.contains(goodsId)) {
        ui->tblGoods->lineEdit(r, col_price)->setDouble(fSpecialPrices[goodsId]);
        ui->tblGoods->lineEdit(r, col_discount_value)->setDouble(0);
    }
    ui->tblGoods->setInteger(r, col_type, isService);
    ui->tblGoods->setString(r, col_returnfrom, returnFrom);
    countGrandTotal();
    return r;
}

void C5SaleDoc::countGrandTotal()
{
    double grandTotal = 0;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        double total = ui->tblGoods->lineEdit(i, col_qty)->getDouble() * ui->tblGoods->lineEdit(i, col_price)->getDouble();
        double disc = total *(ui->tblGoods->lineEdit(i, col_discount_value)->getDouble() / 100);
        ui->tblGoods->lineEdit(i, col_discount_amount)->setDouble(disc);
        total -= disc;
        ui->tblGoods->lineEdit(i, col_grandtotal)->setDouble(total);
        grandTotal += total;
    }
    ui->leGrandTotal->setDouble(grandTotal);
}

bool C5SaleDoc::openDraft(const QString &id)
{
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select * from o_draft_sale where f_id=:f_id");
    if (!fDraftSale.getRecord(db)) {
        C5Message::error(tr("Invalid draft document id"));
        return false;
    }
    if (fDraftSale.state != 1) {
        return openDoc(id);
    }
    switch (db.getInt("f_saletype")) {
        case 1:
            ui->leSaleType->setText(tr("Retail"));
            break;
        case 2:
            ui->leSaleType->setText(tr("Whosale"));
            break;
    }
    ui->flag->setVisible(db.getInt("f_flag") == 1);
    if (db.getInt("f_state") > 1) {
        fToolBar->actions().at(0)->setEnabled(false);
    }
    fPartner.queryRecordOfId(db, fDraftSale.partner);
    setPartner();
    ui->leDate->setDate(fDraftSale.date);
    ui->leTime->setText(fDraftSale.time.toString(FORMAT_TIME_TO_STR));
    ui->leComment->setText(fDraftSale.comment);
    ui->leUuid->setText(fDraftSale.id.toString());
    ui->leDelivery->setText(fDraftSale.deliveryDate.toString(FORMAT_DATE_TO_STR));
    setDeliveryMan();
    QString priceField = "f_price1";
    db[":f_header"] = id;
    db[":f_state"] = 1;
    db[":f_currency"] = ui->cbCurrency->currentData();
    db.exec(QString("select dsb.f_store, dsb.f_qty, g.*, gu.f_name as f_unitname, dsb.f_price, dsb.f_discount "
                    "from o_draft_sale_body dsb "
                    "left join c_goods g on g.f_id=dsb.f_goods "
                    "left join c_goods_prices gpr on gpr.f_goods=g.f_id "
                    "left join c_units gu on gu.f_id=g.f_unit "
                    "where dsb.f_header=:f_header and gpr.f_currency=:f_currency "
                    "and dsb.f_state=:f_state and dsb.f_qty>0 ").arg(priceField));
    ui->tblGoods->setRowCount(0);
    while (db.nextRow()) {
        addGoods("", db.getInt("f_store"), db.getInt("f_id"), db.getString("f_scancode"), db.getString("f_name"),
                 db.getString("f_unitname"), db.getDouble("f_qty"), db.getDouble("f_price"), db.getDouble("f_discount"), 0, "");
    }
    fOpenedFromDraft = true;
    switch (fDraftSale.payment) {
        case 1:
            ui->leCash->setDouble(ui->leGrandTotal->getDouble());
            break;
        case 2:
            ui->leCard->setDouble(ui->leGrandTotal->getDouble());
            break;
        case 3:
            ui->leBankTransfer->setDouble(ui->leGrandTotal->getDouble());
            break;
        case 6:
            ui->leCard->setDouble(ui->leGrandTotal->getDouble());
            break;
        case 7:
            ui->leDebt->setDouble(ui->leGrandTotal->getDouble());
            break;
    }
    fActionSave->setEnabled(true);
    fActionCopy->setEnabled(false);
    fActionDraft->setEnabled(false);
    for (int r = 0; r < ui->tblGoods->rowCount(); r++) {
        for (int c = 0; c < ui->tblGoods->columnCount(); c++) {
            QWidget *w = ui->tblGoods->cellWidget(r, c);
            if (w) {
                w->setEnabled(true);
            }
        }
    }
    ui->wtoolbar->setEnabled(true);
    ui->paymentFrame->setEnabled(true);
    return true;
}

void C5SaleDoc::setPartner()
{
    ui->leTaxpayerName->setText(QString("%1, %2, %3").arg(fPartner.categoryName, fPartner.groupName, fPartner.taxName));
    ui->leTaxpayerId->setText(fPartner.taxCode);
    setMode(fPartner.pricePolitic);
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        ui->tblGoods->lineEdit(i, col_discount_value)->setDouble(fPartner.permanentDiscount);
    }
    fSpecialPrices.clear();
    C5Database db(fDBParams);
    db[":f_partner"] = fPartner.id;
    db.exec("select f_goods, f_price from c_goods_special_prices where f_partner=:f_partner and f_goods not in (select f_id from c_goods where f_nospecial_price=1)");
    while (db.nextRow()) {
        fSpecialPrices[db.getInt("f_goods")] = db.getDouble("f_price");
    }
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (fSpecialPrices.contains(ui->tblGoods->getInteger(i, col_goods_code))) {
            ui->tblGoods->lineEdit(i, col_price)->setDouble(fSpecialPrices[ui->tblGoods->getInteger(i, col_goods_code)]);
            ui->tblGoods->lineEdit(i, col_discount_value)->setDouble(0);
        }
    }
    countGrandTotal();
}

void C5SaleDoc::setPartner(const CPartners &p)
{
    Q_UNUSED(p);
    ui->leTaxpayerName->setText(QString("%1, %2, %3").arg(fPartner.categoryName, fPartner.groupName, fPartner.taxName));
    ui->leTaxpayerId->setText(fPartner.taxCode);
    //setMode(fPartner.pricePolitic);
}

void C5SaleDoc::setDeliveryMan()
{
    ui->leDeluveryMan->clear();
    if (fDraftSale.staff > 0) {
        C5Database db(fDBParams);
        db[":f_id"] = fDraftSale.staff;
        db.exec("select concat_ws(' ', f_last, f_first) as f_fullname from s_user where f_id=:f_id");
        db.nextRow();
        ui->leDeluveryMan->setText(db.getString("f_fullname"));
    }
}

void C5SaleDoc::exportToAs(int doctype)
{
    C5Database db(fDBParams);
    db.exec("select * from as_list");
    if (db.rowCount() == 0) {
        C5Message::error(tr("ArmSoft is not configure"));
        return;
    }
    int dbid;
    int index = 0;
    QString connStr;
    if (db.rowCount() > 0) {
        QStringList dbNames;
        while (db.nextRow()) {
            dbNames.append(db.getString("f_name"));
        }
        index = DlgList2::indexOfList(tr("Armsoft database"), fDBParams, dbNames);
        if (index < 0) {
            return;
        }
        dbid = db.getInt(index, "f_id");
        connStr = db.getString(index, "f_connectionstring");
    } else {
        dbid = db.getInt(0, "f_id");
        connStr = db.getString(0, "f_connectionstring");
    }
    BreezeConfig *b = Configs::construct<BreezeConfig>(fDBParams, 1);
    QJsonObject jo;
    jo["pkServerAPIKey"] = b->apiKey;
    jo["pkFcmToken"] = "0123456789";
    jo["pkUsername"] = b->username;
    jo["pkPassword"] = b->password;
    jo["pkAction"] = 14;
    jo["asconnectionstring"] = connStr;
    jo["asdbid"] = dbid;
    jo["draftid"] = ui->leUuid->text();
    jo["doctype"] = doctype;
    jo["lesexpenseacc"] = __c5config.getRegValue("lesexpenseacc", "").toString();
    jo["lesincomeacc"] = __c5config.getRegValue("lesincomeacc", "").toString();
    jo["lemexpenseacc"] = __c5config.getRegValue("lemexpenseacc", "").toString();
    jo["lemincomeacc"] = __c5config.getRegValue("lemincomeacc", "").toString();
    QJsonObject jdb;
    jdb["host"] = fDBParams.at(0);
    jdb["schema"] = fDBParams.at(1);
    jdb["username"] = fDBParams.at(2);
    jdb["password"] = fDBParams.at(3);
    jo["database"] = jdb;
    jo["vatpercent"] = index == 0 ? (doctype == 5 ? 0.2 : 0.1667) : 0;
    jo["vattype"] = index == 0 ? (doctype == 5 ? "1" : "5") : "3";
    jo["pricewithoutvat"] = index == 0 ? (doctype == 5 ? 1.2 : 1) : 1;
    jo["withvat"] = index == 0 ? (doctype == 5 ? 0.2 : 0) : 0;
    HttpQueryDialog *qd = new HttpQueryDialog(fDBParams, QString("https://%1:%2/magnit").arg(b->ipAddress,
        QString::number(b->port)), jo, this);
    qd->exec();
    qd->deleteLater();
}

void C5SaleDoc::on_btnAddGoods_clicked()
{
    QList<QVariant> vals;
    if (!C5Selector::getValueOfColumn(fDBParams, cache_goods, vals, 3)) {
        return;
    }
    if (vals.at(1).toInt() == 0) {
        C5Message::error(tr("Could not add goods without code"));
        return;
    }
    C5Database db(fDBParams);
    addGoods(vals.at(1).toInt(), db);
}

void C5SaleDoc::on_btnRemoveGoods_clicked()
{
    for (int i = ui->tblGoods->rowCount() - 1; i > -1; i--) {
        if (ui->tblGoods->checkBox(i, col_checkbox)->isChecked()) {
            ui->tblGoods->removeRow(i);
        }
    }
    countGrandTotal();
}

void C5SaleDoc::on_btnEditGoods_clicked()
{
    int row = ui->tblGoods->currentRow();
    if (row < 0) {
        return;
    }
    if (ui->tblGoods->getInteger(row, col_goods_code) == 0) {
        return;
    }
    CE5Goods *ep = new CE5Goods(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    ep->setId(ui->tblGoods->getInteger(row, col_goods_code));
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        if (data.at(0)["f_id"].toInt() == 0) {
            C5Message::error(tr("Cannot change goods without code"));
            return;
        }
        ui->tblGoods->setData(row, col_goods_code, data.at(0)["f_id"]);
        ui->tblGoods->setData(row, col_name, data.at(0)["f_name"]);
        ui->tblGoods->setData(row, col_unit, data.at(0)["f_unitname"]);
        ui->tblGoods->lineEdit(row, col_qty)->setFocus();
    }
    delete e;
}

void C5SaleDoc::on_btnNewGoods_clicked()
{
    CE5Goods *ep = new CE5Goods(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        if (data.at(0)["f_id"].toInt() == 0) {
            C5Message::error(tr("Cannot add goods without code"));
            return;
        }
        C5Database db(fDBParams);
        addGoods(data.at(0)["f_id"].toInt(), db);
    }
    delete e;
    countGrandTotal();
}

void C5SaleDoc::on_cbHall_currentIndexChanged(int index)
{
    ui->cbCashDesk->clear();
    ui->cbCashDesk->setDBValues(fDBParams,
                                QString("select f_id, f_name from h_tables where f_hall=%1").arg(ui->cbHall->itemData(index).toInt()));
    if (ui->cbHall->itemData(index).toInt() == __c5config.getValue(param_default_hall).toInt()) {
        ui->cbCashDesk->setCurrentIndex(ui->cbCashDesk->findData(__c5config.getValue(param_default_table).toInt()));
    }
}

void C5SaleDoc::on_btnSearchTaxpayer_clicked()
{
    QList<QVariant> values;
    if (!C5Selector::getValue(fDBParams, cache_goods_partners, values)) {
        return;
    }
    if (values.count() == 0) {
        return;
    }
    C5Database db(fDBParams);
    fPartner.queryRecordOfId(db, values.at(1));
    setPartner();
}

void C5SaleDoc::on_btnRemoveDelivery_clicked()
{
    ui->leDelivery->clear();
}

void C5SaleDoc::on_btnDelivery_clicked()
{
    QDate d;
    if (C5DateRange::date(d)) {
        ui->leDelivery->setText(d.toString(FORMAT_DATE_TO_STR));
        fDraftSale.deliveryDate = d;
    }
}

void C5SaleDoc::on_btnEditPartner_clicked()
{
    if (fPartner.id.toInt() > 0) {
        CE5Partner *ep = new CE5Partner(fDBParams);
        C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
        ep->setId(fPartner.id.toInt());
        QList<QMap<QString, QVariant> > data;
        if(e->getResult(data)) {
            if (data.at(0)["f_id"].toInt() == 0) {
                C5Message::error(tr("Cannot change partner without code"));
                return;
            }
            C5Database db(fDBParams);
            fPartner.queryRecordOfId(db, fPartner.id);
            setPartner();
        }
        delete e;
    }
}

void C5SaleDoc::on_btnEditAccounts_clicked()
{
    ArmSoftExportOptions d(fDBParams);
    d.exec();
}

void C5SaleDoc::on_leCash_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if (ui->leCash->getDouble() > ui->leGrandTotal->getDouble()) {
        ui->leCash->setDouble(ui->leGrandTotal->getDouble());
    }
}

void C5SaleDoc::on_btnDeliveryMan_clicked()
{
    QList<QVariant> vals;
    C5Selector::getValue(fDBParams, cache_users, vals);
    if (vals.count() == 0) {
        return;
    }
    fDraftSale.staff = vals.at(1).toInt();
    setDeliveryMan();
}

void C5SaleDoc::saveReturnItems()
{
    C5Database db(fDBParams);
    C5StoreDraftWriter dw(db);
    OHeader oheader;
    oheader.id = ui->leUuid->text();
    oheader.staff = __user->id();
    oheader.state = ORDER_STATE_CLOSE;
    oheader.amountTotal = ui->leGrandTotal->getDouble() * -1;
    oheader.amountCash = ui->leCash->getDouble() * -1;
    oheader.amountBank = ui->leBankTransfer->getDouble() * -1;
    oheader.saleType = SALE_RETURN;
    oheader.hall = ui->cbHall->currentData().toInt();
    if (!dw.hallId(oheader.prefix, oheader.hallId, ui->cbHall->currentData().toInt())) {
        C5Message::error(dw.fErrorMsg);
        db.rollback();
        return;
    }
    QString err;
    if (!oheader.write(db, err)) {
        C5Message::error(err);
        db.rollback();
        return;
    }
    bool empty = true;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        empty = false;
        OGoods g;
        g.header = oheader._id();
        g.id = ui->tblGoods->getString(i, col_uuid);
        g.store = ui->tblGoods->comboBox(i, col_store)->currentData().toInt();
        g.goods = ui->tblGoods->getInteger(i, col_goods_code);
        g.qty = ui->tblGoods->lineEdit(i, col_qty)->getDouble();
        g.price = ui->tblGoods->lineEdit(i, col_price)->getDouble();
        g.total = ui->tblGoods->lineEdit(i, col_grandtotal)->getDouble();
        g.sign = -1;
        g.row = i + 1;
        g.storeRec = "";
        g.returnFrom = ui->tblGoods->getString(i, col_returnfrom);
        if (!g.write(db, err)) {
            C5Message::error(err);
            db.rollback();
            return;
        }
    }
    db.commit();
    QJsonObject js;
    js["id"] = oheader._id();
    js["user"] = __user->id();
    js["storein"] = ui->cbStorage->currentData().toInt();
    js["currency"] = ui->cbCurrency->currentData().toInt();
    js["date"] = ui->leDate->date().toString(FORMAT_DATE_TO_STR_MYSQL);
    db.exec(QString("call sf_create_return_of_sale('%1')").arg(json_str(js)));
    fActionSave->setEnabled(false);
    fActionDraft->setEnabled(false);
    C5Message::info(tr("Saved"));
}

void C5SaleDoc::saveAsDraft()
{
    if (C5Message::question(tr("Confirm to make a draft")) != QDialog::Accepted) {
        return;
    }
    fHttp->createHttpQuery("/engine/shop/make-draft.php", QJsonObject{{"id", ui->leUuid->text()}}, SLOT(makeDraftResponse(
                QJsonObject)));
}

void C5SaleDoc::saveCopy()
{
    QString err;
    C5Database db(fDBParams);
    db.startTransaction();
    fDraftSale.id = "";
    fDraftSale.state = 1;
    fDraftSale.saleType = ui->leSaleType->property("id").toInt();
    fDraftSale.date = ui->leDate->date();
    fDraftSale.time = QTime::currentTime();
    fDraftSale.cashier = __user->id();
    fDraftSale.staff = ui->leDeluveryMan->property("id").toInt();
    fDraftSale.amount = ui->leGrandTotal->getDouble();
    fDraftSale.comment = ui->leComment->text();
    fDraftSale.payment = 1;
    fDraftSale.partner = fPartner.id.toInt();
    fDraftSale.discount = 0;
    fDraftSale.deliveryDate = QDate::fromString(ui->leDelivery->text(), "dd/MM/yyyy");
    if (!fDraftSale.write(db, err)) {
        db.rollback();
        C5Message::error(err);
        return;
    }
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        ui->tblGoods->setString(i, col_uuid, "");
        fDraftSaleBody.id = "";
        fDraftSaleBody.header = fDraftSale.id.toString();
        fDraftSaleBody.state = 1;
        fDraftSaleBody.store = ui->tblGoods->comboBox(i, col_store)->currentData().toInt();
        fDraftSaleBody.dateAppend = QDate::currentDate();
        fDraftSaleBody.timeAppend = QTime::currentTime();
        fDraftSaleBody.goods = ui->tblGoods->getInteger(i, col_goods_code);
        fDraftSaleBody.qty = ui->tblGoods->lineEdit(i, col_qty)->getDouble();
        fDraftSaleBody.price = ui->tblGoods->lineEdit(i, col_price)->getDouble();
        fDraftSaleBody.discount = ui->tblGoods->lineEdit(i, col_discount_value)->getDouble();
        fDraftSaleBody.userAppend = __user->id();
        if (!fDraftSaleBody.write(db, err)) {
            db.rollback();
            C5Message::error(err);
            return;
        }
    }
    db.commit();
    auto *doc = __mainWindow->createTab<C5SaleDoc>(fDBParams);
    doc->openDraft(fDraftSale.id.toString());
}

void C5SaleDoc::removeDoc()
{
    if (C5Message::question(tr("Confirm to remove document")) != QDialog::Accepted) {
        return;
    }
    fHttp->createHttpQuery("/engine/shop/remove-order.php", QJsonObject{{"id", ui->leUuid->text()}}, SLOT(removeDocResponse(
                QJsonObject)));
}

void C5SaleDoc::on_btnQr_clicked()
{
    QString s = QInputDialog::getText(this, tr("Emarks"), "");
    if (s.isEmpty()) {
        return;
    }
    fEmarks.append(s);
}
