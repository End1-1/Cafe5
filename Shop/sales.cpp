#include "sales.h"
#include "ui_sales.h"
#include "c5config.h"
#include "printreceiptgroup.h"
#include "c5database.h"
#include "selectprinters.h"
#include "c5replication.h"
#include "c5utils.h"
#include "c5message.h"
#include "printtaxn.h"
#include "sslsocket.h"
#include "datadriver.h"
#include "vieworder.h"
#include "cashcollection.h"
#include "c5printpreview.h"
#include "c5printing.h"
#include "dlgdate.h"

#define VM_TOTAL 0
#define VM_ITEMS 1
#define VM_TOTAL_ITEMS 2
#define VM_GROUPS 3

Sales::Sales() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::Sales)
{
    ui->setupUi(this);
    setWindowTitle(dbhall->name(__c5config.defaultHall()) + "," + dbstore->name(__c5config.defaultStore()));
    ui->lbRetail->setVisible(!__c5config.shopDenyF1());
    ui->leRetail->setVisible(!__c5config.shopDenyF1());
    ui->lbWhosale->setVisible(!__c5config.shopDenyF2());
    ui->leWhosale->setVisible(!__c5config.shopDenyF2());
    ui->btnChangeDate->setVisible(pr(__c5config.dbParams().at(1), cp_t5_change_date_of_sale));
    fViewMode = VM_TOTAL;
    C5Database db(__c5config.dbParams());
    if (__c5config.rdbReplica()) {
        db.exec("select * from o_header");
        ui->btnRetryUpload->setVisible(db.nextRow());
    } else {
        ui->btnRetryUpload->setVisible(false);
    }
    refresh();
}

Sales::~Sales()
{
    delete ui;
}

void Sales::showSales()
{
    Sales *s = new Sales();
    s->showMaximized();
    s->exec();
    delete s;
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
    switch (fViewMode) {
    case VM_TOTAL:
        refreshTotal();
        break;
    case VM_ITEMS:
        refreshItems();
        break;
    case VM_TOTAL_ITEMS:
        refreshTotalItems();
        break;
    case VM_GROUPS:
        refreshGroups();
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
    h.append(tr("Tax"));
    h.append(tr("Date"));
    h.append(tr("Time"));
    h.append(tr("Amount"));
    h.append(tr("Customer"));
    ui->tbl->setColumnCount(h.count());
    ui->tbl->setHorizontalHeaderLabels(h);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 40, 0, 0, 80, 120, 120, 100, 120, 120, 150, 300);
    C5Database db(__c5config.replicaDbParams());
    db[":f_hall"] = __c5config.defaultHall();
    db[":f_start"] = ui->deStart->date();
    db[":f_end"] = ui->deEnd->date();
    db[":f_state"] = ORDER_STATE_CLOSE;
    db.exec("select '', oh.f_id, oh.f_saletype, u.f_login, os.f_name, concat(oh.f_prefix, oh.f_hallid) as f_number, ot.f_receiptnumber, "
            "oh.f_datecash, oh.f_timeclose, oh.f_amounttotal, concat(c.f_taxname, ' ', c.f_contact) as f_client "
            "from o_header oh "
            "left join b_history h on h.f_id=oh.f_id "
            "left join b_cards_discount d on d.f_id=h.f_card "
            "left join c_partners c on c.f_id=oh.f_partner "
            "left join o_tax ot on ot.f_id=oh.f_id "
            "left join o_sale_type os on os.f_id=oh.f_saletype "
            "left join s_user u on u.f_id=oh.f_staff "
            "where oh.f_datecash between :f_start and :f_end and oh.f_state=:f_state " + userCond() +
            "and oh.f_hall=:f_hall "
            "order by oh.f_datecash, oh.f_timeclose ");
    ui->tbl->setRowCount(db.rowCount());
    int row = 0;
    while (db.nextRow()) {
        for (int i = 0; i < ui->tbl->columnCount(); i++) {
            ui->tbl->setData(row, i, db.getValue(i));
        }
        ui->tbl->item(row, 0)->setCheckState(Qt::Unchecked);
        row++;
    }
    int acol = 9;
    ui->leTotal->setDouble(ui->tbl->sumOfColumn(acol));
    double retail = 0, whosale = 0;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (ui->tbl->getInteger(i, 1) == SALE_RETAIL) {
            retail += ui->tbl->getDouble(i, acol);
        } else {
            whosale += ui->tbl->getDouble(i, acol);
        }
    }
    ui->leRetail->setDouble(retail);
    ui->leWhosale->setDouble(whosale);
}

void Sales::refreshItems()
{
    QStringList h;
    h.append(tr("UUID"));
    h.append(tr("Sale type code"));
    h.append(tr("Seller"));
    h.append(tr("Sale type"));
    h.append(tr("Number"));
    h.append(tr("Tax"));
    h.append(tr("Date"));
    h.append(tr("Time"));
    h.append(tr("Scancode"));
    h.append(tr("Goods"));
    h.append(tr("Qty"));
    h.append(tr("Price"));
    h.append(tr("Total"));
    ui->tbl->setColumnCount(h.count());
    ui->tbl->setHorizontalHeaderLabels(h);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 0, 80, 120, 120, 100, 120, 100, 150, 250, 80, 80, 80);
    C5Database db(__c5config.replicaDbParams());
    db[":f_hall"] = __c5config.defaultHall();
    db[":f_start"] = ui->deStart->date();
    db[":f_end"] = ui->deEnd->date();
    db[":f_state"] = ORDER_STATE_CLOSE;
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
            "where oh.f_datecash between :f_start and :f_end and oh.f_state=:f_state " + userCond() +
            "and oh.f_hall=:f_hall "
            "order by oh.f_datecash, oh.f_timeclose ");
    ui->tbl->setRowCount(db.rowCount());
    int row = 0;
    while (db.nextRow()) {
        for (int i = 0; i < ui->tbl->columnCount(); i++) {
            ui->tbl->setData(row, i, db.getValue(i));
        }
        row++;
    }
    int acol = 11;
    ui->leTotal->setDouble(ui->tbl->sumOfColumn(acol));
    double retail = 0, whosale = 0;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (ui->tbl->getInteger(i, 1) == SALE_RETAIL) {
            retail += ui->tbl->getDouble(i, acol);
        } else {
            whosale += ui->tbl->getDouble(i, acol);
        }
    }
    ui->leRetail->setDouble(retail);
    ui->leWhosale->setDouble(whosale);
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
    C5Database db(__c5config.replicaDbParams());
    db[":f_hall"] = __c5config.defaultHall();
    db[":f_start"] = ui->deStart->date();
    db[":f_end"] = ui->deEnd->date();
    db[":f_state"] = ORDER_STATE_CLOSE;
    db.exec("select g.f_scancode, g.f_name as f_goodsname, sum(og.f_qty), sum(og.f_total) "
            "from o_goods og "
            "inner join o_header oh on oh.f_id=og.f_header "
            "inner join c_goods g on g.f_id=og.f_goods "
            "left join s_user u on u.f_id=oh.f_staff "
            "where oh.f_datecash between :f_start and :f_end and oh.f_state=:f_state " + userCond() +
            "and oh.f_hall=:f_hall "
            "group by 1, 2 "
            "order by oh.f_datecash, oh.f_timeclose ");
    ui->tbl->setRowCount(db.rowCount());
    int row = 0;
    while (db.nextRow()) {
        for (int i = 0; i < ui->tbl->columnCount(); i++) {
            ui->tbl->setData(row, i, db.getValue(i));
        }
        row++;
    }
    int acol = 3;
    ui->leTotal->setDouble(ui->tbl->sumOfColumn(acol));
    ui->leRetail->setDouble(0);
    ui->leWhosale->setDouble(0);
}

void Sales::refreshGroups()
{
    QStringList h;
    h.append(tr("Group"));
    h.append(tr("Qty"));
    h.append(tr("Amount"));
    //h.append(tr(""));

    ui->tbl->clearContents();
    ui->tbl->setRowCount(0);
    ui->tbl->setColumnCount(h.count());
    ui->tbl->setHorizontalHeaderLabels(h);
    //ui->tbl->setColumnWidths(ui->tbl->columnCount(), 180, 60, 60, 10, 180, 60, 60, 10, 180, 60, 60);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 200, 80, 80);
    C5Database db(__c5config.replicaDbParams());
    db[":f_hall"] = __c5config.defaultHall();
    db[":f_start"] = ui->deStart->date();
    db[":f_end"] = ui->deEnd->date();
    db[":f_state"] = ORDER_STATE_CLOSE;
    db.exec("select oh.f_datecash, h.f_name as f_hallname, gg.f_name as f_groupname, "
            "sum(og.f_qty * og.f_sign) as f_qty, sum(og.f_total*og.f_sign) as f_total "
            "from o_goods og "
            "inner join o_header oh on oh.f_id=og.f_header "
            "inner join c_goods g on g.f_id=og.f_goods "
            "left join c_groups gg on gg.f_id=g.f_group "
            "left join s_user u on u.f_id=oh.f_staff "
            "left join h_halls h on h.f_id=oh.f_hall "
            "where oh.f_datecash between :f_start and :f_end and oh.f_state=:f_state " + userCond() +
            "and oh.f_hall=:f_hall "
            "group by 1, 2, 3 "
            "order by 3 desc ");
    int row = 0, col = 0;
    ui->leTotal->setDouble(0);
    while (db.nextRow()) {
        if (row > ui->tbl->rowCount() - 1) {
            ui->tbl->addEmptyRow();
        }
        ui->tbl->setString(row, col, db.getString("f_groupname"));
        ui->tbl->setDouble(row, col + 1, db.getDouble("f_qty"));
        ui->tbl->setString(row, col + 2, float_str(db.getDouble("f_total"), 1));
        ui->leTotal->setDouble(ui->leTotal->getDouble() + db.getDouble("f_total"));
//        col += 4;
//        if (col > 11) {
//            col = 0;
//            row++;
//        }
        row++;
    }
    ui->tbl->addEmptyRow();
    ui->tbl->setString(row, col + 2, float_str(ui->leTotal->getDouble(), 1));
    row++;

    db[":f_hall"] = __c5config.defaultHall();
    db[":f_start"] = ui->deStart->date();
    db[":f_end"] = ui->deEnd->date();
    db[":f_state"] = ORDER_STATE_CLOSE;
    db.exec("select sum(oh.f_amounttotal) as f_total "
            "from o_header oh "
            "inner join o_tax ot on ot.f_id=oh.f_id "
            "where ot.f_receiptnumber>0 and oh.f_datecash between :f_start and :f_end and oh.f_state=:f_state " + userCond() +
            "and oh.f_hall=:f_hall ");
    db.nextRow();
    ui->tbl->addEmptyRow();
    ui->tbl->setString(row, 0, tr("Tax"));
    ui->tbl->setString(row, col + 2, float_str(db.getDouble("f_total"), 1));
    row++;

    ui->leRetail->setDouble(0);
    ui->leWhosale->setDouble(0);
}

QString Sales::userCond() const
{
    if (!ui->leLogin->isEmpty()) {
        return " and u.f_login='" + ui->leLogin->text() + "' ";
    }
    return "";
}

void Sales::printpreview()
{
    QString reportAdditionalTitle = tr("Sales by group");
    QString fLabel = dbhall->name(__c5config.defaultHall()) + "," + dbstore->name(__c5config.defaultStore());
    QFont font(font());
    font.setPointSize(20);
    C5Printing p;
    QSize paperSize(2000, 2800);
    p.setFont(font);
    int page = p.currentPageIndex();
    int startFrom = 0;
    bool stopped = false;
    int columnsWidth = 0;
    qreal scaleFactor = 0.40;
    qreal rowScaleFactor = 0.79;
    for (int i = 0; i < ui->tbl->columnCount(); i++) {
        columnsWidth += ui->tbl->columnWidth(i);
    }
    columnsWidth /= scaleFactor;
    if (columnsWidth > 2000) {
        p.setSceneParams(paperSize.height(), paperSize.width(), QPrinter::Landscape);
    } else {
        p.setSceneParams(paperSize.width(), paperSize.height(), QPrinter::Portrait);
    }
    do {
        p.setFontBold(true);
        p.ltext(QString("%1 %2")
                .arg(fLabel)
                .arg(reportAdditionalTitle), 0);
        p.br();
        p.setFontBold(false);
        p.line(0, p.fTop, columnsWidth, p.fTop);
        for (int c = 0; c < ui->tbl->columnCount(); c++) {
            if (ui->tbl->columnWidth(c) == 0) {
                continue;
            }
            if (c > 0) {
                p.ltext(ui->tbl->horizontalHeaderItem(c)->text(), (sumOfColumnsWidghtBefore(c) / scaleFactor) + 1);
                p.line(sumOfColumnsWidghtBefore(c) / scaleFactor, p.fTop, sumOfColumnsWidghtBefore(c) / scaleFactor, p.fTop + (ui->tbl->verticalHeader()->defaultSectionSize() / rowScaleFactor));
            } else {
                p.ltext(ui->tbl->horizontalHeaderItem(c)->text(), 1);
                p.line(0, p.fTop, 0, p.fTop + (ui->tbl->verticalHeader()->defaultSectionSize() / rowScaleFactor));
            }
        }
        p.line(columnsWidth, p.fTop, columnsWidth, p.fTop + (ui->tbl->verticalHeader()->defaultSectionSize() / rowScaleFactor));
        p.br();
        p.line(0, p.fTop, columnsWidth, p.fTop);
    for (int r = startFrom; r < ui->tbl->rowCount(); r++) {
        p.line(0, p.fTop, columnsWidth, p.fTop);
        for (int c = 0; c < ui->tbl->columnCount(); c++) {
            if (ui->tbl->columnWidth(c) == 0) {
                continue;
            }
            int s = ui->tbl->columnSpan(r, c) - 1;
            if (c > 0) {
                p.ltext(ui->tbl->getString(r, c), (sumOfColumnsWidghtBefore(c) / scaleFactor) + 1);
                p.line(sumOfColumnsWidghtBefore(c + s) / scaleFactor, p.fTop, sumOfColumnsWidghtBefore(c + s) / scaleFactor, p.fTop + (ui->tbl->rowHeight(r) / rowScaleFactor));
            } else {
                p.ltext(ui->tbl->getString(r, c), 1);
                p.line(0, p.fTop, 0, p.fTop + (ui->tbl->rowHeight(r) / rowScaleFactor));
            }
            c += s;
        }
        //last vertical line
//        p.line(columnsWidth, p.fTop, columnsWidth, p.fTop + (ui->tbl->rowHeight(r) / rowScaleFactor));
//        if (ui->tblTotal->isVisible() && r == ui->tbl->rowCount() - 1) {
//            p.setFontBold(true);
//            if (p.checkBr(ui->tblTotal->rowHeight(0))) {
//                p.line(0, p.fTop + (ui->tblTotal->rowHeight(0) / rowScaleFactor), columnsWidth, p.fTop + (ui->tblTotal->rowHeight(0) / rowScaleFactor));\
//                p.br();
//                stopped = startFrom >= fModel->rowCount() - 1;
//                p.fTop = p.fNormalHeight - p.fLineHeight;
//                p.ltext(QString("%1 %2, %3 %4, %5/%6")
//                        .arg("Page")
//                        .arg(page + 1)
//                        .arg(tr("Printed"))
//                        .arg(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR2))
//                        .arg(hostinfo)
//                        .arg(hostusername()), 0);
//                p.rtext(fDBParams.at(1));
//                page++;
//            } else {
//                p.br();
//            }
//            p.line(0, p.fTop, columnsWidth, p.fTop);
//            p.line(0, p.fTop + (ui->tblTotal->rowHeight(0) / rowScaleFactor), columnsWidth, p.fTop + (ui->tblTotal->rowHeight(0) / rowScaleFactor));\
//            for (int c = 0; c < ui->tbl->columnCount(); c++) {
//                if (ui->tbl->columnWidth(c) == 0) {
//                    continue;
//                }
//                if (c > 0) {
//                    p.ltext(ui->tblTotal->getString(0, c), (sumOfColumnsWidghtBefore(c) / scaleFactor) + 1);
//                    p.line(sumOfColumnsWidghtBefore(c) / scaleFactor, p.fTop, sumOfColumnsWidghtBefore(c) / scaleFactor, p.fTop + (fTableView->verticalHeader()->defaultSectionSize() / rowScaleFactor));
//                } else {
//                    p.ltext(ui->tblTotal->getString(0, c), 1);
//                    p.line(0, p.fTop, 0, p.fTop + (ui->tbl->verticalHeader()->defaultSectionSize() / rowScaleFactor));
//                }
//            }
//            p.line(columnsWidth, p.fTop, columnsWidth, p.fTop + (ui->tblTotal->rowHeight(0) / rowScaleFactor));
//        }
        if (p.checkBr(p.fLineHeight * 4) || r >= ui->tbl->rowCount() - 1) {
            p.line(0, p.fTop + (ui->tbl->rowHeight(r) / rowScaleFactor), columnsWidth, p.fTop + (ui->tbl->rowHeight(r) / rowScaleFactor));\
            p.br();
            startFrom = r + 1;
            stopped = startFrom >= ui->tbl->rowCount() - 1;
            p.fTop = p.fNormalHeight - p.fLineHeight;
            p.ltext(QString("%1 %2, %3 %4, %5/%6")
                    .arg("Page")
                    .arg(page + 1)
                    .arg(tr("Printed"))
                    .arg(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR2))
                    .arg(hostinfo)
                    .arg(hostusername()), 0);
            p.rtext(fDBParams.at(1));
            if (r < ui->tbl->rowCount() - 1) {
                p.br(p.fLineHeight * 4);
            }
            if (r < ui->tbl->rowCount() - 1) {
                page++;
            }
            break;
        } else {
            p.br();
        }
    }
        if (ui->tbl->rowCount() == 0) {
            stopped = true;
        }
    } while (!stopped);

    C5PrintPreview pp(&p, fDBParams);
    pp.exec();
}

void Sales::printTaxReport(int report_type)
{
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(), C5Config::taxCashier(), C5Config::taxPin(), this);
    QString jsnin, jsnout, err;
    int result;
    result = pt.printReport(QDateTime(ui->deStart->date()),
                            QDateTime(ui->deEnd->date()),
                            report_type, jsnin, jsnout, err);
    C5Database db(C5Config::dbParams());
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
    for (int i = 0; i < column; i++) {
        sum += ui->tbl->columnWidth(i);
    }
    return sum;
}

void Sales::on_btnDateRight_clicked()
{
    changeDate(1);
}

void Sales::on_btnPrint_clicked()
{
    C5Database db(C5Config::dbParams());
    QModelIndexList ml = ui->tbl->selectionModel()->selectedRows();
    if (ml.count() == 0) {
        return;
    }
    if (!C5Config::localReceiptPrinter().isEmpty()) {
        PrintReceiptGroup p;
        switch (C5Config::shopPrintVersion()) {
        case 1: {
            bool p1, p2;
            if (SelectPrinters::selectPrinters(p1, p2)) {
                if (p1) {
                    p.print(ui->tbl->getString(ml.at(0).row(), 1), db, 1);
                }
                if (p2) {
                    p.print(ui->tbl->getString(ml.at(0).row(), 1), db, 2);
                }
            }
            break;
        }
        case 2:
            p.print2(ui->tbl->getString(ml.at(0).row(), 1), db);
            break;
        default:
            break;
        }
    }
//    bool p1, p2;
//    if (SelectPrinters::selectPrinters(p1, p2)) {
//        PrintReceiptGroup p;
//        C5Database db(C5Config::dbParams());
//        if (p1) {
//            p.print(ui->tbl->getString(ml.at(0).row(), 0), db, 1);
//        }
//        if (p2) {
//            p.print(ui->tbl->getString(ml.at(0).row(), 0), db, 2);
//        }
//        //p.print2(oheaderid, db);
//    }
}

void Sales::on_btnItemBack_clicked()
{
    QModelIndexList l = ui->tbl->selectionModel()->selectedRows();
    if (l.count() == 0) {
        C5Message::error(tr("No row selected"));
        return;
    }
    if (ui->tbl->getDouble(l.at(0).row(), 4) < 0) {
        C5Message::error(tr("Return inpossible to return, just view"));
    }
    int col = 1;
    switch (fViewMode) {
    case VM_ITEMS:
        col = 0;
        break;
    default:
        break;
    }
    QString uuid = ui->tbl->getString(l.at(0).row(), col);
    ViewOrder *vo = new ViewOrder(uuid);
    vo->exec();
    vo->deleteLater();
}

void Sales::on_btnRefresh_clicked()
{
    refresh();
}

void Sales::on_btnModeTotal_clicked()
{
    ui->btnModeItems->setChecked(false);
    ui->btnModeTotal->setChecked(true);
    ui->btnTotalByItems->setChecked(false);
    ui->btnGroups->setChecked(false);
    ui->btnPrint->setEnabled(true);
    ui->btnItemBack->setEnabled(true);
    ui->btnPrintTax->setEnabled(true);
    fViewMode = VM_TOTAL;
    refresh();
}

void Sales::on_btnModeItems_clicked()
{
    ui->btnModeItems->setChecked(true);
    ui->btnModeTotal->setChecked(false);
    ui->btnTotalByItems->setChecked(false);
    ui->btnGroups->setChecked(false);
    ui->btnPrint->setEnabled(true);
    ui->btnItemBack->setEnabled(true);
    ui->btnPrintTax->setEnabled(false);
    fViewMode = VM_ITEMS;
    refresh();
}

void Sales::on_btnTotalByItems_clicked()
{
    ui->btnModeItems->setChecked(false);
    ui->btnModeTotal->setChecked(false);
    ui->btnTotalByItems->setChecked(true);
    ui->btnPrint->setEnabled(false);
    ui->btnItemBack->setEnabled(false);
    ui->btnPrintTax->setEnabled(false);
    fViewMode = VM_TOTAL_ITEMS;
    refresh();
}

void Sales::on_btnPrintTax_clicked()
{
    QModelIndexList ml = ui->tbl->selectionModel()->selectedRows();
    if (ml.count() == 0) {
        return;
    }
    if (C5Message::question(tr("Print tax") + "<br>" + float_str(ui->tbl->getDouble(ml.at(0).row(), 9), 2)) != QDialog::Accepted) {
        return;
    }
    QString id = ui->tbl->getString(ml.at(0).row(), 1);
    C5Database db(__c5config.replicaDbParams());
    db[":f_id"] = id;
    db.exec("select * from o_tax where f_id=:f_id");
    if (db.nextRow()) {
        if (db.getInt("f_receiptnumber") > 0) {
            C5Message::error(tr("Cannot print tax twice"));
            return;
        }
    }
    if (__c5config.taxIP().toLower() == "http") {
        QString url = QString("GET /printtax?auth=up&a=get&user=%1&pass=%2&order=%3 HTTP/1.1\r\n\r\n")
                .arg(__c5config.httpServerUsername())
                .arg(__c5config.httpServerPassword())
                .arg(id);
        auto *s = new QSslSocket(this);
        connect(s, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(err(QAbstractSocket::SocketError)));
        s->addCaCertificate(fSslCertificate);
        s->setPeerVerifyMode(QSslSocket::VerifyNone);
        s->connectToHostEncrypted(__c5config.httpServerIP(), __c5config.httpServerPort());
        if (s->waitForEncrypted(5000)) {
            s->write(url.toUtf8());
            if (s->waitForBytesWritten()) {
                s->waitForReadyRead();
                QByteArray d = s->readAll();
                C5Message::info(d);
            } else {
                C5Message::error(s->errorString());
            }
            s->close();
        }
        s->deleteLater();
        return;
    }
    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");
    db.nextRow();
    double disc = db.getDouble("f_discountfactor");
    double card = db.getDouble("f_amountcard");
    db[":f_id"] = id;
    db.exec("select og.f_id, og.f_goods, g.f_name, og.f_qty, gu.f_name as f_unitname, og.f_price, og.f_total,"
            "t.f_taxdept, t.f_adgcode, "
            "og.f_store "
            "from o_goods og "
            "left join c_goods g on g.f_id=og.f_goods "
            "left join c_units gu on gu.f_id=g.f_unit "
            "left join c_groups t on t.f_id=g.f_group "
            "where og.f_header=:f_id");
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(), C5Config::taxCashier(), C5Config::taxPin(), this);
    while (db.nextRow()) {
        pt.addGoods(db.getString("f_taxdept"), //dep
                    db.getString("f_adgcode"), //adg
                    db.getString("f_goods"), //goods id
                    db.getString("f_name"), //name
                    db.getDouble("f_price"), //price
                    db.getDouble("f_qty"), //qty
                    disc); //discount
    }
    QString jsonIn, jsonOut, err;
    QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
    int result = 0;
    result = pt.makeJsonAndPrint(card, 0, jsonIn, jsonOut, err);

    db[":f_id"] = db.uuid();
    db[":f_order"] = id;
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_in"] = jsonIn;
    db[":f_out"] = jsonOut;
    db[":f_err"] = err;
    db[":f_result"] = result;
    db.insert("o_tax_log", false);
    QSqlQuery *q = new QSqlQuery(db.fDb);
    if (result == pt_err_ok) {
        PrintTaxN::parseResponse(jsonOut, firm, hvhh, fiscal, rseq, sn, address, devnum, time);
        db[":f_id"] = id;
        db.exec("delete from o_tax where f_id=:f_id");
        db[":f_id"] = id;
        db[":f_dept"] = C5Config::taxDept();
        db[":f_firmname"] = firm;
        db[":f_address"] = address;
        db[":f_devnum"] = devnum;
        db[":f_serial"] = sn;
        db[":f_fiscal"] = fiscal;
        db[":f_receiptnumber"] = rseq;
        db[":f_hvhh"] = hvhh;
        db[":f_fiscalmode"] = tr("(F)");
        db[":f_time"] = time;
        db.insert("o_tax", false);
        pt.saveTimeResult(id, *q);
        db[":f_tax"] = rseq.toInt();
        db.update("o_goods", "f_header", id);
        delete q;
        ui->tbl->setString(ml.at(0).row(), 5, rseq);
        C5Message::info(tr("Printed"));
    } else {
        pt.saveTimeResult("Not saved - " + id, *q);
        delete q;
        C5Message::error(err + "<br>" + jsonOut + "<br>" + jsonIn);
    }
}

void Sales::on_btnRetryUpload_clicked()
{
    C5Replication r;
    if (r.uploadToServer()) {
        ui->btnRetryUpload->setVisible(false);
    } else {
        C5Message::error(tr("Cannot upload data"));
    }
}

void Sales::on_btnCashColletion_clicked()
{
    auto *cc = new CashCollection();
    cc->exec();
    delete cc;
}

void Sales::on_leFilter_textChanged(const QString &arg1)
{
    for (int r = 0; r < ui->tbl->rowCount(); r++) {
        bool h = true;
        for (int c = 0; c < ui->tbl->columnCount(); c++) {
            if (ui->tbl->getString(r, c).contains(arg1, Qt::CaseInsensitive)) {
                h = false;
                break;
            }
        }
        ui->tbl->setRowHidden(r, h);
    }
}

void Sales::on_btnGroups_clicked()
{
    ui->btnModeItems->setChecked(false);
    ui->btnModeTotal->setChecked(false);
    ui->btnTotalByItems->setChecked(false);
    ui->btnPrint->setEnabled(false);
    ui->btnItemBack->setEnabled(false);
    ui->btnPrintTax->setEnabled(false);
    fViewMode = VM_GROUPS;
    refresh();
    printpreview();
}

void Sales::on_btnChangeDate_clicked()
{
    if (fViewMode != VM_TOTAL) {
        return;
    }
    QDate d;
    C5Database db(__c5config.replicaDbParams());
    for (int  i = 0; i < ui->tbl->rowCount(); i++) {
        if (ui->tbl->item(i, 0)->checkState() == Qt::Checked) {
            if (!d.isValid()) {
                if (!DlgDate::getDate(d)) {
                    return;
                }
            }
            db[":f_datecash"] = d;
            db[":f_id"] = ui->tbl->getString(i, 1);
            if (!db.exec("update o_header set f_datecash=:f_datecash where f_id=:f_id")) {
                C5Message::error(db.fLastError);
                return;
            }
        }
    }
    if (d.isValid()) {
        refresh();
    } else {
        C5Message::info(tr("Nothing was selected"));
    }
}

void Sales::on_btnPrintTaxZ_clicked()
{
    printTaxReport(report_z);
}

void Sales::on_btnPrintTaxX_clicked()
{
    printTaxReport(report_x);
}
