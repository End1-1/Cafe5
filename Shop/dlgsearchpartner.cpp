#include "dlgsearchpartner.h"
#include "c5config.h"
#include "c5database.h"
#include "ui_dlgsearchpartner.h"

DlgSearchPartner::DlgSearchPartner() :
    C5Dialog(),
    ui(new Ui::DlgSearchPartner)
{
    ui->setupUi(this);
    ui->waddpartner->setVisible(false);
    loadPartners();
    ui->tbl->resizeColumnsToContents();
    ui->tbl->setColumnWidth(0, 0);
}

DlgSearchPartner::~DlgSearchPartner()
{
    delete ui;
}

void DlgSearchPartner::on_btnAddPartner_clicked()
{
    ui->waddpartner->setVisible(true);
}

void DlgSearchPartner::on_btnCancel_clicked()
{
    ui->waddpartner->setVisible(false);
}

void DlgSearchPartner::loadPartners()
{
    ui->tbl->setRowCount(0);
    C5Database db;
    db.exec("select f_id, f_taxcode, f_taxname, f_contact, f_phone from c_partners");

    while(db.nextRow()) {
        int r = ui->tbl->rowCount();
        ui->tbl->setRowCount(r + 1);

        for(int i = 0; i < db.columnCount(); i++) {
            ui->tbl->setItem(r, i, new QTableWidgetItem(db.getString(i)));
        }

        ui->tbl->item(r, 0)->setData(Qt::DisplayRole, db.getInt(0));
    }
}

void DlgSearchPartner::on_tbl_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column);

    for(int i = 0; i < ui->tbl->columnCount(); i++) {
        fData[i] = ui->tbl->item(row, i)->data(Qt::EditRole);
    }

    QString resultStr = ui->tbl->item(row, 0)->text();
    result = resultStr.toInt();
    accept();
}

void DlgSearchPartner::on_btnSave_clicked()
{
    C5Database db;
    db[":f_taxcode"] = ui->leTaxcode->text();
    db[":f_taxname"] = ui->leTaxName->text();
    db[":f_contact"] = ui->leContact->text();
    db[":f_phone"] = ui->lePhone->text();
    db.insert("c_partners", false);
    loadPartners();
    ui->lineEdit->setText(ui->leTaxcode->text());
    ui->waddpartner->setVisible(false);
}

void DlgSearchPartner::on_lineEdit_textChanged(const QString &arg1)
{
    for(int i = 0; i < ui->tbl->rowCount(); i++) {
        bool h = true;

        for(int j = 0; j < ui->tbl->columnCount(); j++) {
            if(ui->tbl->item(i, j)->text().contains(arg1, Qt::CaseInsensitive)) {
                h = false;
                break;
            }
        }

        ui->tbl->setRowHidden(i, h);
    }
}
