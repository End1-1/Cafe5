#include "dlgstoreinputxmlinvoicepreview.h"
#include "ui_dlgstoreinputxmlinvoicepreview.h"

#include "c5utils.h"

#include <QHeaderView>
#include <QTableWidgetItem>

DlgStoreInputXmlInvoicePreview::DlgStoreInputXmlInvoicePreview(const StoreInputXmlInvoice &invoice, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgStoreInputXmlInvoicePreview)
{
    ui->setupUi(this);
    setWindowTitle(tr("Invoice preview — %1").arg(invoice.docNumber));

    ui->lbHeader->setText(
        tr("Invoice: %1\nSupplier: %2\nTIN: %3\nDate: %4\nTotal: %5\nLines: %6")
            .arg(invoice.docNumber,
                 invoice.supplierName,
                 invoice.tin,
                 invoice.supplyDate.toString(Qt::ISODate),
                 float_str(invoice.totalPrice, 2),
                 QString::number(invoice.goods.size())));

    ui->tblGoods->setColumnCount(5);
    ui->tblGoods->setHorizontalHeaderLabels(
        {tr("Description"), tr("Qty"), tr("Unit"), tr("Price"), tr("Total")});
    ui->tblGoods->horizontalHeader()->setStretchLastSection(false);
    ui->tblGoods->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tblGoods->setColumnWidth(1, 80);
    ui->tblGoods->setColumnWidth(2, 70);
    ui->tblGoods->setColumnWidth(3, 90);
    ui->tblGoods->setColumnWidth(4, 100);
    ui->tblGoods->setRowCount(invoice.goods.size());

    for (int i = 0; i < invoice.goods.size(); ++i) {
        const StoreInputXmlGoodLine &line = invoice.goods.at(i);
        const double total = line.totalPrice > 0.0001 ? line.totalPrice : (line.qty * line.pricePerUnit);
        ui->tblGoods->setItem(i, 0, new QTableWidgetItem(line.description));
        auto *qtyItem = new QTableWidgetItem(float_str(line.qty, 3));
        qtyItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tblGoods->setItem(i, 1, qtyItem);
        ui->tblGoods->setItem(i, 2, new QTableWidgetItem(line.unit));
        auto *priceItem = new QTableWidgetItem(float_str(line.pricePerUnit, 2));
        priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tblGoods->setItem(i, 3, priceItem);
        auto *totalItem = new QTableWidgetItem(float_str(total, 2));
        totalItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tblGoods->setItem(i, 4, totalItem);
    }
}

DlgStoreInputXmlInvoicePreview::~DlgStoreInputXmlInvoicePreview()
{
    delete ui;
}

void DlgStoreInputXmlInvoicePreview::showInvoice(const StoreInputXmlInvoice &invoice, QWidget *parent)
{
    DlgStoreInputXmlInvoicePreview dlg(invoice, parent);
    dlg.exec();
}
