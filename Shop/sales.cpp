#include "sales.h"
#include <QInputDialog>
#include <QPropertyAnimation>
#include <QShortcut>
#include "c5message.h"
#include "c5permissions.h"
#include "c5user.h"
#include "c5utils.h"
#include "dlgdate.h"
#include "printreceiptgroup.h"
#include "printtaxn.h"
#include "selectprinters.h"
#include "struct_workstationitem.h"
#include "ui_sales.h"
#include "vieworder.h"

#define VM_TOTAL 0
#define VM_ITEMS 1

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
    showAll = mWorkStation.shopShowAll();
    refresh();
    auto *sh = new QShortcut(QKeySequence(Qt::Key_F3), this);
    sh->setContext(Qt::WindowShortcut);
    connect(sh, &QShortcut::activated, this, [this]() {
        C5User *tmp = new C5User(mUser);

        QString password = QInputDialog::getText(this,
                                                 tr("Password"),
                                                 tr("Password"),
                                                 QLineEdit::Password);
        tmp->authorize(
            password,
            fHttp,
            [this, tmp](const QJsonObject &jo) {
                Q_UNUSED(jo)

                if (tmp->check(cp_t12_shop_enter_sale)) {
                    if (tmp->fConfig["copyfrom"].toInt() != 0) {
                        tmp->copySettings(mUser);
                    }

                    showAll = true;
                    refresh();
                } else {
                    tmp->deleteLater();
                }
            },
            [tmp]() { tmp->deleteLater(); });
    });
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
    if (!mWorkStation.defaultPrinter().isEmpty()) {
        PrintReceiptGroup p;
            p.print2(id);
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
    }
}

void Sales::refreshTotal()
{
    //TODO
}

void Sales::refreshItems()
{
    //TODO
    // QStringList h;
    // h.append(tr("UUID"));
    // h.append(tr("Sale type code"));
    // h.append(tr("Seller"));
    // h.append(tr("Sale type"));
    // h.append(tr("Number"));
    // h.append(tr("##"));
    // h.append(tr("Date"));
    // h.append(tr("Time"));
    // h.append(tr("Scancode"));
    // h.append(tr("Goods"));
    // h.append(tr("Qty"));
    // h.append(tr("Price"));
    // h.append(tr("Total"));
    // ui->tbl->setColumnCount(h.count());
    // ui->tbl->setHorizontalHeaderLabels(h);
    // ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 0, 0, 120, 0, 100, 120, 100, 150, 250, 80, 80, 80);
    //  db;
    // db[":f_hall"] = __c5config.defaultHall();
    // db[":f_start"] = ui->deStart->date();
    // db[":f_end"] = ui->deEnd->date();
    // db[":f_state"] = ORDER_STATE_CLOSE;
    // QString sqlCond = "";

    // if (!fUser->check(cp_t12_shop_fiscal_report) || !showAll) {
    //     sqlCond += " and length(json_value(oh.f_data, '$.f_fiscal.rseq'))>0 ";
    // }

    // if(!fUser->check(cp_t12_shop_sale_of_all_users)) {
    //     sqlCond += QString(" and oh.f_staff=%1 ").arg(fUser->id());
    // }

    // db.exec("select oh.f_id, oh.f_saletype, u.f_login, os.f_name, concat(oh.f_prefix, oh.f_hallid) as f_number, json_value(oh.f_data, "
    //         "'$.f_fiscal.rseq'),  "
    //         "oh.f_datecash, oh.f_timeclose, g.f_scancode, g.f_name as f_goodsname, og.f_qty, og.f_price, og.f_total "
    //         "from o_goods og "
    //         "inner join o_header oh on oh.f_id=og.f_header "
    //         "inner join c_goods g on g.f_id=og.f_goods "
    //         "left join b_history h on h.f_id=oh.f_id "
    //         "left join b_cards_discount d on d.f_id=h.f_card "
    //         "left join c_partners c on c.f_id=d.f_client "
    //         "left join o_sale_type os on os.f_id=oh.f_saletype "
    //         "left join s_user u on u.f_id=oh.f_staff "
    //         "where oh.f_datecash between :f_start and :f_end and oh.f_state=:f_state "
    //         + sqlCond
    //         + "and oh.f_hall=:f_hall "
    //           "order by oh.f_datecash, oh.f_timeclose ");
    // ui->tbl->setRowCount(db.rowCount());
    // int row = 0;

    // while(db.nextRow()) {
    //     for(int i = 0; i < ui->tbl->columnCount(); i++) {
    //         ui->tbl->setData(row, i, db.getValue(i));
    //     }

    //     row++;
    // }

    // int acol = 12;
    // ui->leTotal->setDouble(ui->tbl->sumOfColumn(acol));
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
    //TODO
    //todo: memory leak
    // DlgReturnItem *i = new DlgReturnItem(mUser);
    // i->showMaximized();
    // i->setFocus();
    // i->exec();
    // i->deleteLater();
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
    ui->btnViewOrder->setEnabled(true);
    ui->btnItemBack->setEnabled(true);
    fViewMode = VM_ITEMS;
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
