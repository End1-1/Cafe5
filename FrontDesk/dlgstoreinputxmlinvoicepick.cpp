#include "dlgstoreinputxmlinvoicepick.h"
#include "c5message.h"
#include "ui_dlgstoreinputxmlinvoicepick.h"
#include <QDialogButtonBox>

DlgStoreInputXmlInvoicePick::DlgStoreInputXmlInvoicePick(const QVector<StoreInputXmlInvoice> &invoices, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgStoreInputXmlInvoicePick)
    , mInvoices(invoices)
{
    ui->setupUi(this);
    ui->tblInvoices->setColumnWidth(0, 220);
    ui->tblInvoices->setColumnWidth(1, 90);
    ui->tblInvoices->setColumnWidth(2, 110);
    ui->tblInvoices->setColumnWidth(3, 90);
    ui->tblInvoices->setColumnWidth(4, 90);

    ui->tblInvoices->setRowCount(mInvoices.size());
    for (int i = 0; i < mInvoices.size(); ++i) {
        const StoreInputXmlInvoice &invoice = mInvoices.at(i);
        ui->tblInvoices->setItem(i, 0, new QTableWidgetItem(invoice.supplierName));
        ui->tblInvoices->setItem(i, 1, new QTableWidgetItem(invoice.tin));
        ui->tblInvoices->setItem(i, 2, new QTableWidgetItem(invoice.docNumber));
        ui->tblInvoices->setItem(i, 3, new QTableWidgetItem(invoice.supplyDate.toString(Qt::ISODate)));
        ui->tblInvoices->setItem(i, 4, new QTableWidgetItem(QString::number(invoice.totalPrice, 'f', 2)));
    }

    if (!mInvoices.isEmpty()) {
        ui->tblInvoices->selectRow(0);
    }

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgStoreInputXmlInvoicePick::tryAccept);
}

DlgStoreInputXmlInvoicePick::~DlgStoreInputXmlInvoicePick()
{
    delete ui;
}

StoreInputXmlInvoice DlgStoreInputXmlInvoicePick::selectedInvoice() const
{
    const int row = ui->tblInvoices->currentRow();
    if (row < 0 || row >= mInvoices.size()) {
        return {};
    }
    return mInvoices.at(row);
}

void DlgStoreInputXmlInvoicePick::tryAccept()
{
    if (ui->tblInvoices->currentRow() < 0) {
        C5Message::error(tr("Select invoice"));
        return;
    }
    accept();
}
