#include "dlginventorysemireadylist.h"
#include "ui_dlginventorysemireadylist.h"
#include "c5message.h"
#include "c5utils.h"
#include <QDialog>
#include <QHeaderView>
#include <QTableWidgetItem>

DlgInventorySemireadyList::DlgInventorySemireadyList(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgInventorySemireadyList)
{
    ui->setupUi(this);
    ui->tbl->setColumnHidden(0, true);
    ui->tbl->horizontalHeader()->setStretchLastSection(true);
    ui->tbl->setColumnWidth(1, 320);
    ui->tbl->setColumnWidth(2, 80);

    connect(ui->btnClose, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->btnDelete, &QPushButton::clicked, this, &DlgInventorySemireadyList::onDeleteClicked);
}

DlgInventorySemireadyList::~DlgInventorySemireadyList()
{
    delete ui;
}

void DlgInventorySemireadyList::setBatches(const QVector<SemireadyBatch> &batches)
{
    ui->tbl->setRowCount(0);
    for (const SemireadyBatch &b : batches) {
        const int row = ui->tbl->rowCount();
        ui->tbl->insertRow(row);
        ui->tbl->setItem(row, 0, new QTableWidgetItem(b.id));
        ui->tbl->setItem(row, 1, new QTableWidgetItem(b.dishName));
        ui->tbl->setItem(row, 2, new QTableWidgetItem(float_str(b.dishQty, 3)));
    }
}

void DlgInventorySemireadyList::onDeleteClicked()
{
    const int row = ui->tbl->currentRow();
    if (row < 0) {
        return;
    }

    const QString name = ui->tbl->item(row, 1) ? ui->tbl->item(row, 1)->text() : QString();
    if (C5Message::question(tr("Confirm to remove") + "<br>" + name) != QDialog::Accepted) {
        return;
    }

    const QString batchId = ui->tbl->item(row, 0) ? ui->tbl->item(row, 0)->text() : QString();
    if (batchId.isEmpty()) {
        return;
    }

    emit removeBatchRequested(batchId);
}
