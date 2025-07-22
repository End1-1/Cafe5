#include "c5salefromstoreorder.h"
#include "ui_c5salefromstoreorder.h"
#include "c5mainwindow.h"
#include "../../NewTax/Src/printtaxn.h"
#include "breezeconfig.h"
#include "../Forms/dlglist2.h"
#include "httpquerydialog.h"
#include "c5cache.h"
#include "c5message.h"
#include "c5database.h"
#include "c5utils.h"
#include "c5config.h"
#include "c5printrecipta4.h"
#include <QMenu>
#include <QClipboard>
#include <QFileDialog>
#include <QDesktopServices>
#include <QXlsx/header/xlsxdocument.h>

C5SaleFromStoreOrder::C5SaleFromStoreOrder() :
    C5Dialog(),
    ui(new Ui::C5SaleFromStoreOrder)
{
    ui->setupUi(this);
    ui->tblData->setColumnWidths(ui->tblData->columnCount(), 0, 0, 300, 80, 80, 80, 80, 0);
    ui->leID->setVisible(false);
    ui->lePartner->setSelector(ui->lePartnerName, cache_goods_partners);
    ui->leSeller->setSelector(ui->leSellerName, cache_users);
    ui->leHall->setSelector(ui->leHallName, cache_halls);
    //ui->btnRemove->setVisible(pr(cp_t5_refund_goods));
}

C5SaleFromStoreOrder::~C5SaleFromStoreOrder()
{
    delete ui;
}

void C5SaleFromStoreOrder::openOrder(const QString &id)
{
    C5SaleFromStoreOrder *d = new C5SaleFromStoreOrder();
    d->loadOrder(id);
    d->exec();
    delete d;
}

void C5SaleFromStoreOrder::removeOrderResponse(const QJsonObject &jdoc)
{
    accept();
    C5Message::info(tr("Removal complete"));
}

void C5SaleFromStoreOrder::loadOrder(const QString &id)
{
    ui->leID->setText(id);
    C5Database db;
    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");

    if(db.nextRow()) {
        ui->leUUID->setText(id);
        ui->deDate->setDate(db.getDate("f_datecash"));
        ui->teTime->setText(db.getString("f_timeclose"));
        ui->leUserId->setText(db.getString("f_prefix") + QString::number(db.getInt("f_hallid")));
        ui->leTotal->setDouble(db.getDouble("f_amounttotal"));
        ui->leTotalCash->setDouble(db.getDouble("f_amountcash"));
        ui->leTotalCard->setDouble(db.getDouble("f_amountcard"));
        ui->lePartner->setValue(db.getString("f_partner"));
        ui->leHall->setValue(db.getInt("f_hall"));
        ui->btnRemove->setVisible(db.getInt("f_source") > 0);
        ui->leSeller->setValue(db.getInt("f_staff"));
    } else {
        C5Message::error(tr("No such order"));
        return;
    }

    db[":f_order"] = ui->leID->text();
    db.exec("select * from b_clients_debts where f_order=:f_order");
    ui->chDebt->setChecked(db.nextRow());
    db[":f_id"] = id;
    db.exec("select og.f_id, og.f_goods, g.f_name, og.f_qty, gu.f_name, og.f_price, og.f_total,"
            "t.f_taxdept, t.f_adgcode, "
            "og.f_store, g.f_scancode "
            "from o_goods og "
            "left join c_goods g on g.f_id=og.f_goods "
            "left join c_units gu on gu.f_id=g.f_unit "
            "left join c_groups t on t.f_id=g.f_group "
            "where og.f_header=:f_id");

    while(db.nextRow()) {
        int row = ui->tblData->addEmptyRow();
        ui->tblData->setString(row, 0, db.getString(0));
        ui->tblData->setInteger(row, 1, db.getInt(1));
        ui->tblData->setString(row, 2, db.getString(2) + " " + db.getString("f_scancode"));
        ui->tblData->setDouble(row, 3, db.getDouble(3));
        ui->tblData->setString(row, 4, db.getString(4));
        ui->tblData->setDouble(row, 5, db.getDouble(5));
        ui->tblData->setDouble(row, 6, db.getDouble(6));
        ui->tblData->setInteger(row, 7, db.getInt(7));
        ui->tblData->setString(row, 8, db.getString("f_taxdept"));
        ui->tblData->setString(row, 9, db.getString("f_adgcode"));
    }

    db[":f_id"] = id;
    db.exec("select * from o_tax where f_id=:f_id");
    ui->btnPrintTax->setVisible(!db.nextRow());
}

void C5SaleFromStoreOrder::exportToAS(int doctype)
{
    C5Database db;
    db.exec("select * from as_list");

    if(db.rowCount() == 0) {
        C5Message::error(tr("ArmSoft is not configure"));
        return;
    }

    int dbid;
    int index = 0;
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
    } else {
        dbid = db.getInt(0, "f_id");
        connStr = db.getString(0, "f_connectionstring");
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
    jo["draftid"] = ui->leUUID->text();
    jo["doctype"] = doctype;
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
    HttpQueryDialog *qd = new HttpQueryDialog(QString("https://%1:%2/magnit").arg(b->ipAddress,
        QString::number(b->port)), jo, this);
    qd->exec();
    qd->deleteLater();
}

void C5SaleFromStoreOrder::on_btnRemove_clicked()
{
    if(C5Message::question(tr("Confirm to remove")) != QDialog::Accepted) {
        return;
    }

    fHttp->createHttpQuery("/engine/shop/remove-order.php", QJsonObject{{"id", ui->leUUID->text()}}, SLOT(
        removeOrderResponse(QJsonObject)));
}

void C5SaleFromStoreOrder::on_btnPrintTax_clicked()
{
    QElapsedTimer t;
    t.start();
    C5Database db;
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(),
                 C5Config::taxCashier(), C5Config::taxPin(), this);

    for(int i = 0; i < ui->tblData->rowCount(); i++) {
        pt.addGoods(ui->tblData->getString(i, 8).toInt(), //dep
                    ui->tblData->getString(i, 9), //adg
                    ui->tblData->getString(i, 1), //goods id
                    ui->tblData->getString(i, 2), //name
                    ui->tblData->getDouble(i, 5), //price
                    ui->tblData->getDouble(i, 3), //qty
                    0); //discount
    }

    QString jsonIn, jsonOut, err;
    QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
    int result = 0;
    result = pt.makeJsonAndPrint(ui->leTotalCard->getDouble(), 0, jsonIn, jsonOut, err);

    if(result == pt_err_ok) {
        QJsonObject jtax;
        jtax["f_order"] = ui->leID->text();
        jtax["f_elapsed"] = t.elapsed();
        jtax["f_in"] = jsonIn;
        jtax["f_out"] = jsonOut;
        jtax["f_err"] = err;
        jtax["f_result"] = result;
        jtax["f_state"] = result == pt_err_ok ? 1 : 0;
        db.exec(QString("call sf_create_shop_tax('%1')").arg(QString(QJsonDocument(jtax).toJson(QJsonDocument::Compact))));

        if(result != pt_err_ok) {
            C5Message::error(err);
        }
    }
}

void C5SaleFromStoreOrder::on_btnSave_clicked()
{
    QString err;

    if(!ui->deDate->date().isValid()) {
        err += tr("Date is not valid");
    }

    if(!err.isEmpty()) {
        C5Message::error(err);
        return;
    }

    C5Database db;

    if(ui->chDebt->isChecked()) {
        if(ui->lePartner->getInteger() == 0) {
            C5Message::error(tr("Dept option is selected, but no partner selected"));
            return;
        }

        db[":f_order"] = ui->leID->text();
        db.exec("select * from b_clients_debts where f_order=:f_order");

        if(db.nextRow()) {
            db[":f_costumer"] = ui->lePartner->getInteger();
            db.update("b_clients_debts", "f_order", ui->leID->text());
        } else {
            db[":f_order"] = ui->leID->text();
            db[":f_costumer"] = ui->lePartner->getInteger();
            db[":f_date"] = ui->deDate->date();
            db[":f_amount"] = ui->leTotalCash->getDouble() * -1;
            db[":f_currency"] = 1;
            db[":f_source"] = 1;
            db.insert("b_clients_debts");
        }
    } else {
        db.deleteFromTable("b_clients_debts", "f_order", ui->leID->text());
    }

    db[":f_partner"] = ui->lePartner->getInteger();
    db[":f_staff"] = ui->leSeller->getInteger();
    db[":f_datecash"] = ui->deDate->date();
    db[":f_hall"] = ui->leHall->getInteger();
    db.update("o_header", where_id(ui->leID->text()));
    C5Message::info(tr("Saved"));
}

void C5SaleFromStoreOrder::on_btnCopyUUID_clicked()
{
    qApp->clipboard()->setText(ui->leUUID->text());
}

void C5SaleFromStoreOrder::on_btnPrintA4_clicked()
{
    C5PrintReciptA4 p(ui->leUUID->text(), this);
    QString err;

    if(!p.print(err)) {
        C5Message::error(err);
    }
}

void C5SaleFromStoreOrder::on_btnExportToExcel_clicked()
{
    QXlsx::Document d;
    d.addSheet("Sheet1");
    /* HEADER */
    QColor color = Qt::white;
    QFont headerFont(qApp->font());
    QXlsx::Format hf;
    hf.setFont(headerFont);
    hf.setBorderStyle(QXlsx::Format::BorderThin);
    hf.setPatternBackgroundColor(color);
    d.setColumnWidth(1, 10);
    d.setColumnWidth(2, 15);
    d.setColumnWidth(3, 50);
    d.setColumnWidth(4, 20);
    d.setColumnWidth(5, 20);
    d.setColumnWidth(6, 20);
    d.setColumnWidth(7, 20);
    int col = 1, row = 1;
    d.write(row, col, QString("%1 N%2").arg(tr("Order"), ui->leUserId->text()), hf);
    row++;

    if(!ui->lePartnerName->isEmpty()) {
        d.write(row, col, tr("Buyer") + " " + ui->lePartnerName->text(), hf);
        row++;
    }

    QList<int> cols;
    QStringList vals;
    col = 1;
    cols << col++;
    vals << tr("Date");

    for(int i = 0; i < cols.count(); i++) {
        d.write(row, cols.at(i), vals.at(i), hf);
    }

    row++;
    vals.clear();
    vals << ui->deDate->text() + " " + ui->teTime->text();

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
    bf.setFont(bodyFont);
    bf.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    bf.setBorderStyle(QXlsx::Format::BorderThin);

    for(int i = 0; i < ui->tblData->rowCount(); i++) {
        vals.clear();
        vals << QString::number(i + 1);
        vals << ui->tblData->getString(i, 1);
        vals << ui->tblData->getString(i, 2);
        vals << ui->tblData->getString(i, 3);
        vals << ui->tblData->getString(i, 4);
        vals << ui->tblData->getString(i, 5);
        vals << ui->tblData->getString(i, 6);

        for(int i = 0; i < cols.count(); i++) {
            d.write(row, cols.at(i), vals.at(i), bf);
        }

        row++;
    }

    cols.clear();
    cols << 6 << 7;
    vals.clear();
    vals << tr("Total amount");
    vals << QString::number(str_float(ui->leTotal->text()));

    for(int i = 0; i < cols.count(); i++) {
        d.write(row, cols.at(i), vals.at(i), hf);
    }

    row++;
    col = 1;
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

void C5SaleFromStoreOrder::on_btnPrintA4_3_clicked()
{
    exportToAS(20);
}

void C5SaleFromStoreOrder::on_btnPrintA4_2_clicked()
{
    exportToAS(5);
}
