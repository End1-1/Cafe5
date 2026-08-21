#include "dlgstoreinputxmlinvoicepick.h"
#include "c5message.h"
#include "dlgstoreinputxmlinvoicepreview.h"
#include "ui_dlgstoreinputxmlinvoicepick.h"
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QTableWidgetItem>

DlgStoreInputXmlInvoicePick::DlgStoreInputXmlInvoicePick(const QVector<StoreInputXmlInvoice> &invoices, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgStoreInputXmlInvoicePick)
    , mInvoices(invoices)
{
    ui->setupUi(this);
    ui->tblInvoices->setColumnCount(6);
    ui->tblInvoices->setHorizontalHeaderLabels(
        {tr("Import"), tr("Supplier"), tr("TIN"), tr("Invoice #"), tr("Date"), tr("Total")});
    ui->tblInvoices->horizontalHeader()->setStretchLastSection(false);
    ui->tblInvoices->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tblInvoices->setColumnWidth(0, 70);
    ui->tblInvoices->setColumnWidth(2, 90);
    ui->tblInvoices->setColumnWidth(3, 120);
    ui->tblInvoices->setColumnWidth(4, 100);
    ui->tblInvoices->setColumnWidth(5, 90);
    ui->tblInvoices->setRowCount(mInvoices.size());

    for (int i = 0; i < mInvoices.size(); ++i) {
        const StoreInputXmlInvoice &invoice = mInvoices.at(i);

        auto *check = new QCheckBox(ui->tblInvoices);
        check->setChecked(true);
        auto *wrap = new QWidget(ui->tblInvoices);
        auto *layout = new QHBoxLayout(wrap);
        layout->setContentsMargins(6, 0, 6, 0);
        layout->addWidget(check);
        layout->addStretch();
        ui->tblInvoices->setCellWidget(i, 0, wrap);
        mChecks.append(check);

        ui->tblInvoices->setItem(i, 1, new QTableWidgetItem(invoice.supplierName));
        ui->tblInvoices->setItem(i, 2, new QTableWidgetItem(invoice.tin));
        ui->tblInvoices->setItem(i, 3, new QTableWidgetItem(invoice.docNumber));
        ui->tblInvoices->setItem(i, 4, new QTableWidgetItem(invoice.supplyDate.toString(Qt::ISODate)));
        ui->tblInvoices->setItem(i, 5, new QTableWidgetItem(QString::number(invoice.totalPrice, 'f', 2)));
    }

    connect(ui->btnSelectAll, &QPushButton::clicked, this, [this]() { selectAll(true); });
    connect(ui->btnClearAll, &QPushButton::clicked, this, [this]() { selectAll(false); });
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgStoreInputXmlInvoicePick::tryAccept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &DlgStoreInputXmlInvoicePick::reject);
    connect(ui->tblInvoices, &QTableWidget::cellDoubleClicked, this, &DlgStoreInputXmlInvoicePick::onInvoiceDoubleClicked);
    connect(ui->leSearch, &QLineEdit::textChanged, this, &DlgStoreInputXmlInvoicePick::onSearchTextChanged);
}

DlgStoreInputXmlInvoicePick::~DlgStoreInputXmlInvoicePick()
{
    delete ui;
}

QVector<StoreInputXmlInvoice> DlgStoreInputXmlInvoicePick::selectedInvoices() const
{
    QVector<StoreInputXmlInvoice> result;
    result.reserve(mInvoices.size());
    for (int i = 0; i < mInvoices.size(); ++i) {
        if (mChecks.at(i)->isChecked()) {
            result.append(mInvoices.at(i));
        }
    }
    return result;
}

void DlgStoreInputXmlInvoicePick::selectAll(bool checked)
{
    for (QCheckBox *check : mChecks) {
        check->setChecked(checked);
    }
}

void DlgStoreInputXmlInvoicePick::onInvoiceDoubleClicked(int row, int column)
{
    Q_UNUSED(column);
    if (row < 0 || row >= mInvoices.size()) {
        return;
    }
    DlgStoreInputXmlInvoicePreview::showInvoice(mInvoices.at(row), this);
}

void DlgStoreInputXmlInvoicePick::onSearchTextChanged(const QString &text)
{
    const QString needle = text.trimmed();
    for (int i = 0; i < ui->tblInvoices->rowCount(); ++i) {
        if (needle.isEmpty()) {
            ui->tblInvoices->setRowHidden(i, false);
            continue;
        }
        bool match = false;
        for (int col = 1; col < ui->tblInvoices->columnCount(); ++col) {
            const QTableWidgetItem *item = ui->tblInvoices->item(i, col);
            if (item && item->text().contains(needle, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        ui->tblInvoices->setRowHidden(i, !match);
    }
}

void DlgStoreInputXmlInvoicePick::tryAccept()
{
    if (selectedInvoices().isEmpty()) {
        C5Message::error(tr("Check at least one invoice"));
        return;
    }
    accept();
}
