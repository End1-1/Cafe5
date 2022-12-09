#include "dlgmemoryread.h"
#include "ui_dlgmemoryread.h"
#include "workspace.h"

DlgMemoryRead::DlgMemoryRead() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgMemoryRead)
{
    ui->setupUi(this);
    fResult = 0;
    ui->tblMemory->setColumnWidth(0, 100);
    ui->tblMemory->setColumnWidth(1, 150);
    ui->btnReprint->setVisible(false);
}

DlgMemoryRead::~DlgMemoryRead()
{
    delete ui;
}

int DlgMemoryRead::getWindow()
{
    DlgMemoryRead rm;
    rm.getIncompleteOrders();
    return rm.fResult;
}

int DlgMemoryRead::sessionHistory()
{
    DlgMemoryRead rm;
    rm.getSessionHistory();
    return rm.fResult;
}

void DlgMemoryRead::on_btnCancel_clicked()
{
    reject();
}

void DlgMemoryRead::on_btnOpen_clicked()
{
    if (fResult > 0) {
        accept();
    } else {
        C5Message::error(tr("Nothing was selected"));
    }
}

void DlgMemoryRead::on_tblMemory_cellClicked(int row, int column)
{
    if (row >  -1) {
        fResult = ui->tblMemory->item(row, 0)->text().toInt();
    }
}

int DlgMemoryRead::getSessionHistory()
{
    ui->btnReprint->setVisible(true);
    ui->btnOpen->setVisible(false);
    ui->tblMemory->setColumnCount(4);
    ui->tblMemory->setColumnWidth(0, 0);
    ui->tblMemory->setColumnWidth(1, 150);
    ui->tblMemory->setColumnWidth(2, 150);
    ui->tblMemory->setColumnWidth(3, 150);
    QStringList header;
    header.append(tr("Id"));
    header.append(tr("Number"));
    header.append(tr("Date"));
    header.append(tr("Amount"));
    ui->tblMemory->setHorizontalHeaderLabels(header);
    QString sql ("SELECT h.f_id, concat(h.f_prefix, h.f_hallid) as f_hallid, "
                    "date_format(cast(concat(f_dateclose, ' ', f_timeclose) as datetime),'%d/%c/%Y %H:%i' ) as f_date, h.f_amounttotal "
                    "from o_header h "
                    "INNER JOIN a_header_cash hc ON hc.f_oheader=h.f_id and hc.f_session=:f_session "
                    "order by f_dateclose desc, f_timeclose desc ");
    C5Database db(fDBParams);
    db[":f_session"] = __c5config.getRegValue("session");
    db.exec(sql);
    while (db.nextRow()) {
        int r = ui->tblMemory->rowCount();
        ui->tblMemory->setRowCount(r + 1);
        ui->tblMemory->setItem(r, 0, new QTableWidgetItem(db.getString("f_id")));
        ui->tblMemory->setItem(r, 1, new QTableWidgetItem(db.getString("f_hallid")));
        ui->tblMemory->setItem(r, 2, new QTableWidgetItem(db.getString("f_date")));
        ui->tblMemory->setItem(r, 3, new QTableWidgetItem(float_str(db.getDouble("f_amounttotal"), 2)));
    }
    exec();
    return 0;
}

int DlgMemoryRead::getIncompleteOrders()
{
    C5Database db(fDBParams);
    db.exec("select distinct(f_window) as f_window, sum(f_qty*f_price) as f_total from a_sale_temp group by 1");
    while (db.nextRow()) {
        int r = ui->tblMemory->rowCount();
        ui->tblMemory->setRowCount(r + 1);
        ui->tblMemory->setItem(r, 0, new QTableWidgetItem(db.getString("f_window")));
        ui->tblMemory->setItem(r, 1, new QTableWidgetItem(float_str(db.getDouble("f_total"), 2)));
    }
    return exec();
}


void DlgMemoryRead::on_btnReprint_clicked()
{
    int row = ui->tblMemory->currentRow();
    if (row < 0) {
        return;
    }
    Workspace::fWorkspace->printReceipt(ui->tblMemory->item(row, 0)->text(), false, false);
    accept();
}
