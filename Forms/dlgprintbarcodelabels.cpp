#include "dlgprintbarcodelabels.h"
#include "ui_dlgprintbarcodelabels.h"
#include "c5storebarcode.h"
#include "c5lineedit.h"
#include "c5message.h"
#include <QPrintDialog>
#include <QHeaderView>

DlgPrintBarcodeLabels::DlgPrintBarcodeLabels(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgPrintBarcodeLabels)
{
    ui->setupUi(this);
    setWindowTitle(tr("Print barcode labels"));
    ui->tbl->setColumnWidth(0, 320);
    ui->tbl->setColumnWidth(1, 160);
    ui->tbl->setColumnWidth(2, 80);
    ui->tbl->setColumnWidth(3, 100);
    ui->tbl->horizontalHeader()->setStretchLastSection(true);
}

DlgPrintBarcodeLabels::~DlgPrintBarcodeLabels()
{
    delete ui;
}

bool DlgPrintBarcodeLabels::printLabels(QWidget *parent, const QList<BarcodeLabelItem> &items)
{
    QList<BarcodeLabelItem> filtered;
    filtered.reserve(items.size());
    for(const BarcodeLabelItem &it : items) {
        if(it.barcode.trimmed().isEmpty()) {
            continue;
        }
        if(it.qty <= 0) {
            continue;
        }
        filtered.append(it);
    }
    if(filtered.isEmpty()) {
        C5Message::info(tr("Nothing to print"));
        return false;
    }

    DlgPrintBarcodeLabels dlg(parent);
    dlg.setItems(filtered);
    return dlg.exec() == QDialog::Accepted;
}

void DlgPrintBarcodeLabels::setItems(const QList<BarcodeLabelItem> &items)
{
    ui->tbl->setRowCount(0);
    for(const BarcodeLabelItem &it : items) {
        const int row = ui->tbl->rowCount();
        ui->tbl->setRowCount(row + 1);
        ui->tbl->setString(row, 0, it.name);
        ui->tbl->setString(row, 1, it.barcode.trimmed());
        ui->tbl->createLineEdit(row, 2)->setInteger(it.qty);
        ui->tbl->setString(row, 3, it.price);
        if(auto *nameItem = ui->tbl->item(row, 0)) {
            nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        }
        if(auto *codeItem = ui->tbl->item(row, 1)) {
            codeItem->setFlags(codeItem->flags() & ~Qt::ItemIsEditable);
        }
        if(auto *priceItem = ui->tbl->item(row, 3)) {
            priceItem->setFlags(priceItem->flags() & ~Qt::ItemIsEditable);
        }
    }
}

bool DlgPrintBarcodeLabels::doPrint()
{
    QPrintDialog pd(this);
    if(pd.exec() != QDialog::Accepted) {
        return false;
    }

    bool any = false;
    for(int i = 0; i < ui->tbl->rowCount(); ++i) {
        C5LineEdit *leQty = ui->tbl->lineEdit(i, 2);
        const int qty = leQty ? leQty->getInteger() : 0;
        if(qty <= 0) {
            continue;
        }
        const QString name = ui->tbl->getString(i, 0);
        const QString code = ui->tbl->getString(i, 1);
        const QString price = ui->tbl->getString(i, 3);
        if(code.isEmpty()) {
            continue;
        }
        for(int j = 0; j < qty; ++j) {
            if(!C5StoreBarcode::printSelected(code, price, name, pd)) {
                C5Message::error(tr("Print error"));
                return false;
            }
            any = true;
        }
    }
    if(!any) {
        C5Message::info(tr("Nothing to print"));
        return false;
    }
    return true;
}

void DlgPrintBarcodeLabels::on_btnPrint_clicked()
{
    if(doPrint()) {
        accept();
    }
}

void DlgPrintBarcodeLabels::on_btnCancel_clicked()
{
    reject();
}
