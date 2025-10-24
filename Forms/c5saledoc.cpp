#include "c5saledoc.h"
#include "c5checkbox.h"
#include "ui_c5saledoc.h"
#include "c5message.h"
#include "c5utils.h"
#include "c5cache.h"
#include "c5config.h"
#include "c5selector.h"
#include "c5user.h"
#include "jsons.h"
#include "c5printrecipta4.h"
#include "ce5goods.h"
#include "c5storedraftwriter.h"
#include "c5daterange.h"
#include "breezeconfig.h"
#include "armsoftexportoptions.h"
#include "dlglist2.h"
#include "chatmessage.h"
#include "httpquerydialog.h"
#include "c5mainwindow.h"
#include "nloadingdlg.h"
#include "ce5partner.h"
#include "outputofheader.h"
#include "../../NewTax/Src/printtaxn.h"
#include "oheader.h"
#include "c5storedoc.h"
#include <QXlsx/header/xlsxdocument.h>
#include <QClipboard>
#include <QSqlQuery>
#include <QInputDialog>
#include <QFileDialog>
#include <QDesktopServices>
#include <QDomDocument>

#define col_uuid 0
#define col_checkbox 1
#define col_goods_code 2
#define col_none 3
#define col_barcode 4
#define col_name 5
#define col_qty 6
#define col_unit 7
#define col_price 8
#define col_discount_value 9
#define col_discount_amount 10
#define col_grandtotal 11
#define col_isservice 12
#define col_returnfrom 13
#define col_emarks 14

#define float_str_(value, f) QString::number(value, 'f', f)

C5SaleDoc::C5SaleDoc(QWidget *parent) :
    C5Widget(parent),
    ui(new Ui::C5SaleDoc)
{
    ui->setupUi(this);
    fIcon = ":/pricing.png";
    fLabel = tr("Sale");
    ui->tblGoods->setColumnWidths(ui->tblGoods->columnCount(), 0, 50, 0, 150, 200, 300, 80, 80, 80, 80, 80, 80, 0, 0);
    ui->cbCurrency->setDBValues("select f_id, f_name from e_currency");
    ui->cbCurrency->setCurrentIndex(ui->cbCurrency->findData(__c5config.getValue(param_default_currency)));
    ui->cbStorage->setDBValues("select f_id, f_name from c_storages order by 2");
    ui->cbStorage->setCurrentIndex(-1);
    ui->cbHall->setDBValues("select f_id, f_name from h_halls order by 2");
    ui->cbHall->setCurrentIndex(ui->cbHall->findData(__c5config.getValue(param_default_hall).toInt()));
    ui->cbStorage->setCurrentIndex(ui->cbStorage->findData(__c5config.getValue(param_default_store).toInt()));
    ui->cbCashDesk->setCurrentIndex(ui->cbCashDesk->findData(__c5config.getValue(param_default_table).toInt()));
    ui->tblGoods->setColumnHidden(col_none, true);
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

    switch(fMode) {
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
    if(handleId == REPORT_HANDLER_SALE_DOC_OPEN_DRAFT) {
        if(!openDraft(data.toString())) {
            return false;
        }
    }

    return true;
}

QToolBar* C5SaleDoc::toolBar()
{
    if(!fToolBar) {
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
        fPrintReturnTax = fToolBar->addAction(QIcon(":/fiscal.png"), tr("Return fiscal"), this, SLOT(cancelFiscal()));
        fToolBar->addAction(QIcon(":/excel.png"), tr("Export to Excel"), this, SLOT(exportToExcel()));
        fToolBar->addAction(QIcon(":/whosale.png"), tr("Create store\r\ndocument"), this, SLOT(createStoreDocument()));
        fActionReturn = fToolBar->addAction(QIcon(":/trading.png"), tr("Return"), this, SLOT(returnItems()));
        fToolBar->addAction(QIcon(":/xml.png"), tr("Export to XML"), this, SLOT(exportXML()));
    }

    return fToolBar;
}

bool C5SaleDoc::openDoc(const QString &uuid)
{
    NLoadingDlg loadingDlg(this);
    loadingDlg.setWindowModality(Qt::ApplicationModal);
    loadingDlg.show();
    C5Database db;
    //GOODS
    db[":f_header"] = uuid;
    db.exec(QString("select dsb.f_id as ogoodsid, dsb.f_store, dsb.f_qty, g.*, "
                    "gu.f_name as f_unitname, dsb.f_price, dsb.f_discountfactor,  "
                    "dsb.f_returnfrom "
                    "from o_goods dsb "
                    "left join c_goods g on g.f_id=dsb.f_goods "
                    "left join c_units gu on gu.f_id=g.f_unit "
                    "where dsb.f_header=:f_header  "
                    "order by dsb.f_row "));

    while(db.nextRow()) {
        addGoods(db.getString("ogoodsid"),
                 db.getInt("f_store"),
                 db.getInt("f_id"),
                 db.getString("f_scancode"),
                 db.getString("f_name"),
                 db.getString("f_unitname"),
                 db.getDouble("f_qty"),
                 db.getDouble("f_price"),
                 db.getDouble("f_discountfactor") * 100, 0,
                 db.getString("f_returnfrom"));
    }

    //HEADER
    db[":f_id"] = uuid;
    db.exec("select * from o_header where f_id=:f_id");
    OHeader o;

    if(!o.getRecord(db)) {
        C5Message::error(tr("Invalid draft document id"));
        return false;
    }

    ui->cbHall->setCurrentIndex(ui->cbHall->findData(o.hall));
    ui->cbCashDesk->setCurrentIndex(ui->cbCashDesk->findData(o.table));
    ui->leCashier->setProperty("f_cashier", db.getInt("f_cashier"));
    setMode(db.getInt("f_saletype"));

    if(db.getInt("f_state") > 1) {
        fToolBar->actions().at(0)->setEnabled(false);
    }

    ui->leDocnumber->setText(QString("%1%2").arg(db.getString("f_prefix")).arg(db.getInt("f_hallid")));
    ui->leDocnumber->setProperty("f_hallid", db.getInt("f_hallid"));
    ui->leDocnumber->setProperty("f_prefix", db.getString("f_prefix"));
    ui->leCash->setDouble(db.getDouble("f_amountcash"));
    ui->leCard->setDouble(db.getDouble("f_amountcard"));
    ui->leBankTransfer->setDouble(db.getDouble("f_amountbank"));
    ui->leDebt->setDouble(db.getDouble("f_amountdebt"));
    ui->lePrepaid->setDouble(db.getDouble("f_amountprepaid"));
    ui->leGrandTotal->setDouble(db.getDouble("f_amounttotal"));
    //CASHIER
    db[":f_id"] = ui->leCashier->property("f_cashier");
    db.exec("select * from s_user where f_id=:f_id");
    db.nextRow();
    ui->leCashier->setText(QString("%1 %2").arg(db.getString("f_last"), db.getString("f_first")));
    fPartner.queryRecordOfId(db, o.partner);
    setPartner(fPartner);
    ui->leDate->setDate(o.dateCash);
    ui->leTime->setText(o.timeClose.toString(FORMAT_TIME_TO_STR));
    ui->leComment->setText(o.comment);
    ui->leUuid->setText(o.id.toString());
    //DELIVERY INFO
    db[":f_id"] = uuid;
    db.exec("select * from o_draft_sale where f_id=:f_id");

    if(db.nextRow()) {
        ui->leDelivery->setText(db.getDate("f_datefor").toString(FORMAT_DATE_TO_STR));
        db[":f_id"] = db.getInt("f_staff");
        db.exec("select f_id, concat_ws(' ', f_last, f_first) as f_fullname from s_user where f_id=:f_id");

        if(db.nextRow()) {
            ui->leDeluveryMan->setProperty("id", db.getInt("f_id"));
            ui->leDeluveryMan->setText(db.getString("f_fullname"));
        }
    }

    //EMARKS
    db[":f_header"] = uuid;
    db.exec("select * from o_qr where f_header = :f_header");

    while(db.nextRow()) {
        fEmarks.append(QByteArray::fromBase64(db.getString("f_qr").toUtf8()
                                              .replace("https://baristamarket.am/?fbclid=", "")));
    }

    fActionSave->setEnabled(o.state != ORDER_STATE_CLOSE);
    //fActionDraft->setEnabled(o.state == ORDER_STATE_CLOSE);
    fActionCopy->setEnabled(true);

    for(int r = 0; r < ui->tblGoods->rowCount(); r++) {
        for(int c = 0; c < ui->tblGoods->columnCount(); c++) {
            QWidget *w = ui->tblGoods->cellWidget(r, c);

            if(w) {
                w->setEnabled(o.state != ORDER_STATE_CLOSE);
            }
        }
    }

    ui->wtoolbar->setEnabled(o.state != ORDER_STATE_CLOSE);
    ui->paymentFrame->setEnabled(o.state != ORDER_STATE_CLOSE);
    countTotalQty();
    getFiscalNum();
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
    static_cast<C5LineEdit*>(sender())->setDouble(ui->leGrandTotal->getDouble());
}

void C5SaleDoc::createStoreDocument()
{
    C5Database db;
    QHash<int, double> prices1;
    db.exec("select f_id, f_lastinputprice from c_goods");

    while(db.nextRow()) {
        prices1[db.getInt(0)] = db.getDouble(1);
    }

    auto *sd = __mainWindow->createTab<C5StoreDoc>();
    sd->setMode(C5StoreDoc::sdInput);
    sd->setStore(0, 0);
    sd->setReason(DOC_REASON_INPUT);
    sd->setComment(ui->leComment->text());

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        sd->addGoods(ui->tblGoods->getInteger(i, col_goods_code),
                     ui->tblGoods->getString(i, col_name),
                     ui->tblGoods->lineEdit(i, col_qty)->getDouble(),
                     ui->tblGoods->getString(i, col_unit),
                     0, 0, "");
    }
}

void C5SaleDoc::printSale()
{
    C5PrintReciptA4 p(ui->leUuid->text(), this);
    QString err;
    p.print(err);

    if(!err.isEmpty()) {
        C5Message::error(err);
    }
}

void C5SaleDoc::exportXML()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export to XML"), "", "*.xml");

    if(fileName.isEmpty()) {
        return;
    }

    QDomDocument doc;
    // Декларация
    QDomProcessingInstruction decl = doc.createProcessingInstruction("xml", "version=\"1.0\"");
    doc.appendChild(decl);
    // Корневой элемент
    QDomElement root = doc.createElement("ExportedData");
    root.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    root.setAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
    root.setAttribute("xmlns", "http://www.taxservice.am/tp3/invoice/definitions");
    doc.appendChild(root);
    // Invoice
    QDomElement invoice = doc.createElement("Invoice");
    invoice.setAttribute("Version", "1.0");
    root.appendChild(invoice);
    // Type
    invoice.appendChild(doc.createElement("Type")).appendChild(doc.createTextNode("1"));
    invoice.appendChild(doc.createElement("Traceable")).appendChild(doc.createTextNode("false"));
    // GeneralInfo
    QDomElement generalInfo = doc.createElement("GeneralInfo");
    invoice.appendChild(generalInfo);
    generalInfo.appendChild(doc.createElement("ParentInvoiceNumber"));
    generalInfo.appendChild(doc.createElement("EcrReceipt"));
    generalInfo.appendChild(doc.createElement("SupplyDate"))
               .appendChild(doc.createTextNode(ui->leDate->date().toString("yyyy-MM-dd") + "+04:00"));
    generalInfo.appendChild(doc.createElement("Procedure")).appendChild(doc.createTextNode("2"));
    QDomElement dealInfo = doc.createElement("DealInfo");
    dealInfo.appendChild(doc.createElement("DealNumber"));
    generalInfo.appendChild(dealInfo);
    generalInfo.appendChild(doc.createElement("AdditionalData"));
    generalInfo.appendChild(doc.createElement("AdjustmentAccount")).appendChild(doc.createTextNode("false"));
    // SupplierInfo
    QDomElement supplierInfo = doc.createElement("SupplierInfo");
    invoice.appendChild(supplierInfo);
    supplierInfo.appendChild(doc.createElement("VATNumber"));
    QDomElement supplierTaxpayer = doc.createElement("Taxpayer");
    supplierInfo.appendChild(supplierTaxpayer);
    supplierTaxpayer.appendChild(doc.createElement("TIN"))
                    .appendChild(doc.createTextNode(C5Config::fMainJson["companytin"].toString()));
    supplierTaxpayer.appendChild(doc.createElement("Name"))
                    .appendChild(doc.createTextNode(C5Config::fMainJson["companyname"].toString()));
    supplierTaxpayer.appendChild(doc.createElement("Address"))
                    .appendChild(doc.createTextNode(C5Config::fMainJson["companyaddress"].toString()));
    QDomElement supplierBank = doc.createElement("BankAccount");
    supplierBank.appendChild(doc.createElement("BankName"))
                .appendChild(doc.createTextNode(C5Config::fMainJson["companybank"].toString()));
    supplierBank.appendChild(doc.createElement("BankAccountNumber"))
                .appendChild(doc.createTextNode(C5Config::fMainJson["companybankaccount"].toString()));
    supplierTaxpayer.appendChild(supplierBank);
    supplierTaxpayer.appendChild(doc.createElement("AdditionalData"));
    supplierInfo.appendChild(doc.createElement("SupplyLocation"))
                .appendChild(doc.createTextNode(C5Config::fMainJson["companyaddress"].toString()));
    supplierInfo.appendChild(doc.createElement("FactualSupplierAddress"))
                .appendChild(doc.createTextNode(C5Config::fMainJson["companyaddress"].toString()));
    //BehalfOf SUpplier
    QDomElement behalfSupplier = doc.createElement("OnBehalfOfSupplierInfo");
    invoice.appendChild(behalfSupplier);
    behalfSupplier.appendChild(doc.createElement("VATNumber"));
    QDomElement behalfTaxpayer = doc.createElement("Taxpayer");
    behalfSupplier.appendChild(behalfTaxpayer);
    behalfTaxpayer.appendChild(doc.createElement("TIN"));
    behalfTaxpayer.appendChild(doc.createElement("Name"));
    behalfTaxpayer.appendChild(doc.createElement("Address"));
    QDomElement behalfBank = doc.createElement("BankAccount");
    behalfTaxpayer.appendChild(behalfBank);
    behalfBank.appendChild(doc.createElement("BankName"));
    behalfBank.appendChild(doc.createElement("BankAccountNumber"));
    behalfTaxpayer.appendChild(doc.createElement("AdditionalData"));
    behalfTaxpayer.appendChild(doc.createElement("PrincipalTinNotRequired"))
                  .appendChild(doc.createTextNode("false"));
    // BuyerInfo
    QDomElement buyerInfo = doc.createElement("BuyerInfo");
    invoice.appendChild(buyerInfo);
    buyerInfo.appendChild(doc.createElement("VATNumber")); // Пустой
    QDomElement buyerTaxpayer = doc.createElement("Taxpayer");
    buyerTaxpayer.appendChild(doc.createElement("TIN"))
                 .appendChild(doc.createTextNode(ui->leTaxpayerId->text()));
    buyerTaxpayer.appendChild(doc.createElement("Passport"));
    buyerTaxpayer.appendChild(doc.createElement("Name"))
                 .appendChild(doc.createTextNode(fPartner.taxName));
    buyerTaxpayer.appendChild(doc.createElement("Address"))
                 .appendChild(doc.createTextNode(fPartner.address));
    QDomElement buyerBank = doc.createElement("BankAccount");
    buyerBank.appendChild(doc.createElement("BankName"));
    buyerBank.appendChild(doc.createElement("BankAccountNumber"));
    buyerTaxpayer.appendChild(buyerBank);
    buyerTaxpayer.appendChild(doc.createElement("AdditionalData"));
    buyerTaxpayer.appendChild(doc.createElement("TinNotRequired")).appendChild(doc.createTextNode("false"));
    buyerTaxpayer.appendChild(doc.createElement("IsNatural")).appendChild(doc.createTextNode("false"));
    buyerInfo.appendChild(buyerTaxpayer);
    buyerInfo.appendChild(doc.createElement("BuyerEmail"));
    QDomElement mediator = doc.createElement("Mediator");
    mediator.appendChild(doc.createElement("Name"));
    mediator.appendChild(doc.createElement("LicenseNumber"));
    buyerInfo.appendChild(mediator);
    buyerInfo.appendChild(doc.createElement("DeliveryMethod"));
    buyerInfo.appendChild(doc.createElement("DeliveryLocation"))
             .appendChild(doc.createTextNode(ui->leDeliveryAddress->text()));
    buyerInfo.appendChild(doc.createElement("BuyerResidenceAddress"));
    buyerInfo.appendChild(doc.createElement("FactualBuyerAddress"))
             .appendChild(doc.createTextNode(ui->leDeliveryAddress->text()));
    // GoodsInfo
    QDomElement goodsInfo = doc.createElement("GoodsInfo");
    invoice.appendChild(goodsInfo);
    double totalVat = 0;
    double totalWithoutVAT = 0;

    for(int i = 0; i < ui->tblGoods->rowCount(); ++i) {
        QDomElement good = doc.createElement("Good");
        double qty = ui->tblGoods->lineEdit(i, col_qty)->getDouble() ;
        double price = ui->tblGoods->lineEdit(i, col_price)->getDouble();
        price -= (price * ui->tblGoods->lineEdit(i, col_discount_value)->getDouble() / 100.0);
        double priceWithoutVAT = (price  / 1.2) ;
        double vat = (priceWithoutVAT  * 0.2);
        vat *= qty;
        totalVat += vat  ;
        totalWithoutVAT += priceWithoutVAT * qty ;
        // good.appendChild(doc.createElement("GoodId"))
        //     .appendChild(doc.createTextNode(QString::number(ui->tblGoods->getInteger(i, col_goods_code))));
        good.appendChild(doc.createElement("Description"))
            .appendChild(doc.createTextNode(ui->tblGoods->getString(i, col_name)));
        good.appendChild(doc.createElement("Unit"))
            .appendChild(doc.createTextNode(ui->tblGoods->getString(i, col_unit)));
        good.appendChild(doc.createElement("Amount")).appendChild(doc.createTextNode(QString::number(qty)));
        good.appendChild(doc.createElement("PricePerUnit"))
            .appendChild(doc.createTextNode(float_str_(priceWithoutVAT, 4)));
        good.appendChild(doc.createElement("Price"))
            .appendChild(doc.createTextNode(float_str_(priceWithoutVAT * qty, 2)));
        good.appendChild(doc.createElement("VATRate")).appendChild(doc.createTextNode("20"));
        good.appendChild(doc.createElement("VAT")).appendChild(doc.createTextNode(float_str_(vat, 2)));
        good.appendChild(doc.createElement("EnvironmentalFee")).appendChild(doc.createTextNode("0"));
        good.appendChild(doc.createElement("TotalPrice"))
            .appendChild(doc.createTextNode(float_str_(price * qty, 2)));
        goodsInfo.appendChild(good);

        if(i == ui->tblGoods->rowCount() - 1) {
            QDomElement total = doc.createElement("Total");
            total.appendChild(doc.createElement("Price"))
                 .appendChild(doc.createTextNode(float_str_(totalWithoutVAT, 2)));
            total.appendChild(doc.createElement("VAT"))
                 .appendChild(doc.createTextNode(float_str_(totalVat, 2)));
            total.appendChild(doc.createElement("TotalPrice"))
                 .appendChild(doc.createTextNode(float_str_(ui->leGrandTotal->getDouble(), 2)));
            total.appendChild(doc.createElement("TotalEnvironmentalFee")).appendChild(doc.createTextNode("0"));
            goodsInfo.appendChild(total);
        }
    }

    QFile f(fileName);

    if(f.open(QIODevice::WriteOnly)) {
        QTextStream stream(&f);
        stream.setEncoding(QStringConverter::Utf8);
        doc.save(stream, 2);
        f.close();
        C5Message::info(tr("Exported"));
    } else {
        C5Message::error(tr("Could not export to the file"));
    }
}

void C5SaleDoc::fiscale()
{
    //    auto *d = new C5PrintTaxAnywhere(ui->leUuid->text());
    //    d->exec();
    //    d->deleteLater();
    //    return;
    QElapsedTimer t;
    t.start();
    C5Database db;
    db[":f_header"] = ui->leUuid->text();
    db.exec("select gr.f_adgcode, og.f_goods, gn.f_name, og.f_price, og.f_qty, og.f_discountfactor*100 as f_discount "
            "from o_goods og "
            "left join c_goods gn on gn.f_id=og.f_goods "
            "left join c_groups gr on gr.f_id=gn.f_group "
            "where og.f_header=:f_header");
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(),
                 C5Config::taxUseExtPos().isEmpty() ? "false" : C5Config::taxUseExtPos(), C5Config::taxCashier(), C5Config::taxPin(),
                 this);

    while(db.nextRow()) {
        pt.addGoods(1, //dep
                    db.getString("f_adgcode"), //adg
                    QString::number(db.getInt("f_goods")), //goods id
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

    if(result != pt_err_ok) {
        C5Message::error(err);;
    } else {
        getFiscalNum();
    }
}

void C5SaleDoc::cancelFiscal()
{
    C5Database db;
    db[":f_order"] = ui->leUuid->text();
    db[":f_state"] = 1;
    db.exec("select * from o_tax_log where f_order=:f_order and f_state=:f_state");

    if(db.nextRow()) {
        QJsonObject jo = __strjson(db.getString("f_out"));
        ui->leFiscal->setInteger(jo["rseq"].toInt());
        QString crn = jo["crn"].toString();
        int rseq = jo["rseq"].toInt();
        PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(),
                     C5Config::taxCashier(), C5Config::taxPin(), this);
        QString jsnin, jsnout, err;
        int result;
        result = pt.printTaxback(rseq, crn, jsnin, jsnout, err);
        db[":f_id"] = db.uuid();
        db[":f_order"] = ui->leUuid->text();
        db[":f_date"] = QDate::currentDate();
        db[":f_time"] = QTime::currentTime();
        db[":f_in"] = jsnin;
        db[":f_out"] = jsnout;
        db[":f_err"] = err;
        db[":f_result"] = result;
        db.insert("o_tax_log", false);

        if(result != pt_err_ok) {
            C5Message::error(err);
        } else {
            db[":f_fiscal"] = QVariant();
            db[":f_receiptnumber"] = QVariant();
            db[":f_time"] = QVariant();
            db.update("o_tax", "f_id", ui->leUuid->text());
            db[":f_order"] = ui->leUuid->text();
            db.exec("update o_tax_log set f_state=0 where f_order=:f_order and f_state=1");
            C5Message::info(tr("Taxback complete"));
            getFiscalNum();
        }
    } else {
        C5Message::error(tr("Nothing to cancel"));
    }
}

void C5SaleDoc::createInvoiceAS()
{
    exportToAs(6);
}

void C5SaleDoc::createRetailAS()
{
    exportToAs(7);
}

void C5SaleDoc::makeStoreOutput()
{
    C5Database db;
    OutputOfHeader ooh;
    ooh.make(db, ui->leUuid->text());
}

void C5SaleDoc::exportToExcel()
{
    QXlsx::Document d;
    d.addSheet("Sheet1");
    /* HEADER */
    QFont headerFont(qApp->font());
    QXlsx::Format headerFormat;
    headerFormat.setFont(headerFont);
    headerFormat.setFontColor(Qt::black);
    headerFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    headerFormat.setBorderStyle(QXlsx::Format::BorderThin);
    d.setColumnWidth(1, 10);
    d.setColumnWidth(2, 15);
    d.setColumnWidth(3, 50);
    d.setColumnWidth(4, 20);
    d.setColumnWidth(5, 20);
    d.setColumnWidth(6, 20);
    d.setColumnWidth(7, 20);
    d.setColumnWidth(8, 20);
    d.setColumnWidth(9, 20);
    int col = 1, row = 1;
    d.write(row, col, QString("%1 N%2").arg(tr("Order"), ui->leDocnumber->text()),
            headerFormat);
    row++;

    if(!ui->leTaxpayerName->isEmpty()) {
        auto t = QString("%4 %1: %2, %3, %5").arg(tr("Taxpayer number"),
                 ui->leTaxpayerId->text(), ui->leTaxpayerName->text(),
                 tr("Buyer"), fPartner.phone);
        d.write(row, col, t, headerFormat);
        row++;
    }

    QList<int> cols;
    QStringList vals;
    col = 1;
    cols << col++;
    vals << tr("Date");

    for(int i = 0; i < cols.count(); i++) {
        d.write(row, cols.at(i), vals.at(i), headerFormat);
    }

    row++;
    vals.clear();
    vals << ui->leDate->text() + " " + ui->leTime->text();

    for(int i = 0; i < cols.count(); i++) {
        d.write(row, cols.at(i), vals.at(i), headerFormat);
    }

    row += 2;
    cols.clear();

    for(int i = 0; i < 9; i++) {
        cols << i + 1;
    }

    vals.clear();
    vals << tr("NN")
         << tr("Material code")
         << tr("Goods")
         << tr("Qty")
         << tr("Unit")
         << tr("Price")
         << tr("Discount")
         << tr("Discounted price")
         << tr("Total");

    for(int i = 0; i < cols.count(); i++) {
        d.write(row, cols.at(i), vals.at(i), headerFormat);
    }

    row++;
    QFont bodyFont(qApp->font());
    QXlsx::Format bodyFormat;
    bodyFormat.setFont(bodyFont);
    bodyFormat.setFontColor(Qt::black);
    bodyFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    bodyFormat.setBorderStyle(QXlsx::Format::BorderThin);

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        vals.clear();
        vals << QString::number(i + 1);
        vals << ui->tblGoods->getString(i, 4);
        vals << ui->tblGoods->getString(i, 5);
        vals << ui->tblGoods->lineEdit(i, 6)->text();
        vals << ui->tblGoods->getString(i, 7);
        vals << ui->tblGoods->lineEdit(i, 8)->text();
        vals << ui->tblGoods->lineEdit(i, col_discount_value)->text();
        vals << float_str(ui->tblGoods->lineEdit(i, col_price)->getDouble() - (ui->tblGoods->lineEdit(i,
                          col_price)->getDouble() * (ui->tblGoods->lineEdit(i, col_discount_value)->getDouble() / 100)), 1);
        vals << ui->tblGoods->lineEdit(i, col_grandtotal)->text();

        for(int j = 0; j < cols.count(); j++) {
            d.write(row, cols.at(j), vals.at(j), bodyFormat);
        }

        row++;
    }

    cols.clear();
    cols << 8 << 9;
    vals.clear();
    vals << tr("Total amount");
    vals << QString::number(str_float(ui->leGrandTotal->text()));

    for(int i = 0; i < cols.count(); i++) {
        d.write(row, cols.at(i), vals.at(i), headerFormat);
    }

    row++;
    d.mergeCells("A1:E1");
    d.mergeCells("A2:E2");
    d.mergeCells("A3:E3");
    d.mergeCells("A4:E4");
    QString filename = QFileDialog::getSaveFileName(nullptr, "", "", "*.xlsx");

    if(filename.isEmpty()) {
        return;
    }

    d.saveAs(filename);
    QDesktopServices::openUrl(filename);
}

void C5SaleDoc::returnItems()
{
    NLoadingDlg loadingDlg(this);
    loadingDlg.setWindowModality(Qt::ApplicationModal);
    loadingDlg.show();
    C5Database db;
    int hallid;
    QString prefix;
    db[":f_id"] = ui->cbHall->currentData();
    db.exec("select f_counter + 1, f_prefix as f_counter from h_halls where f_id=:f_id for update");

    if(db.nextRow()) {
        hallid = db.getInt(0);
        prefix = db.getString(1);
        db[":f_counter"] = db.getInt(0);
        db.update("h_halls", where_id(ui->cbHall->currentData().toInt()));
    } else {
        C5Message::error(tr("No hall with id") + "<br>" + ui->cbHall->currentText());
        return;
    }

    QString err;
    OHeader oh;
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
    oh.dateCash = QDate::currentDate();
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

    if(!oh.write(db, err)) {
        C5Message::error(err);
        return;
    }

    for(const QString &s : fRowToDelete) {
        db[":f_id"] = s;
        db.exec("delete from o_goods where f_id=:f_id");
    }

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        OGoods g;
        g.header = oh._id();
        g.goods = ui->tblGoods->getInteger(i, col_goods_code);
        g.store = ui->cbStorage->currentData().toInt();
        g.qty = ui->tblGoods->lineEdit(i, col_qty)->getDouble();
        g.price = ui->tblGoods->lineEdit(i, col_grandtotal)->getDouble() / ui->tblGoods->lineEdit(i, col_qty)->getDouble();
        g.total = ui->tblGoods->lineEdit(i, col_grandtotal)->getDouble();
        g.sign = -1;
        g.row = i + 1;
        g.storeRec = "";
        g.returnFrom = ui->tblGoods->getString(i, col_uuid);

        if(!g.write(db, err)) {
            C5Message::error(err);
            return;
        }
    }

    auto *a = __mainWindow->createTab<C5SaleDoc>();
    a->openDoc(oh._id());
}

void C5SaleDoc::saveDataChanges()
{
    QString err;

    if(ui->leDate->date().isValid() == false) {
        err += tr("Date is not valid") + "<br>";
    }

    if(!ui->cbHall->currentData().isValid()) {
        err += tr("Hall is not valid") + "<br>";
    }

    if(fPartner.id.toInt() == 0) {
        err += tr("Partner is required");
    }

    if(!ui->cbCashDesk->currentData().isValid()) {
        err += tr("Cash desk is not valid") + "<br>";
    }

    if(ui->cbStorage->currentData().toInt() < 1) {
        err += tr("Storage is not defined");
    }

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if(ui->tblGoods->lineEdit(i, col_qty)->getDouble() < 0.001) {
            err += tr("Quantity not defined on row: ") + QString::number(i + 1) + "<br>";
        }
    }

    if(ui->leGrandTotal->getDouble() < 0) {
        err += tr("Invalid total amount");
    }

    if(ui->leCash->getDouble()
            + ui->leCard->getDouble()
            + ui->lePrepaid->getDouble()
            + ui->leDebt->getDouble()
            + ui->leBankTransfer->getDouble()
            < ui->leGrandTotal->getDouble()) {
        err += tr("Incomplete payment") + "<br>";
    }

    if(ui->leCash->getDouble()
            + ui->leCard->getDouble()
            + ui->lePrepaid->getDouble()
            + ui->leDebt->getDouble()
            + ui->leBankTransfer->getDouble()
            > ui->leGrandTotal->getDouble()) {
        err += tr("Incomplete payment") + "<br>";
    }

    QDate deliveryDate = QDate::fromString(ui->leDelivery->text(), "dd/MM/yyyy");

    if(!deliveryDate.isValid()) {
        deliveryDate = QDate::currentDate();
        ui->leDelivery->setText(deliveryDate.toString(FORMAT_DATE_TO_STR));
    }

    if(err.isEmpty() == false) {
        C5Message::error(err);
        return;
    }

    if(fMode == 4) {
        saveReturnItems();
        return;
    }

    bool isNew = false;

    if(ui->leUuid->isEmpty()) {
        isNew = true;
        ui->leUuid->setText(C5Database::uuid());
    }

    QString uuid = ui->leUuid->text();
    QJsonObject jdoc;
    jdoc["session"] = C5Database::uuid();
    jdoc["giftcard"] = 0;
    jdoc["settings"] = __c5config.fSettingsName;
    jdoc["organization"] = ui->leTaxpayerName->text();
    jdoc["contact"] = ui->leTaxpayerId->text();
    QJsonObject jd;
    jd["f_delivery"] = deliveryDate.toString(FORMAT_DATE_TO_STR);
    jdoc["draft"] = jd;
    QJsonObject jh;
    jh["f_id"] = uuid;
    jh["f_hallid"] = ui->leDocnumber->property("f_hallid").toInt();
    jh["f_prefix"] = ui->leDocnumber->property("f_prefix").toString();
    jh["f_state"] = ORDER_STATE_CLOSE;
    jh["f_hall"] = ui->cbHall->currentData().toInt();
    jh["f_table"] = ui->cbCashDesk->currentData().toInt();
    jh["f_dateopen"] = QDate::currentDate().toString(FORMAT_DATE_TO_STR_MYSQL);
    jh["f_dateclose"] = QDate::currentDate().toString(FORMAT_DATE_TO_STR_MYSQL);
    jh["f_datecash"] = ui->leDate->date().toString(FORMAT_DATE_TO_STR_MYSQL);
    jh["f_timeopen"] = QTime::currentTime().toString(FORMAT_TIME_TO_STR);
    jh["f_timeclose"] = QTime::currentTime().toString(FORMAT_TIME_TO_STR);
    jh["f_cashier"] = ui->leCashier->property("f_cashier").toInt()  == 0 ? __user->id() :
                      ui->leCashier->property("f_cashier").toInt();
    jh["f_staff"] = __user->id();
    jh["f_comment"] = ui->leComment->text();
    jh["f_print"] = 0;
    jh["f_amounttotal"] = ui->leGrandTotal->getDouble();
    jh["f_amountcash"] = ui->leCash->getDouble();
    jh["f_amountcard"] = ui->leCard->getDouble();
    jh["f_amountprepaid"] = ui->lePrepaid->getDouble();
    jh["f_amountbank"] = ui->leBankTransfer->getDouble();
    jh["f_amountcredit"] = 0;
    jh["f_amountidram"] = 0;
    jh["f_amounttelcell"] = 0;
    jh["f_amountdebt"] = ui->leDebt->getDouble();
    jh["f_amountpayx"] = 0;
    jh["f_amountother"] = 0;
    jh["f_amountservice"] = 0;
    jh["f_amountdiscount"] = ui->leDiscount->getDouble();
    jh["f_servicefactor"] = 0;
    jh["f_discountfactor"] = 0;
    jh["f_source"] = 2;
    jh["f_saletype"] = fMode;
    jh["f_partner"] = fPartner.id.toInt();
    jh["f_currency"] = ui->cbCurrency->currentData().toInt();
    jh["f_taxpayertin"] = fPartner.taxCode;
    jh["f_cash"] = ui->leCountCash->getDouble();
    jh["f_change"] = ui->leChange->getDouble();
    jdoc["header"] = jh;
    QJsonArray jg;

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        QJsonObject jt;
        jt["f_id"] = C5Database::uuid();
        jt["f_header"] = jh["f_id"];
        jt["f_store"] = ui->cbStorage->currentData().toInt();
        jt["f_goods"] = ui->tblGoods->getInteger(i, col_goods_code);
        jt["f_qty"] = ui->tblGoods->lineEdit(i, col_qty)->getDouble();
        jt["f_price"] = ui->tblGoods->lineEdit(i, col_price)->getDouble();
        jt["f_total"] = ui->tblGoods->lineEdit(i, col_grandtotal)->getDouble();
        jt["f_tax"] = 0;
        jt["f_sign"] = 1;
        jt["f_taxdebt"] = 0;
        jt["f_adgcode"] = "";
        jt["f_row"] = i;
        jt["f_storerec"] = "";
        jt["f_discountfactor"] = ui->tblGoods->lineEdit(i, col_discount_value)->getDouble() / 100;
        jt["f_discountmode"] = 1;
        jt["f_discountamount"] = ui->tblGoods->lineEdit(i, col_discount_amount)->getDouble();
        jt["f_return"] = 0;
        jt["f_returnfrom"] =  QJsonValue();
        jt["f_isservice"] = ui->tblGoods->getInteger(i, col_isservice);
        jt["f_amountaccumulate"] = 0;
        jt["f_emarks"] = ui->tblGoods->getString(i, col_emarks).replace("\"", "\\\"");
        jg.append(jt);
    }

    jdoc["goods"] = jg;
    QJsonObject jhistory;
    jhistory["f_card"] = 0;
    jhistory["f_data"] = 0;
    jhistory["f_type"] = 0;
    jdoc["history"] = jhistory;
    QJsonObject jflags;
    jflags["f_1"] =  0;
    jflags["f_2"] =  0;
    jflags["f_3"] =  0;
    jflags["f_4"] =  0;
    jflags["f_5"] =  0;
    jdoc["flags"] = jflags;
    QString sql = QString(QJsonDocument(jdoc).toJson(QJsonDocument::Compact));
    sql.replace("'", "\\'");
    sql = QString("call sf_create_shop_order('%1')").arg(sql);
    C5Database db;

    if(!db.exec(sql)) {
        C5Message::error(db.fLastError);
        return ;
    }

    db[":f_session"] = jdoc["session"].toString();

    if(!db.exec("select f_result from a_result where f_session=:f_session")) {
        C5Message::error(db.fLastError);
        return ;
    }

    if(!db.nextRow()) {
        C5Message::error("Program error. Cannot get result of session");
        return ;
    }

    QJsonObject jr = QJsonDocument::fromJson(db.getString("f_result").toUtf8()).object();

    if(jr["status"].toInt() != 1) {
        C5Message::error(jr["message"].toString());
        return ;
    }

    ui->leDocnumber->setText(QString("%1%2").arg(jr["f_prefix"].toString()).arg(jr["f_hallid"].toInt()));
    ui->leDocnumber->setProperty("f_hallid", jr["f_hallid"].toInt());
    ui->leDocnumber->setProperty("f_prefix", jr["f_prefix"].toString());

    if(isNew && !__c5config.httpServerIP().isEmpty()) {
        for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
            QJsonObject jo;
            jo["action"] = MSG_GOODS_RESERVE;
            jo["goods"] = ui->tblGoods->getInteger(i, col_goods_code);
            jo["qty"] = ui->tblGoods->getDouble(i, col_qty);
            jo["goodsname"] = ui->tblGoods->getString(i, col_name);
            jo["scancode"] = ui->tblGoods->getString(i, col_barcode);
            jo["unit"] = ui->tblGoods->getString(i, col_unit);
            jo["usermessage"] = tr("Online shop") + " " + ui->leDocnumber->text();
            jo["enddate"] = ui->leDate->date().addDays(7).toString(FORMAT_DATE_TO_STR);
            jo["userfrom"] = __c5config.defaultStore();
            jo["userto"] = ui->cbStorage->currentData().toInt();
            fHttp->createHttpQuery("/engine/shop/create-reserve.php", jo, SLOT(createReserveResponse(QJsonObject)), QVariant(),
                                   false);
        }
    }

    for(int r = 0; r < ui->tblGoods->rowCount(); r++) {
        for(int c = 0; c < ui->tblGoods->columnCount(); c++) {
            QWidget *w = ui->tblGoods->cellWidget(r, c);

            if(w) {
                w->setEnabled(false);
            }
        }
    }

    ui->wtoolbar->setEnabled(false);
    ui->paymentFrame->setEnabled(false);
    fActionSave->setEnabled(false);
    //fActionDraft->setEnabled(true);
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
    C5Database db;
    db[":f_scancode"] = code;
    db.exec("select f_id from c_goods where f_scancode=:f_scancode");

    if(db.nextRow()) {
        addGoods(db.getInt(0), db);
        return;
    }
}

int C5SaleDoc::addGoods(int goodsId, C5Database &db)
{
    if(ui->cbCurrency->currentIndex() < 0) {
        C5Message::error(tr("Select currencty"));
        return -1;
    }

    QString priceField, priceDiscount;
    priceField = "f_price1";
    priceDiscount = "f_price1disc";
    db[":f_id"] = goodsId;
    db[":f_currency"] = ui->cbCurrency->currentData();
    db[":f_partner"] = fPartner.id;
    db.exec(QString(R"(
                    select g.*, gu.f_name as f_unitname,
                    if((coalesce(sp.f_price, 0)>0), sp.f_price, if (gpr.%1>0, gpr.%1, gpr.%2)) as f_price
                    from c_goods g
                    left join c_goods_prices gpr on gpr.f_goods=g.f_id
                    left join c_goods_special_prices sp on sp.f_goods=g.f_id and sp.f_partner=:f_partner
                    left join c_units gu on gu.f_id=g.f_unit
                    where g.f_id=:f_id and gpr.f_currency=:f_currency
    )").arg(priceDiscount, priceField));

    if(db.nextRow()) {
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
    l->setDouble(qty * price);
    l->setReadOnly(true);
    connect(ui->tblGoods->lineEdit(r, col_qty), &C5LineEdit::textEdited, this, &C5SaleDoc::on_QtyTextChanged);
    connect(ui->tblGoods->lineEdit(r, col_price), &C5LineEdit::textEdited, this, &C5SaleDoc::on_PriceTextChanged);
    connect(ui->tblGoods->lineEdit(r, col_discount_value), &C5LineEdit::textEdited, this,
            &C5SaleDoc::on_discountValueChanged);

    if(fSpecialPrices.contains(goodsId)) {
        ui->tblGoods->lineEdit(r, col_price)->setDouble(fSpecialPrices[goodsId]);
        ui->tblGoods->lineEdit(r, col_discount_value)->setDouble(0);
    }

    ui->tblGoods->setInteger(r, col_isservice, isService);
    ui->tblGoods->setString(r, col_returnfrom, returnFrom);
    countGrandTotal();
    return r;
}

void C5SaleDoc::countGrandTotal()
{
    double grandTotal = 0;

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        double total = ui->tblGoods->lineEdit(i, col_qty)->getDouble() * ui->tblGoods->lineEdit(i, col_price)->getDouble();
        double disc = total * (ui->tblGoods->lineEdit(i, col_discount_value)->getDouble() / 100);
        ui->tblGoods->lineEdit(i, col_discount_amount)->setDouble(disc);
        total -= disc;
        ui->tblGoods->lineEdit(i, col_grandtotal)->setDouble(total);
        grandTotal += total;
    }

    ui->leGrandTotal->setDouble(grandTotal);
    countTotalQty();
}

void C5SaleDoc::countTotalQty()
{
    double totalqty = 0;

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        totalqty += ui->tblGoods->lineEdit(i, col_qty)->getDouble();
    }

    ui->leTotalQty->setDouble(totalqty);
}

bool C5SaleDoc::openDraft(const QString &id)
{
    NLoadingDlg loadingDlg(this);
    loadingDlg.setWindowModality(Qt::ApplicationModal);
    loadingDlg.show();
    C5Database db;
    db[":f_id"] = id;
    db.exec("select * from o_draft_sale where f_id=:f_id");

    if(!fDraftSale.getRecord(db)) {
        C5Message::error(tr("Invalid draft document id"));
        return false;
    }

    if(fDraftSale.state != 1) {
        return openDoc(id);
    }

    ui->leCashier->setProperty("f_cashier", db.getInt("f_cashier"));

    switch(db.getInt("f_saletype")) {
    case 1:
        ui->leSaleType->setText(tr("Retail"));
        break;

    case 2:
        ui->leSaleType->setText(tr("Whosale"));
        break;
    }

    if(db.getInt("f_state") > 1) {
        fToolBar->actions().at(0)->setEnabled(false);
    }

    fPartner.queryRecordOfId(db, fDraftSale.partner);
    setPartner();
    ui->leDate->setDate(fDraftSale.date);

    if(C5Config::fMainJson["change_draft_date_to_current"].toBool()) {
        ui->leDate->setDate(QDate::currentDate());
    }

    ui->leTime->setText(fDraftSale.time.toString(FORMAT_TIME_TO_STR));
    ui->leComment->setText(fDraftSale.comment);
    ui->leUuid->setText(fDraftSale.id.toString());
    ui->leDelivery->setText(fDraftSale.deliveryDate.toString(FORMAT_DATE_TO_STR));
    setDeliveryMan();
    db[":f_id"] = ui->leCashier->property("f_cashier");
    db.exec("select * from s_user where f_id=:f_id");
    db.nextRow();
    ui->leCashier->setText(QString("%1 %2").arg(db.getString("f_last"), db.getString("f_first")));
    QString priceField = "f_price1";
    db[":f_header"] = id;
    db[":f_state"] = 1;
    db[":f_currency"] = ui->cbCurrency->currentData();
    db.exec(QString("select dsb.f_store, dsb.f_qty, g.*, "
                    "gu.f_name as f_unitname, dsb.f_price, dsb.f_discount "
                    "from o_draft_sale_body dsb "
                    "left join c_goods g on g.f_id=dsb.f_goods "
                    "left join c_goods_prices gpr on gpr.f_goods=g.f_id "
                    "left join c_units gu on gu.f_id=g.f_unit "
                    "where dsb.f_header=:f_header and gpr.f_currency=:f_currency "
                    "and dsb.f_state=:f_state and dsb.f_qty>0 ").arg(priceField));
    ui->tblGoods->setRowCount(0);

    while(db.nextRow()) {
        ui->cbStorage->setCurrentIndex(ui->cbStorage->findData(db.getInt("f_store")));
        addGoods("", db.getInt("f_store"), db.getInt("f_id"), db.getString("f_scancode"), db.getString("f_name"),
                 db.getString("f_unitname"), db.getDouble("f_qty"), db.getDouble("f_price"), db.getDouble("f_discount"), 0, "");
    }

    fOpenedFromDraft = true;

    switch(fDraftSale.payment) {
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

    db[":f_header"] = id;
    db.exec("select * from o_qr where f_header = :f_header");

    while(db.nextRow()) {
        fEmarks.append(QByteArray::fromBase64(db.getString("f_qr").toUtf8()));
    }

    fActionSave->setEnabled(true);
    fActionCopy->setEnabled(false);

    //fActionDraft->setEnabled(false);
    for(int r = 0; r < ui->tblGoods->rowCount(); r++) {
        for(int c = 0; c < ui->tblGoods->columnCount(); c++) {
            QWidget *w = ui->tblGoods->cellWidget(r, c);

            if(w) {
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

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        ui->tblGoods->lineEdit(i, col_discount_value)->setDouble(fPartner.permanentDiscount);
    }

    fSpecialPrices.clear();
    C5Database db;
    db[":f_partner"] = fPartner.id;
    db.exec("select f_goods, f_price from c_goods_special_prices where f_partner=:f_partner and f_goods not in (select f_id from c_goods where f_nospecial_price=1)");

    while(db.nextRow()) {
        fSpecialPrices[db.getInt("f_goods")] = db.getDouble("f_price");
    }

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if(fSpecialPrices.contains(ui->tblGoods->getInteger(i, col_goods_code))) {
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
    ui->leDeliveryAddress->setText(p.address);
    //setMode(fPartner.pricePolitic);
}

void C5SaleDoc::setDeliveryMan()
{
    ui->leDeluveryMan->clear();
    C5Database db;

    if(fDraftSale.staff > 0) {
        db[":f_id"] = fDraftSale.staff;
        db.exec("select concat_ws(' ', f_last, f_first) as f_fullname from s_user where f_id=:f_id");
        db.nextRow();
        ui->leDeluveryMan->setText(db.getString("f_fullname"));
    }

    if(!fActionSave->isEnabled()) {
        db["f_id"] = ui->leUuid->text();
        db["f_staff"] = fDraftSale.staff;
        db.exec("update o_header set f_staff=:f_staff where f_id=:f_id");
    }
}

void C5SaleDoc::exportToAs(int doctype)
{
    C5Database db;
    db.exec("select * from as_list");

    if(db.rowCount() == 0) {
        C5Message::error(tr("ArmSoft is not configure"));
        return;
    }

    int dbid = -1;
    int index = 0;
    QString dealtype;
    QString connStr;

    if(db.rowCount() > 0) {
        QStringList dbNames;

        while(db.nextRow()) {
            dbNames.append(db.getString("f_name"));
        }

        index = DlgList2::indexOfList(tr("Armsoft database"), dbNames);

        if(index < 0) {
            return;
        }

        dbid = db.getInt(index, "f_id");
        connStr = db.getString(index, "f_connectionstring");
        dealtype = db.getString(index, "f_dealtype");
    }

    BreezeConfig *b = Configs::construct<BreezeConfig>(1);
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
    jo["dealtype"] = dealtype;
    jo["lesexpenseacc"] = __c5config.getRegValue("lesexpenseacc", "").toString();
    jo["lesincomeacc"] = __c5config.getRegValue("lesincomeacc", "").toString();
    jo["lemexpenseacc"] = __c5config.getRegValue("lemexpenseacc", "").toString();
    jo["lemincomeacc"] = __c5config.getRegValue("lemincomeacc", "").toString();
    QJsonObject jdb;
    jdb["host"] = __c5config.dbParams().at(0);
    jdb["schema"] = __c5config.dbParams().at(1);
    jdb["username"] = __c5config.dbParams().at(2);
    jdb["password"] = __c5config.dbParams().at(3);
    jo["database"] = jdb;
    jo["vatpercent"] = index == 0 ? (doctype == 5 ? 0.2 : 0.1667) : 0;
    jo["vattype"] = index == 0 ? (doctype == 5 ? "1" : "5") : "3";
    jo["pricewithoutvat"] = index == 0 ? (doctype == 5 ? 1.2 : 1) : 1;
    jo["withvat"] = index == 0 ? (doctype == 5 ? 0.2 : 0) : 0;
    QMap<QString, QVariant> m;
    QJsonObject jh;
    db[":f_id"] = ui->leUuid->text();
    db.exec("select ds.f_saletype, oh.* from o_header oh left join o_draft_sale ds on oh.f_id=ds.f_id where oh.f_id=:f_id ");
    db.nextRow();
    db.rowToMap(m);
    jh = QJsonObject::fromVariantMap(m);
    m.clear();
    jo["header"] = jh;
    //AS PARTNER CODE
    db[":f_asdbid"] = jo["asdbid"].toInt();
    db[":f_table"] = "c_partners";
    db[":f_tableid"] = jh["f_partner"].toInt();
    db.exec("select * from as_convert where f_asdbid=:f_asdbid and f_table=:f_table and f_tableid=:f_tableid");

    if(db.nextRow()) {
        jh["f_aspartnerid"] = db.getString("f_ascode");
    } else {
        C5Message::error("No partner with specified id");
        return;
    }

    //PRICE POLITIC
    db[":f_id"] = jh["f_partner"].toInt();
    db.exec("select * from c_partners where f_id=:f_id");

    if(db.nextRow()) {
        jh["f_price_politic"] = db.getInt("f_price_politic");
        jh["f_address"] = db.getString("f_address");
        jh["f_legaladdress"] = db.getString("f_legaladdress");
        jh["f_taxcode"] = db.getString("f_taxcode");
    } else {
        C5Message::error("Cannot find partner price politic");
        return;
    }

    jo["header"] = jh;
    //GET GOODS
    QJsonArray ja;
    db[":f_header"] = ui->leUuid->text();
    db[":f_asdbid"] = jo["asdbid"].toInt();
    db[":f_dealtype"] = dealtype;
    db.exec("SELECT ds.f_goods, a.f_ascode, g.f_name, ds.f_price - (ds.f_price * ds.f_discountfactor) as f_price, ds.f_qty, "
            "ds.f_price as f_initprice, ds.f_discountfactor as f_discount, "
            "g.f_service, a2.f_ascode as f_asstore, :f_dealtype as f_dealtype "
            "FROM o_goods ds "
            "LEFT JOIN c_goods g ON g.f_id=ds.f_goods "
            "left join as_convert a on a.f_tableid=g.f_id and a.f_table='c_goods' and a.f_asdbid=:f_asdbid "
            "left join as_convert a2 on a2.f_tableid=ds.f_store and a2.f_table='c_storages' and a2.f_asdbid=:f_asdbid "
            "where ds.f_header=:f_header  ");

    while(db.nextRow()) {
        if(db.getString("f_ascode").isEmpty()) {
            C5Message::error(QString("%1 %2. \r\n").arg(tr("No goods code exists for"), db.getString("f_name")));
            return;
        }

        m.clear();
        QJsonObject jtemp;
        db.rowToMap(m);
        jtemp = QJsonObject::fromVariantMap(m);
        ja.append(jtemp);
    }

    jo["body"] = ja;
    jo["params"] = jo;
    jo["command"] = "armsoft";
    jo["handler"] = "armsoft";
    jo["key"] = "asdf7fa8kk49888d!!jjdjmskkak98983mj???m";
    HttpQueryDialog *qd = new HttpQueryDialog(QString("ws://%1:%2").arg(b->ipAddress,
        QString::number(b->port)), jo, this);
    qd->exec();
    qd->deleteLater();
}

void C5SaleDoc::getFiscalNum()
{
    C5Database db;
    db[":f_order"] = ui->leUuid->text();
    db[":f_state"] = 1;
    db.exec("select * from o_tax_log where f_order=:f_order and f_state=:f_state");

    if(db.nextRow()) {
        QJsonObject jo = __strjson(db.getString("f_out"));
        ui->leFiscal->setInteger(jo["rseq"].toInt());
    }
}

void C5SaleDoc::on_btnAddGoods_clicked()
{
    QJsonArray vals;

    if(!C5Selector::getValueOfColumn(cache_goods, vals, 3)) {
        return;
    }

    if(vals.at(1).toInt() == 0) {
        C5Message::error(tr("Could not add goods without code"));
        return;
    }

    C5Database db;
    addGoods(vals.at(1).toInt(), db);
}

void C5SaleDoc::on_btnRemoveGoods_clicked()
{
    for(int i = ui->tblGoods->rowCount() - 1; i > -1; i--) {
        if(ui->tblGoods->checkBox(i, col_checkbox)->isChecked()) {
            if(ui->tblGoods->getString(i, 0).isEmpty() == false) {
                fRowToDelete.append(ui->tblGoods->getString(i, 0));
            }

            ui->tblGoods->removeRow(i);
        }
    }

    countGrandTotal();
}

void C5SaleDoc::on_btnEditGoods_clicked()
{
    int row = ui->tblGoods->currentRow();

    if(row < 0) {
        return;
    }

    if(ui->tblGoods->getInteger(row, col_goods_code) == 0) {
        return;
    }

    CE5Goods *ep = new CE5Goods();
    C5Editor *e = C5Editor::createEditor(ep, 0);
    ep->setId(ui->tblGoods->getInteger(row, col_goods_code));
    QList<QMap<QString, QVariant> > data;

    if(e->getResult(data)) {
        if(data.at(0)["f_id"].toInt() == 0) {
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
    CE5Goods *ep = new CE5Goods();
    C5Editor *e = C5Editor::createEditor(ep, 0);
    QList<QMap<QString, QVariant> > data;

    if(e->getResult(data)) {
        if(data.at(0)["f_id"].toInt() == 0) {
            C5Message::error(tr("Cannot add goods without code"));
            return;
        }

        C5Database db;
        addGoods(data.at(0)["f_id"].toInt(), db);
    }

    delete e;
    countGrandTotal();
}

void C5SaleDoc::on_cbHall_currentIndexChanged(int index)
{
    ui->cbCashDesk->clear();
    ui->cbCashDesk->setDBValues(QString("select f_id, f_name from h_tables where f_hall=%1").arg(ui->cbHall->itemData(index).toInt()));

    if(ui->cbHall->itemData(index).toInt() == __c5config.getValue(param_default_hall).toInt()) {
        ui->cbCashDesk->setCurrentIndex(ui->cbCashDesk->findData(__c5config.getValue(param_default_table).toInt()));
    }
}

void C5SaleDoc::on_btnSearchTaxpayer_clicked()
{
    QJsonArray values;

    if(!C5Selector::getValue(cache_goods_partners, values)) {
        return;
    }

    if(values.count() == 0) {
        return;
    }

    C5Database db;
    fPartner.queryRecordOfId(db, values.at(1).toInt());
    setPartner();
}

void C5SaleDoc::on_btnRemoveDelivery_clicked()
{
    ui->leDelivery->clear();
}

void C5SaleDoc::on_btnDelivery_clicked()
{
    QDate d;

    if(C5DateRange::date(d)) {
        ui->leDelivery->setText(d.toString(FORMAT_DATE_TO_STR));
        fDraftSale.deliveryDate = d;
    }
}

void C5SaleDoc::on_btnEditPartner_clicked()
{
    if(fPartner.id.toInt() > 0) {
        CE5Partner *ep = new CE5Partner();
        C5Editor *e = C5Editor::createEditor(ep, 0);
        ep->setId(fPartner.id.toInt());
        QList<QMap<QString, QVariant> > data;

        if(e->getResult(data)) {
            if(data.at(0)["f_id"].toInt() == 0) {
                C5Message::error(tr("Cannot change partner without code"));
                return;
            }

            C5Database db;
            fPartner.queryRecordOfId(db, fPartner.id.toInt());
            setPartner();
        }

        delete e;
    }
}

void C5SaleDoc::on_btnEditAccounts_clicked()
{
    ArmSoftExportOptions d;
    d.exec();
}

void C5SaleDoc::on_leCash_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);

    if(ui->leCash->getDouble() > ui->leGrandTotal->getDouble()) {
        ui->leCash->setDouble(ui->leGrandTotal->getDouble());
    }
}

void C5SaleDoc::on_btnDeliveryMan_clicked()
{
    QJsonArray vals;
    C5Selector::getValue(cache_users, vals);

    if(vals.count() == 0) {
        return;
    }

    fDraftSale.staff = vals.at(1).toInt();
    setDeliveryMan();
}

void C5SaleDoc::saveReturnItems()
{
    C5Database db;
    C5StoreDraftWriter dw(db);
    OHeader oheader;
    oheader.id = ui->leUuid->text();
    oheader.staff = __user->id();
    oheader.state = ORDER_STATE_CLOSE;
    oheader.amountTotal = ui->leGrandTotal->getDouble() * -1;
    oheader.amountCash = ui->leCash->getDouble() * -1;
    oheader.amountBank = ui->leBankTransfer->getDouble() * -1;
    oheader.partner = fPartner.id.toInt();
    oheader.dateCash = ui->leDate->date();
    oheader.saleType = SALE_RETURN;
    oheader.hall = ui->cbHall->currentData().toInt();

    if(!dw.hallId(oheader.prefix, oheader.hallId, ui->cbHall->currentData().toInt())) {
        C5Message::error(dw.fErrorMsg);
        return;
    }

    QString err;

    if(!oheader.write(db, err)) {
        C5Message::error(err);
        return;
    }

    bool empty = true;

    for(const QString &s : fRowToDelete) {
        db[":f_id"] = s;
        db.exec("delete from o_goods where f_id=:f_id");
    }

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        empty = false;
        OGoods g;
        g.header = oheader._id();
        g.id = ui->tblGoods->getString(i, col_uuid);
        g.store = ui->cbStorage->currentData().toInt();
        g.goods = ui->tblGoods->getInteger(i, col_goods_code);
        g.qty = ui->tblGoods->lineEdit(i, col_qty)->getDouble();
        g.price = ui->tblGoods->lineEdit(i, col_price)->getDouble();
        g.total = ui->tblGoods->lineEdit(i, col_grandtotal)->getDouble();
        g.sign = -1;
        g.row = i + 1;
        g.storeRec = "";
        g.returnFrom = ui->tblGoods->getString(i, col_returnfrom);

        if(!g.write(db, err)) {
            C5Message::error(err);
            return;
        }
    }

    QJsonObject js;
    js["id"] = oheader._id();
    js["user"] = __user->id();
    js["storein"] = ui->cbStorage->currentData().toInt();
    js["currency"] = ui->cbCurrency->currentData().toInt();
    js["date"] = ui->leDate->date().toString(FORMAT_DATE_TO_STR_MYSQL);
    js["partner"] = fPartner.id.toInt();
    db.exec(QString("call sf_create_return_of_sale('%1')").arg(json_str(js)));
    fActionSave->setEnabled(false);
    //fActionDraft->setEnabled(false);
    C5Message::info(tr("Saved"));
}

void C5SaleDoc::saveAsDraft()
{
    if(C5Message::question(tr("Confirm to make a draft")) != QDialog::Accepted) {
        return;
    }

    QDate deliveryDate = QDate::fromString(ui->leDelivery->text(), "dd/MM/yyyy");

    if(!deliveryDate.isValid()) {
        deliveryDate = QDate::currentDate();
    }

    QString uuid = ui->leUuid->text();
    QJsonObject jdoc;
    jdoc["class"] = "saledoc";
    jdoc["method"] = "createDraft";
    jdoc["session"] = C5Database::uuid();
    jdoc["giftcard"] = 0;
    jdoc["settings"] = __c5config.fSettingsName;
    jdoc["organization"] = ui->leTaxpayerName->text();
    jdoc["contact"] = ui->leTaxpayerId->text();
    QJsonObject jh;
    jh["f_id"] = uuid;
    jh["f_hallid"] = 0;
    jh["f_prefix"] = "";
    jh["f_state"] = ORDER_STATE_CLOSE;
    jh["f_hall"] = ui->cbHall->currentData().toInt();
    jh["f_table"] = ui->cbCashDesk->currentData().toInt();
    jh["f_dateopen"] = QDate::currentDate().toString(FORMAT_DATE_TO_STR_MYSQL);
    jh["f_dateclose"] = QDate::currentDate().toString(FORMAT_DATE_TO_STR_MYSQL);
    jh["f_datecash"] = QDate::currentDate().toString(FORMAT_DATE_TO_STR_MYSQL);
    jh["f_timeopen"] = QTime::currentTime().toString(FORMAT_TIME_TO_STR);
    jh["f_timeclose"] = QTime::currentTime().toString(FORMAT_TIME_TO_STR);
    jh["f_cashier"] = __user->id();
    jh["f_staff"] = fDraftSale.staff;
    jh["f_comment"] = ui->leComment->text();
    jh["f_print"] = 0;
    jh["f_amounttotal"] = ui->leGrandTotal->getDouble();
    jh["f_amountcash"] = ui->leCash->getDouble();
    jh["f_amountcard"] = ui->leCard->getDouble();
    jh["f_amountprepaid"] = ui->lePrepaid->getDouble();
    jh["f_amountbank"] = ui->leBankTransfer->getDouble();
    jh["f_amountcredit"] = 0;
    jh["f_amountidram"] = 0;
    jh["f_amounttelcell"] = 0;
    jh["f_amountdebt"] = ui->leDebt->getDouble();
    jh["f_amountpayx"] = 0;
    jh["f_amountother"] = 0;
    jh["f_amountservice"] = 0;
    jh["f_amountdiscount"] = ui->leDiscount->getDouble();
    jh["f_servicefactor"] = 0;
    jh["f_discountfactor"] = 0;
    jh["f_source"] = 2;
    jh["f_saletype"] = fMode;
    jh["f_partner"] = fPartner.id.toInt();
    jh["f_currency"] = ui->cbCurrency->currentData().toInt();
    jh["f_taxpayertin"] = fPartner.taxCode;
    jh["f_cash"] = ui->leCountCash->getDouble();
    jh["f_change"] = ui->leChange->getDouble();
    jh["f_deliverydate"] = deliveryDate.toString(FORMAT_DATE_TO_STR_MYSQL);
    jdoc["header"] = jh;
    QJsonArray jg;

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        QJsonObject jt;
        jt["f_id"] = C5Database::uuid();
        jt["f_header"] = jh["f_id"];
        jt["f_store"] = ui->cbStorage->currentData().toInt();
        jt["f_goods"] = ui->tblGoods->getInteger(i, col_goods_code);
        jt["f_qty"] = ui->tblGoods->lineEdit(i, col_qty)->getDouble();
        jt["f_price"] = ui->tblGoods->lineEdit(i, col_price)->getDouble();
        jt["f_total"] = ui->tblGoods->lineEdit(i, col_grandtotal)->getDouble();
        jt["f_tax"] = 0;
        jt["f_sign"] = 1;
        jt["f_taxdebt"] = 0;
        jt["f_adgcode"] = "";
        jt["f_row"] = i;
        jt["f_storerec"] = "";
        jt["f_discountfactor"] = ui->tblGoods->lineEdit(i, col_discount_value)->getDouble();
        jt["f_discountmode"] = 1;
        jt["f_discountamount"] = ui->tblGoods->lineEdit(i, col_discount_amount)->getDouble();
        jt["f_return"] = 0;
        jt["f_returnfrom"] =  QJsonValue();
        jt["f_isservice"] = ui->tblGoods->getInteger(i, col_isservice);
        jt["f_amountaccumulate"] = 0;
        jt["f_emarks"] = ui->tblGoods->getString(i, col_emarks).replace("\"", "\\\"");
        jt["f_row"] = i;
        jg.append(jt);
    }

    jdoc["goods"] = jg;
    QJsonObject jhistory;
    jhistory["f_card"] = 0;
    jhistory["f_data"] = 0;
    jhistory["f_type"] = 0;
    jdoc["history"] = jhistory;
    QJsonObject jflags;
    jflags["f_1"] =  0;
    jflags["f_2"] =  0;
    jflags["f_3"] =  0;
    jflags["f_4"] =  0;
    jflags["f_5"] =  0;
    jdoc["flags"] = jflags;
    fHttp->createHttpQuery("/engine/office/", jdoc, SLOT(makeDraftResponse(QJsonObject)));
}

void C5SaleDoc::saveCopy()
{
    NLoadingDlg loadingDlg(this);
    loadingDlg.setWindowModality(Qt::ApplicationModal);
    loadingDlg.show();
    QString err;
    C5Database db;
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
    fDraftSale.deliveryDate = QDate::currentDate();

    if(!fDraftSale.write(db, err)) {
        C5Message::error(err);
        return;
    }

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        ui->tblGoods->setString(i, col_uuid, "");
        fDraftSaleBody.id = "";
        fDraftSaleBody.header = fDraftSale.id.toString();
        fDraftSaleBody.state = 1;
        fDraftSaleBody.store = ui->cbStorage->currentData().toInt();
        fDraftSaleBody.dateAppend = QDate::currentDate();
        fDraftSaleBody.timeAppend = QTime::currentTime();
        fDraftSaleBody.goods = ui->tblGoods->getInteger(i, col_goods_code);
        fDraftSaleBody.qty = ui->tblGoods->lineEdit(i, col_qty)->getDouble();
        fDraftSaleBody.price = ui->tblGoods->lineEdit(i, col_price)->getDouble();
        fDraftSaleBody.discount = ui->tblGoods->lineEdit(i, col_discount_value)->getDouble();
        fDraftSaleBody.userAppend = __user->id();

        if(!fDraftSaleBody.write(db, err)) {
            C5Message::error(err);
            return;
        }
    }

    loadingDlg.close();
    auto *doc = __mainWindow->createTab<C5SaleDoc>();
    doc->openDraft(fDraftSale.id.toString());
}

void C5SaleDoc::removeDoc()
{
    if(C5Message::question(tr("Confirm to remove document")) != QDialog::Accepted) {
        return;
    }

    fHttp->createHttpQuery("/engine/shop/remove-order.php", QJsonObject{{"id", ui->leUuid->text()}}, SLOT(removeDocResponse(
                QJsonObject)));
}

void C5SaleDoc::on_btnQr_clicked()
{
    fHttp->createHttpQueryLambda("/engine/v2/officen/sale-doc/getemarks",
    QJsonObject{{"f_header", ui->leUuid->text()}}, [](const QJsonObject & jdoc) {
    }, [](const QJsonObject & jerr) {});
}

void C5SaleDoc::on_btnCalculator_clicked()
{
}

void C5SaleDoc::on_cbStorage_currentIndexChanged(int index)
{
}

void C5SaleDoc::on_btnCashier_clicked()
{
    QJsonArray vals;
    C5Selector::getValue(cache_users, vals);

    if(vals.count() == 0) {
        return;
    }

    fDraftSale.cashier = vals.at(1).toInt();
    ui->leCashier->setProperty("f_cashier", vals.at(1).toInt());
    ui->leCashier->setText(QString("%1 %2").arg(vals.at(2).toString(), vals.at(3).toString()));

    if(!fActionSave->isEnabled()) {
        C5Database db;
        db[":f_id"] = ui->leUuid->text();
        db[":f_cashier"] = vals.at(1).toInt();
        db.exec("update o_header set f_cashier=:f_cashier where f_id=:f_id");
    }
}

void C5SaleDoc::on_btnCopyUUID_clicked()
{
    qApp->clipboard()->setText(ui->leUuid->text());
}
