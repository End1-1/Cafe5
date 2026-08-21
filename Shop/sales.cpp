#include "sales.h"
#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QPageSize>
#include <QPrinter>
#include <QPrinterInfo>
#include <QPropertyAnimation>
#include "c5message.h"
#include "c5permissions.h"
#include "c5printing.h"
#include "c5user.h"
#include "c5utils.h"
#include "dlgdate.h"
#include "dlgfindorder.h"
#include "format_date.h"
#include "httplite.h"
#include "ninterface.h"
#include "printreceiptgroup.h"
#include "printtaxn.h"
#include "selectprinters.h"
#include "struct_workstationitem.h"
#include "ui_sales.h"
#include "vieworder.h"

#define VM_TOTAL 0
#define VM_ITEMS 1
#define VM_STAFF 2

Sales::Sales(C5User *user) :
    C5ShopDialog(user),
    ui(new Ui::Sales)
{
    ui->setupUi(this);
    fUser = user;
    //TODO ui->btnChangeDate->setVisible(fUser->check(cp_t12_change_date_of_sale));
    ui->btnModeItems->setVisible(fUser->check(cp_t12_shop_report_goods));
    fViewMode = VM_TOTAL;
    ui->lbTotalQty->setVisible(false);
    ui->lbTotalAmount->setVisible(ui->leTotal->isVisible());
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
    s->setWindowModality(Qt::WindowModal);
    s->showMaximized();
    s->setFocus();
}

bool Sales::printReceipt(const QString &id, C5User *user)
{
    PrintReceiptGroup::print2(id, user, user);
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

    case VM_STAFF:
        refreshByStaff();
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
    h.append(tr("Prefix"));
    h.append(tr("##"));
    h.append(tr("Date"));
    h.append(tr("Amount"));
    h.append(tr("Customer"));
    h.append(tr("Deliverman"));
    h.append(tr("Comment"));
    ui->tbl->setColumnCount(h.count());
    ui->tbl->setHorizontalHeaderLabels(h);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 40, 0, 0, 0, 120, 100, 100, 160, 150, 100, 100, 300);
    ui->tbl->setRowCount(0);
    ui->leTotal->setDouble(0);
    ui->leTotalQty->setDouble(0);

    NInterface::query1(QStringLiteral("/engine/v2/shop/sales/get-orders"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("hall"), mWorkStation.defaultHallId()},
                        {QStringLiteral("date1"), ui->deStart->date().toString(FORMAT_DATE_TO_STR_MYSQL)},
                        {QStringLiteral("date2"), ui->deEnd->date().toString(FORMAT_DATE_TO_STR_MYSQL)}},
                       [this](const QJsonObject &jo) {
                           const QJsonArray rows = jo.value(QStringLiteral("rows")).toArray();
                           ui->tbl->setRowCount(rows.size());
                           int row = 0;
                           for(const QJsonValue &jv : rows) {
                               const QJsonObject o = jv.toObject();
                               const QString date = o.value(QStringLiteral("f_datecash")).toString();
                               const QString time = o.value(QStringLiteral("f_timeclose")).toString().trimmed();
                               const QString dateTime = time.isEmpty() ? date : QStringLiteral("%1 %2").arg(date, time);
                               ui->tbl->createCheckbox(row, 0);
                               ui->tbl->setData(row, 1, o.value(QStringLiteral("f_id")).toString());
                               ui->tbl->setInteger(row, 2, o.value(QStringLiteral("f_saletype")).toInt());
                               ui->tbl->setData(row, 3, o.value(QStringLiteral("f_login")).toString());
                               ui->tbl->setData(row, 4, o.value(QStringLiteral("f_saletype_name")).toString());
                               ui->tbl->setData(row, 5, o.value(QStringLiteral("f_prefix")).toString());
                               ui->tbl->setData(row, 6, o.value(QStringLiteral("f_fiscal")).toString());
                               ui->tbl->setData(row, 7, dateTime);
                               ui->tbl->setDouble(row, 8, o.value(QStringLiteral("f_amounttotal")).toDouble());
                               ui->tbl->setData(row, 9, o.value(QStringLiteral("f_client")).toString());
                               ui->tbl->setData(row, 10, o.value(QStringLiteral("f_deliverman")).toString());
                               ui->tbl->setData(row, 11, o.value(QStringLiteral("f_comment")).toString());
                               ++row;
                           }
                           ui->leTotal->setDouble(jo.value(QStringLiteral("total")).toDouble());
                       });
}

void Sales::refreshItems()
{
    QStringList h;
    h.append(tr("UUID"));
    h.append(tr("Sale type code"));
    h.append(tr("Seller"));
    h.append(tr("Sale type"));
    h.append(tr("Prefix"));
    h.append(tr("##"));
    h.append(tr("Date"));
    h.append(tr("Scancode"));
    h.append(tr("Goods"));
    h.append(tr("Qty"));
    h.append(tr("Price"));
    h.append(tr("Total"));
    h.append(tr("Comment"));
    ui->tbl->setColumnCount(h.count());
    ui->tbl->setHorizontalHeaderLabels(h);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 0, 0, 120, 100, 100, 160, 150, 250, 80, 80, 80, 200);
    ui->tbl->setRowCount(0);
    ui->leTotal->setDouble(0);
    ui->leTotalQty->setDouble(0);

    NInterface::query1(QStringLiteral("/engine/v2/shop/sales/get-items"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("hall"), mWorkStation.defaultHallId()},
                        {QStringLiteral("date1"), ui->deStart->date().toString(FORMAT_DATE_TO_STR_MYSQL)},
                        {QStringLiteral("date2"), ui->deEnd->date().toString(FORMAT_DATE_TO_STR_MYSQL)}},
                       [this](const QJsonObject &jo) {
                           const QJsonArray rows = jo.value(QStringLiteral("rows")).toArray();
                           ui->tbl->setRowCount(rows.size());
                           int row = 0;
                           for(const QJsonValue &jv : rows) {
                               const QJsonObject o = jv.toObject();
                               const QString date = o.value(QStringLiteral("f_datecash")).toString();
                               const QString time = o.value(QStringLiteral("f_timeclose")).toString().trimmed();
                               const QString dateTime = time.isEmpty() ? date : QStringLiteral("%1 %2").arg(date, time);
                               ui->tbl->setData(row, 0, o.value(QStringLiteral("f_id")).toString());
                               ui->tbl->setInteger(row, 1, o.value(QStringLiteral("f_saletype")).toInt());
                               ui->tbl->setData(row, 2, o.value(QStringLiteral("f_login")).toString());
                               ui->tbl->setData(row, 3, o.value(QStringLiteral("f_saletype_name")).toString());
                               ui->tbl->setData(row, 4, o.value(QStringLiteral("f_prefix")).toString());
                               ui->tbl->setData(row, 5, o.value(QStringLiteral("f_fiscal")).toString());
                               ui->tbl->setData(row, 6, dateTime);
                               ui->tbl->setData(row, 7, o.value(QStringLiteral("f_scancode")).toString());
                               ui->tbl->setData(row, 8, o.value(QStringLiteral("f_goodsname")).toString());
                               ui->tbl->setDouble(row, 9, o.value(QStringLiteral("f_qty")).toDouble());
                               ui->tbl->setDouble(row, 10, o.value(QStringLiteral("f_price")).toDouble());
                               ui->tbl->setDouble(row, 11, o.value(QStringLiteral("f_total")).toDouble());
                               ui->tbl->setData(row, 12, o.value(QStringLiteral("f_comment")).toString());
                               ++row;
                           }
                           ui->leTotal->setDouble(jo.value(QStringLiteral("total")).toDouble());
                           ui->leTotalQty->setDouble(jo.value(QStringLiteral("total_qty")).toDouble());
                       });
}

void Sales::refreshByStaff()
{
    QStringList h;
    h.append(tr("Code"));
    h.append(tr("Sales assistant"));
    h.append(tr("Login"));
    h.append(tr("Orders"));
    h.append(tr("Cash"));
    h.append(tr("Card"));
    h.append(tr("Idram"));
    h.append(tr("Telcell"));
    h.append(tr("Bank"));
    h.append(tr("Debt"));
    h.append(tr("Prepaid"));
    h.append(tr("Total"));
    ui->tbl->setColumnCount(h.count());
    ui->tbl->setHorizontalHeaderLabels(h);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 220, 120, 80, 100, 100, 100, 100, 100, 100, 100, 120);
    ui->tbl->setRowCount(0);
    ui->leTotal->setDouble(0);
    ui->leTotalQty->setDouble(0);
    ui->leTotalQty->setVisible(true);
    ui->lbTotalQty->setVisible(true);

    NInterface::query1(QStringLiteral("/engine/v2/shop/sales/get-by-staff"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("hall"), mWorkStation.defaultHallId()},
                        {QStringLiteral("date1"), ui->deStart->date().toString(FORMAT_DATE_TO_STR_MYSQL)},
                        {QStringLiteral("date2"), ui->deEnd->date().toString(FORMAT_DATE_TO_STR_MYSQL)}},
                       [this](const QJsonObject &jo) {
                           const QJsonArray rows = jo.value(QStringLiteral("rows")).toArray();
                           ui->tbl->setRowCount(rows.size());
                           int row = 0;
                           for(const QJsonValue &jv : rows) {
                               const QJsonObject o = jv.toObject();
                               ui->tbl->setInteger(row, 0, o.value(QStringLiteral("f_staff")).toInt());
                               ui->tbl->setData(row, 1, o.value(QStringLiteral("f_staff_name")).toString());
                               ui->tbl->setData(row, 2, o.value(QStringLiteral("f_login")).toString());
                               ui->tbl->setInteger(row, 3, o.value(QStringLiteral("f_count")).toInt());
                               ui->tbl->setDouble(row, 4, o.value(QStringLiteral("f_amount_cash")).toDouble());
                               ui->tbl->setDouble(row, 5, o.value(QStringLiteral("f_amount_card")).toDouble());
                               ui->tbl->setDouble(row, 6, o.value(QStringLiteral("f_amount_idram")).toDouble());
                               ui->tbl->setDouble(row, 7, o.value(QStringLiteral("f_amount_telcell")).toDouble());
                               ui->tbl->setDouble(row, 8, o.value(QStringLiteral("f_amount_bank")).toDouble());
                               ui->tbl->setDouble(row, 9, o.value(QStringLiteral("f_amount_debt")).toDouble());
                               ui->tbl->setDouble(row, 10, o.value(QStringLiteral("f_amount_prepaid")).toDouble());
                               ui->tbl->setDouble(row, 11, o.value(QStringLiteral("f_amounttotal")).toDouble());
                               ++row;
                           }
                           ui->leTotal->setDouble(jo.value(QStringLiteral("total")).toDouble());
                           ui->leTotalQty->setDouble(jo.value(QStringLiteral("total_count")).toDouble());
                       });
}

void Sales::printTaxReport(int report_type)
{
    FiscalMachine fm = getFiscalMachine(mWorkStation.fiscalMachineId());
    PrintTaxN pt(fm.ip, fm.port, fm.machinePassword, fm.externalPosString(), fm.opPin, fm.opPassword, this);
    QString jsnin, jsnout, err;
    int result;
    result = pt.printReport(ui->deStart->date(), ui->deEnd->date(),
                            report_type, jsnin, jsnout, err);
    //TODO
    //  db;
    // db[":f_id"] = db.uuid();
    // db[":f_order"] = QString("Report %1").arg(report_type == report_x ? "X" : "Z");
    // db[":f_date"] = QDate::currentDate();
    // db[":f_time"] = QTime::currentTime();
    // db[":f_in"] = jsnin;
    // db[":f_out"] = jsnout;
    // db[":f_err"] = err;
    // db[":f_result"] = result;
    // db.insert("o_tax_log", false);
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
    DlgFindOrder dlg(mUser, this);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }
    const QString orderId = dlg.orderId();
    if (orderId.isEmpty()) {
        return;
    }
    auto *a = new ViewOrder(fWorking, orderId, mUser);
    a->showMaximized();
    a->setFocus();
    a->exec();
    a->deleteLater();
}

void Sales::on_btnRefresh_clicked()
{
    refresh();
}

void Sales::on_btnModeTotal_clicked()
{
    toggleMenu(false);
    ui->btnModeItems->setChecked(false);
    ui->btnSalesAssistant->setChecked(false);
    ui->btnModeTotal->setChecked(true);
    ui->btnViewOrder->setEnabled(true);
    ui->btnItemBack->setEnabled(true);
    ui->leTotalQty->setVisible(false);
    ui->lbTotalQty->setVisible(false);
    fViewMode = VM_TOTAL;
    refresh();
}

void Sales::on_btnModeItems_clicked()
{
    toggleMenu(false);
    ui->btnModeItems->setChecked(true);
    ui->btnModeTotal->setChecked(false);
    ui->btnSalesAssistant->setChecked(false);
    ui->btnViewOrder->setEnabled(true);
    ui->btnItemBack->setEnabled(true);
    ui->leTotalQty->setVisible(true);
    ui->lbTotalQty->setVisible(true);
    fViewMode = VM_ITEMS;
    refresh();
}

void Sales::on_btnSalesAssistant_clicked()
{
    toggleMenu(false);
    ui->btnSalesAssistant->setChecked(true);
    ui->btnModeTotal->setChecked(false);
    ui->btnModeItems->setChecked(false);
    ui->btnViewOrder->setEnabled(false);
    ui->btnItemBack->setEnabled(false);
    fViewMode = VM_STAFF;
    refresh();
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
    auto *a = new ViewOrder(fWorking, ui->tbl->getString(ml.at(0).row(), col), mUser);
    a->showMaximized();
    a->setFocus();
    a->exec();
    a->deleteLater();
}

void Sales::on_btnShowMenu_clicked()
{
    toggleMenu(true);
}

void Sales::on_btnCloseMenu_clicked()
{
    toggleMenu(false);
}

void Sales::on_btnPrintTotal_clicked()
{
    toggleMenu(false);
    if (!mWorkStation.isReceiptPrintingConfigured()) {
        C5Message::error(tr("Receipt printer is not configured"));
        return;
    }

    const QString date1 = ui->deStart->date().toString(FORMAT_DATE_TO_STR_MYSQL);
    const QString date2 = ui->deEnd->date().toString(FORMAT_DATE_TO_STR_MYSQL);

    NInterface::query1(QStringLiteral("/engine/v2/shop/sales/get-daily-by-payment"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("hall"), mWorkStation.defaultHallId()},
                        {QStringLiteral("date1"), date1},
                        {QStringLiteral("date2"), date2}},
                       [this](const QJsonObject &jdoc) {
                           printDailyByPayment(jdoc);
                       });
}

void Sales::printDailyByPayment(const QJsonObject &jdoc)
{
    const QString printerName = mWorkStation.receiptPrinter();
    QPrinterInfo pi;
    if (mWorkStation.hasReceiptPrinter()) {
        pi = QPrinterInfo::printerInfo(printerName);
    }

    const int bs = 20;
    QFont font(qApp->font());
    font.setPointSize(bs);
    C5Printing p;
    QPrinter printer(pi.isNull() ? QPrinterInfo() : pi);
    if (!pi.isNull()) {
        printer.setPageSize(QPageSize::Custom);
        printer.setFullPage(false);
        QRectF pr = printer.pageRect(QPrinter::DevicePixel);
        constexpr qreal SAFE_RIGHT_MM = 4.0;
        const qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
        p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
    } else {
        p.setSceneParams(650, 2800, 96);
    }
    p.setFont(font);
    p.setFontSize(bs);
    p.setFontBold(true);

    const QString logoFile = qApp->applicationDirPath() + QStringLiteral("/logo_receipt.png");
    if (QFile::exists(logoFile)) {
        p.image(logoFile, Qt::AlignHCenter);
        p.br();
    }

    p.ctext(tr("Daily revenue"));
    p.br();
    p.ctext(ui->deStart->date().toString(FORMAT_DATE_TO_STR));
    p.br();
    if (ui->deStart->date() != ui->deEnd->date()) {
        p.ctext(ui->deEnd->date().toString(FORMAT_DATE_TO_STR));
        p.br();
    }
    p.setFontBold(false);
    p.br();

    p.ltext(tr("Orders count"), 0);
    p.rtext(QString::number(jdoc.value(QStringLiteral("f_count_id")).toInt()));
    p.br();
    p.setFontBold(true);
    p.ltext(tr("Total"), 0);
    p.rtext(float_str(jdoc.value(QStringLiteral("f_amount_total")).toDouble(), 2));
    p.br();
    p.setFontBold(false);
    p.line();
    p.br();

    auto printPay = [&p](const QString &title, double amount) {
        if (qAbs(amount) > 0.009) {
            p.ltext(title, 0);
            p.rtext(float_str(amount, 2));
            p.br();
        }
    };
    printPay(tr("Cash"), jdoc.value(QStringLiteral("f_amount_cash")).toDouble());
    printPay(tr("Card"), jdoc.value(QStringLiteral("f_amount_card")).toDouble());
    printPay(tr("Idram"), jdoc.value(QStringLiteral("f_amount_idram")).toDouble());
    printPay(tr("Telcell"), jdoc.value(QStringLiteral("f_amount_telcell")).toDouble());
    printPay(tr("Bank"), jdoc.value(QStringLiteral("f_amount_bank")).toDouble());
    printPay(tr("Debt"), jdoc.value(QStringLiteral("f_amount_debt")).toDouble());
    printPay(tr("Prepaid"), jdoc.value(QStringLiteral("f_amount_prepaid")).toDouble());
    printPay(tr("Complimentary"), jdoc.value(QStringLiteral("f_amount_complimentary")).toDouble());
    printPay(tr("Other"), jdoc.value(QStringLiteral("f_amount_other")).toDouble());

    p.line();
    p.br();
    p.br();
    p.setFontSize(bs - 4);
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();

    if (mWorkStation.usePrintServer()) {
        auto *http = new HttpLite(qApp);
        QJsonObject json;
        json.insert(QStringLiteral("print_data"), p.jsonData());
        json.insert(QStringLiteral("printer_name"), printerName);
        http->post(mWorkStation.printServer(), json);
    }
    if (mWorkStation.hasReceiptPrinter()) {
        if (!pi.isNull()) {
            p.print(printer);
        } else {
            C5Message::error(tr("Printer not found") + ": " + printerName);
        }
    }
}
