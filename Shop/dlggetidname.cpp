#include "dlggetidname.h"
#include "c5database.h"
#include "ui_dlggetidname.h"

QHash<int, QString> DlgGetIDName::fQueries;
QHash<int, DlgGetIDName*> DlgGetIDName::fDialogs;

DlgGetIDName::DlgGetIDName(C5User *user) :
    C5Dialog(user),
    ui(new Ui::DlgGetIDName)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidth(0, 100);
    ui->tbl->setColumnWidth(1, 400);

    if(fQueries.isEmpty()) {
        fQueries[idname_users_fullname] = "select f_id, concat_ws(' ', f_last, f_first) from s_user order by 2";
        fQueries[idname_partners_full] =
            "select f_id, concat_ws(' ', f_taxcode, f_taxname, f_contact, f_phone) from c_partners order by 2";
    }
}

DlgGetIDName::~DlgGetIDName()
{
    delete ui;
}

bool DlgGetIDName::get(C5User *user, QString &id, QString &name, int table, QWidget *parent)
{
    DlgGetIDName *d;

    if(fDialogs.contains(table)) {
        d = fDialogs[table];
    } else {
        d = new DlgGetIDName(user);
        d->fTable = table;
        d->getData();
    }

    if(d->exec() == QDialog::Accepted) {
        id = d->fId;
        name = d->fName;
        return true;
    }

    return false;
}

void DlgGetIDName::on_btnCancel_clicked()
{
    reject();
}

void DlgGetIDName::getData()
{
    C5Database db;
    db.exec(fQueries[fTable]);
    ui->tbl->setRowCount(db.rowCount());
    ui->tbl->clearSelection();
    int r = 0;

    while(db.nextRow()) {
        ui->tbl->setItem(r, 0, new QTableWidgetItem(db.getString(0)));
        ui->tbl->setItem(r, 1, new QTableWidgetItem(db.getString(1)));
        r++;
    }
}

void DlgGetIDName::on_btnOk_clicked()
{
    if(ui->tbl->currentRow() < 0) {
        return;
    }

    fId = ui->tbl->item(ui->tbl->currentRow(), 0)->text();
    fName = ui->tbl->item(ui->tbl->currentRow(), 1)->text();
    accept();
}

void DlgGetIDName::on_btnRefresh_clicked()
{
    getData();
}

void DlgGetIDName::on_leFilter_textChanged(const QString &arg1)
{
    for(int i = 0; i < ui->tbl->rowCount(); i++) {
        ui->tbl->setRowHidden(i, !ui->tbl->item(i, 0)->text().contains(arg1, Qt::CaseInsensitive)
                              && !ui->tbl->item(i, 1)->text().contains(arg1, Qt::CaseInsensitive));
    }
}

void DlgGetIDName::on_tbl_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    on_btnOk_clicked();
}
