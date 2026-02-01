#include "sales.h"
#include "ui_sales.h"
#include "c5config.h"
#include "printreceiptgroup.h"
#include "c5database.h"
#include "selectprinters.h"
#include "c5utils.h"
#include "c5message.h"
#include "printtaxn.h"
#include "vieworder.h"
#include "c5permissions.h"
#include "cashcollection.h"
#include "c5permissions.h"
#include "c5user.h"
#include "c5permissions.h"
#include "dlgreturnitem.h"
#include "dlgdate.h"
#include <QPropertyAnimation>

#define VM_TOTAL 0
#define VM_ITEMS 1
#define VM_TOTAL_ITEMS 2

Sales::Sales(C5User *user) :
    C5ShopDialog(user),
    ui(new Ui::Sales)
{
    ui->setupUi(this);
    fUser = user;
    setWindowTitle(__c5config.getRegValue("windowtitle").toString());
    //TODO ui->btnChangeDate->setVisible(fUser->check(cp_t12_change_date_of_sale));
    ui->btnModeItems->setVisible(fUser->check(cp_t12_shop_report_goods));
    ui->btnTotalByItems->setVisible(fUser->check(cp_t12_shop_report_goods));
    fViewMode = VM_TOTAL;
    ui->lbTotalQty->setVisible(false);
    ui->leTotal->setVisible(__c5config.fMainJson["hide_revenue"].toBool() == false);
    ui->lbTotalAmount->setVisible(ui->leTotal->isVisible());
    ui->leTotalQty->setVisible(__c5config.fMainJson["hide_reenue"].toBool() == false);
    ui->lbTotalQty->setVisible(ui->leTotalQty->isVisible());
    ui->wMenuPanel->setMinimumWidth(0);
    ui->wMenuPanel->setMaximumWidth(0);
    ui->wMenuPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mPanelAnim = new QPropertyAnimation(ui->wMenuPanel, "maximumWidth", this);
    mPanelAnim->setDuration(220);
    mPanelAnim->setEasingCurve(QEasingCurve::OutCubic);
    refresh();
}

Sales::~Sales()
{
    delete ui;
    delete mUser;
}

void Sales::showSales(Working *w, C5User *u)
{
    Sales *s = new Sales(u);
    s->fWorking = w;
    s->showMaximized();
}

bool Sales::printCheckWithTax(C5Database &db, const QString &id)
{
    QElapsedTimer et;
    et.start();
    bool resultb = true;
    db[":f_id"] = id;
    db.exec("select * from o_tax where f_id=:f_id");

    if(db.nextRow()) {
        if(db.getInt("f_receiptnumber") > 0) {
            C5Message::error(tr("Cannot print tax twice"));
            return resultb;
        }
    }

    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");
    db.nextRow();
    double card = db.getDouble("f_amountcard");
    double prepaid = db.getDouble("f_amountprepaid");
    double idram = db.getDouble("f_idram");
    int partner = db.getInt("f_partner");
    QString useExtPos = idram > 0.01 ? "true" : C5Config::taxUseExtPos();
    QString partnerHvhh;

    if(partner > 0) {
        db[":f_id"] = partner;
        db.exec("select f_taxcode from c_partners where f_id=:f_id");

        if(db.nextRow()) {
            partnerHvhh = db.getString(0);
        }
    }

    db[":f_id"] = id;
    db.exec("select og.f_id, og.f_goods, g.f_name, og.f_qty, gu.f_name as f_unitname, og.f_price, og.f_total,"
            "t.f_taxdept, t.f_adgcode, "
            "og.f_store "
            "from o_goods og "
            "left join c_goods g on g.f_id=og.f_goods "
            "left join c_units gu on gu.f_id=g.f_unit "
            "left join c_groups t on t.f_id=g.f_group "
            "where og.f_header=:f_id");
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), useExtPos, C5Config::taxCashier(),
                 C5Config::taxPin(), 0);
    pt.fPartnerTin = partnerHvhh;

    while(db.nextRow()) {
        pt.addGoods(db.getString("f_taxdept").toInt(), //dep
                    db.getString("f_adgcode"), //adg
                    db.getString("f_goods"), //goods id
                    db.getString("f_name"), //name
                    db.getDouble("f_price"), //price
                    db.getDouble("f_qty"), //qty
                    db.getDouble("f_discountfactor") * 100); //discount
    }

    QString jsonIn, jsonOut, err;
    int result = 0;
    result = pt.makeJsonAndPrint(card, prepaid, jsonIn, jsonOut, err);
    QJsonObject jtax;
    jtax["f_order"] = id;
    jtax["f_elapsed"] = et.elapsed();
    jtax["f_in"] = jsonIn;
    jtax["f_out"] = jsonOut;
    jtax["f_err"] = err;
    jtax["f_result"] = result;
    jtax["f_state"] = result == pt_err_ok ? 1 : 0;
    db.exec(QString("call sf_create_shop_tax('%1')").arg(QString(QJsonDocument(jtax).toJson(QJsonDocument::Compact))));

    if(result == pt_err_ok) {
        C5Message::info(tr("Printed"));
    } else {
        C5Message::error(err);
    }

    return resultb;
}

bool Sales::printReceipt(const QString &id, C5User *user)
{
    if(!C5Config::localReceiptPrinter().isEmpty()) {
        C5Database db;
        PrintReceiptGroup p;

        switch(C5Config::shopPrintVersion()) {
        case 1: {
            bool p1, p2;

            if(SelectPrinters::selectPrinters(p1, p2, user)) {
                if(p1) {
                    p.print(id, db, 1);
                }

                if(p2) {
                    p.print(id, db, 2);
                }
            }

            break;
        }

        case 2:
            p.print2(id, db);
            break;

        default:
            break;
        }
    }

    return true;
}

void Sales::on_btnDateLeft_clicked()
{
    changeDate(-1);
}

void Sales::changeDate(int d)
{
    ui->deStart->setDate(ui->deStart->date().addDays(d));
    ui->deEnd->setDate(ui->deEnd->date().addDays(d));
    refresh();
}

void Sales::refresh()
{
    int md = mUser->fConfig["shop_max_days_of_history"].toInt();

    if(md > 0) {
        if(QDate::currentDate().addDays(-md) > ui->deStart->date()) {
            ui->deStart->setDate(QDate::currentDate().addDays(-md));
            ui->deEnd->setDate(ui->deStart->date());
        }
    }

    switch(fViewMode) {
    case VM_TOTAL:
        refreshTotal();
        break;

    case VM_ITEMS:
        refreshItems();
        break;

    case VM_TOTAL_ITEMS:
        refreshTotalItems();
        break;
    }
}

void Sales::refreshTotal()
{
    QStringList h;
    h.append("X");
    h.append(tr("UUID"));
    h.append(tr("Sale type code"));
    h.append(tr("Seller"));
    h.append(tr("Sale type"));
    h.append(tr("Number"));
    h.append(tr("##"));
    h.append(tr("Date"));
    h.append(tr("Time"));
    h.append(tr("Amount"));
    h.append(tr("Customer"));
    h.append(tr("Deliverman"));
    h.append(tr("Address"));
    ui->tbl->setColumnCount(h.count());
    ui->tbl->setHorizontalHeaderLabels(h);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 40, 0, 0, 0, 120, 0, 100, 120, 120, 150, 100, 100, 300);
    C5Database db;
    db[":f_hall"] = __c5config.defaultHall();
    db[":f_start"] = ui->deStart->date();
    db[":f_end"] = ui->deEnd->date();
    db[":f_state"] = ORDER_STATE_CLOSE;
    QString sqlCond = "";

    if(!fUser->check(cp_t12_shop_fiscal_report)) {
        sqlCond += " and length(ot.f_receiptnumber)>0 ";
    }

    if(!fUser->check(cp_t12_shop_sale_of_all_users)) {
        sqlCond += QString(" and oh.f_staff=%1 ").arg(fUser->id());
    }

    QString sql =
        QString("select '', oh.f_id, oh.f_saletype, u.f_login, os.f_name, "
            "concat(oh.f_prefix, oh.f_hallid) as f_number, "
            "ot.f_receiptnumber, "
            "oh.f_datecash, oh.f_timeclose, oh.f_amounttotal, "
            "concat(c.f_taxname, ' ', c.f_contact) as f_client, "
            "concat_ws(' ', dm.f_last, dm.f_first) as f_deliverman, "
            "oh.f_comment "
            "from o_header oh "
            "left join o_header_options oo on oo.f_id=oh.f_id "
            "left join b_history h on h.f_id=oh.f_id "
            "left join b_cards_discount d on d.f_id=h.f_card "
            "left join c_partners c on c.f_id=oh.f_partner "
            "left join o_tax ot on ot.f_id=oh.f_id "
            "left join o_sale_type os on os.f_id=oh.f_saletype "
            "left join s_user u on u.f_id=oh.f_staff "
            "left join s_user dm on dm.f_id=oo.f_deliveryman "
            "where oh.f_datecash between :f_start and :f_end "
            "and oh.f_state=:f_state " + sqlCond +
            "and oh.f_hall=:f_hall "
            "order by oh.f_datecash, oh.f_timeclose ");
    db.exec(sql);
    ui->tbl->setRowCount(db.rowCount());
    int row = 0;

    while(db.nextRow()) {
        for(int i = 0; i < ui->tbl->columnCount(); i++) {
            ui->tbl->setData(row, i, db.getValue(i));
        }

        qDebug() << db.getInt("f_saletype") << db.getDouble("f_amounttotal");
        ui->tbl->setInteger(row, 2, db.getInt("f_saletype"));
        ui->tbl->setDouble(row, 9, db.getDouble("f_amounttotal"));
        qDebug() << ui->tbl->getDouble(row, 9) << ui->tbl->getData(row, 9);
        ui->tbl->item(row, 0)->setCheckState(Qt::Unchecked);
        row++;
    }

    int acol = 9;
    ui->leTotal->setDouble(ui->tbl->sumOfColumn(acol));
}

void Sales::refreshItems()
{
    QStringList h;
    h.append(tr("UUID"));
    h.append(tr("Sale type code"));
    h.append(tr("Seller"));
    h.append(tr("Sale type"));
    h.append(tr("Number"));
    h.append(tr("##"));
    h.append(tr("Date"));
    h.append(tr("Time"));
    h.append(tr("Scancode"));
    h.append(tr("Goods"));
    h.append(tr("Qty"));
    h.append(tr("Price"));
    h.append(tr("Total"));
    ui->tbl->setColumnCount(h.count());
    ui->tbl->setHorizontalHeaderLabels(h);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 0, 0, 120, 0, 100, 120, 100, 150, 250, 80, 80, 80);
    C5Database db;
    db[":f_hall"] = __c5config.defaultHall();
    db[":f_start"] = ui->deStart->date();
    db[":f_end"] = ui->deEnd->date();
    db[":f_state"] = ORDER_STATE_CLOSE;
    QString sqlCond = "";

    if(!fUser->check(cp_t12_shop_fiscal_report)) {
        sqlCond += " and length(ot.f_receiptnumber)>0 ";
    }

    if(!fUser->check(cp_t12_shop_sale_of_all_users)) {
        sqlCond += QString(" and oh.f_staff=%1 ").arg(fUser->id());
    }

    db.exec("select oh.f_id, oh.f_saletype, u.f_login, os.f_name, concat(oh.f_prefix, oh.f_hallid) as f_number, ot.f_receiptnumber, "
            "oh.f_datecash, oh.f_timeclose, g.f_scancode, g.f_name as f_goodsname, og.f_qty, og.f_price, og.f_total "
            "from o_goods og "
            "inner join o_header oh on oh.f_id=og.f_header "
            "inner join c_goods g on g.f_id=og.f_goods "
            "left join b_history h on h.f_id=oh.f_id "
            "left join b_cards_discount d on d.f_id=h.f_card "
            "left join c_partners c on c.f_id=d.f_client "
            "left join o_tax ot on ot.f_id=oh.f_id "
            "left join o_sale_type os on os.f_id=oh.f_saletype "
            "left join s_user u on u.f_id=oh.f_staff "
            "where oh.f_datecash between :f_start and :f_end and oh.f_state=:f_state " + sqlCond +
            "and oh.f_hall=:f_hall "
            "order by oh.f_datecash, oh.f_timeclose ");
    ui->tbl->setRowCount(db.rowCount());
    int row = 0;

    while(db.nextRow()) {
        for(int i = 0; i < ui->tbl->columnCount(); i++) {
            ui->tbl->setData(row, i, db.getValue(i));
        }

        row++;
    }

    int acol = 12;
    ui->leTotal->setDouble(ui->tbl->sumOfColumn(acol));
}

void Sales::refreshTotalItems()
{
    QStringList h;
    h.append(tr("Scancode"));
    h.append(tr("Goods"));
    h.append(tr("Qty"));
    h.append(tr("Total"));
    ui->tbl->setColumnCount(h.count());
    ui->tbl->setHorizontalHeaderLabels(h);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 150, 250, 80, 80);
    C5Database db;
    db[":f_hall"] = __c5config.defaultHall();
    db[":f_start"] = ui->deStart->date();
    db[":f_end"] = ui->deEnd->date();
    db[":f_state"] = ORDER_STATE_CLOSE;
    QString sqlCond = "";

    if(!fUser->check(cp_t12_shop_fiscal_report)) {
        sqlCond += " and length(ot.f_receiptnumber)>0 ";
    }

    if(!fUser->check(cp_t12_shop_sale_of_all_users)) {
        sqlCond += QString(" and oh.f_staff=%1 ").arg(fUser->id());
    }

    db.exec("select g.f_scancode, g.f_name as f_goodsname, sum(og.f_qty), sum(og.f_total) "
            "from o_goods og "
            "inner join o_header oh on oh.f_id=og.f_header "
            "inner join c_goods g on g.f_id=og.f_goods "
            "left join s_user u on u.f_id=oh.f_staff "
            "left join o_tax ot on ot.f_id=oh.f_id "
            "where oh.f_datecash between :f_start and :f_end and oh.f_state=:f_state " + sqlCond +
            "and oh.f_hall=:f_hall "
            "group by 1, 2 "
            "order by oh.f_datecash, oh.f_timeclose ");
    ui->tbl->setRowCount(db.rowCount());
    int row = 0;

    while(db.nextRow()) {
        for(int i = 0; i < ui->tbl->columnCount(); i++) {
            ui->tbl->setData(row, i, db.getValue(i));
        }

        row++;
    }

    int acol = 3;
    ui->leTotal->setDouble(ui->tbl->sumOfColumn(acol));
}

void Sales::printTaxReport(int report_type)
{
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(),
                 C5Config::taxCashier(), C5Config::taxPin(), this);
    QString jsnin, jsnout, err;
    int result;
    result = pt.printReport(ui->deStart->date(), ui->deEnd->date(),
                            report_type, jsnin, jsnout, err);
    C5Database db;
    db[":f_id"] = db.uuid();
    db[":f_order"] = QString("Report %1").arg(report_type == report_x ? "X" : "Z");
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_in"] = jsnin;
    db[":f_out"] = jsnout;
    db[":f_err"] = err;
    db[":f_result"] = result;
    db.insert("o_tax_log", false);
}

int Sales::sumOfColumnsWidghtBefore(int column)
{
    int sum = 0;

    for(int i = 0; i < column; i++) {
        sum += ui->tbl->columnWidth(i);
    }

    return sum;
}

void Sales::toggleMenu(bool visible)
{
    const int W = 300;
    mPanelAnim->stop();
    mPanelAnim->setStartValue(ui->wMenuPanel->width());
    mPanelAnim->setEndValue(visible ? W : 0);
    mPanelAnim->start();
}

void Sales::on_btnDateRight_clicked()
{
    changeDate(1);
}

void Sales::on_btnItemBack_clicked()
{
    toggleMenu(false);
    //todo: memory leak
    DlgReturnItem *i = new DlgReturnItem(mUser);
    i->exec();
}

void Sales::on_btnRefresh_clicked()
{
    refresh();
}

void Sales::on_btnModeTotal_clicked()
{
    toggleMenu(false);
    ui->btnModeItems->setChecked(false);
    ui->btnModeTotal->setChecked(true);
    ui->btnTotalByItems->setChecked(false);
    ui->btnViewOrder->setEnabled(true);
    ui->btnItemBack->setEnabled(true);
    fViewMode = VM_TOTAL;
    refresh();
}

void Sales::on_btnModeItems_clicked()
{
    toggleMenu(false);
    ui->btnModeItems->setChecked(true);
    ui->btnModeTotal->setChecked(false);
    ui->btnTotalByItems->setChecked(false);
    ui->btnViewOrder->setEnabled(true);
    ui->btnItemBack->setEnabled(true);
    fViewMode = VM_ITEMS;
    refresh();
}

void Sales::on_btnTotalByItems_clicked()
{
    toggleMenu(false);
    ui->btnModeItems->setChecked(false);
    ui->btnModeTotal->setChecked(false);
    ui->btnTotalByItems->setChecked(true);
    ui->btnViewOrder->setEnabled(false);
    ui->btnItemBack->setEnabled(false);
    fViewMode = VM_TOTAL_ITEMS;
    refresh();
}

void Sales::on_btnCashColletion_clicked()
{
    toggleMenu(false);
    auto *cc = new CashCollection(mUser);
    cc->exec();
    delete cc;
}

void Sales::on_leFilter_textChanged(const QString &arg1)
{
    for(int r = 0; r < ui->tbl->rowCount(); r++) {
        bool h = true;

        for(int c = 0; c < ui->tbl->columnCount(); c++) {
            if(ui->tbl->getString(r, c).contains(arg1, Qt::CaseInsensitive)) {
                h = false;
                break;
            }
        }

        ui->tbl->setRowHidden(r, h);
    }
}

void Sales::on_btnPrintTaxZ_clicked()
{
    toggleMenu(false);
    printTaxReport(report_z);
}

void Sales::on_btnPrintTaxX_clicked()
{
    toggleMenu(false);
    printTaxReport(report_x);
}

void Sales::on_btnExit_clicked()
{
    reject();
    deleteLater();
}

void Sales::on_btnViewOrder_clicked()
{
    toggleMenu(false);
    QModelIndexList ml = ui->tbl->selectionModel()->selectedRows();

    if(ml.count() == 0) {
        return;
    }

    int col = fViewMode == VM_TOTAL ? 1 : 0;
    ViewOrder(fWorking, ui->tbl->getString(ml.at(0).row(), col), mUser).exec();
}

void Sales::on_btnChangeDate_clicked()
{
    toggleMenu(false);

    if(fViewMode != VM_TOTAL) {
        return;
    }

    QDate d;
    C5Database db;

    for(int  i = 0; i < ui->tbl->rowCount(); i++) {
        if(ui->tbl->item(i, 0)->checkState() == Qt::Checked) {
            if(!d.isValid()) {
                if(!DlgDate::getDate(d, mUser)) {
                    return;
                }
            }

            db[":f_datecash"] = d;
            db[":f_id"] = ui->tbl->getString(i, 1);

            if(!db.exec("update o_header set f_datecash=:f_datecash where f_id=:f_id")) {
                C5Message::error(db.fLastError);
                return;
            }
        }
    }

    if(d.isValid()) {
        refresh();
    } else {
        C5Message::info(tr("Nothing was selected"));
    }
}

void Sales::on_btnItemChange_clicked()
{
    toggleMenu(false);
    //todo: memory leak
    DlgReturnItem *i = new DlgReturnItem(mUser);;
    i->setMode(2);
    i->showMaximized();
}

void Sales::on_btnShowMenu_clicked()
{
    toggleMenu(true);
}

void Sales::on_btnCloseMenu_clicked()
{
    toggleMenu(false);
}
