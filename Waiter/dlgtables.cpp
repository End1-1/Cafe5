#include "dlgtables.h"
#include "ui_dlgtables.h"
#include "ninterface.h"
#include "c5utils.h"
#include "c5user.h"
#include "struct_workstationitem.h"
#include "../WaiterDesigner/waitertablestyle.h"
#include "../WaiterDesigner/tablecelldelegate.h"
#include <QApplication>
#include <QScrollBar>

DlgTables::DlgTables(C5User *user) :
    C5WaiterDialog(user),
    ui(new Ui::DlgTables)
{
    ui->setupUi(this);
    WaiterTblTablesStyle::applyToTable(ui->tblTables);
}

DlgTables::~DlgTables()
{
    delete ui;
}

int DlgTables::exec()
{
    C5WaiterDialog::exec();
    return mResult;
}

void DlgTables::showEvent(QShowEvent *e)
{
    C5WaiterDialog::showEvent(e);
    NInterface::query1("/engine/v2/waiter/hall/get", mUser->mSessionKey, this, {},
    [this](const QJsonObject & jdoc) {
        mHall = parseJsonArray<HallItem>(jdoc["halls"].toArray());
        mTables = parseJsonArray<TableItem>(jdoc["tables"].toArray());
        int i = 0;

        for(auto const &h : std::as_const(mHall)) {
            QToolButton *btn = new QToolButton();
            btn->setText(h.name);
            btn->setToolButtonStyle(Qt::ToolButtonTextOnly);
            btn->setMinimumSize(QSize(140, 50));
            btn->setProperty("id", h.id);
            connect(btn, &QToolButton::clicked, this, &DlgTables::hallClicked);
            ui->gh->insertWidget(ui->gh->count() - 2, btn);
        }

        auto *tbl = ui->tblTables;
        auto *header = tbl->horizontalHeader();
        int baseWidth = header->defaultSectionSize();
        int tableWidth = tbl->viewport()->width();

        if(tbl->verticalScrollBar()->isVisible()) {
            tableWidth -= tbl->verticalScrollBar()->width();
        }

        int cols = qMax(1, tableWidth / baseWidth);
        tbl->setColumnCount(cols);
        int extra = tableWidth - cols * baseWidth;
        int add = extra / cols;
        int finalWidth = baseWidth + add;
        header->setSectionResizeMode(QHeaderView::Fixed);

        for(int c = 0; c < cols; ++c) {
            tbl->setColumnWidth(c, finalWidth);
        }

        hallClicked();
    });
}

void DlgTables::hallClicked()
{
    auto* btn = qobject_cast<QToolButton*>(sender());
    int hall = btn ? btn->property("id").toInt() : mWorkStation.defaultHallId();
    QVector<TableItem> copy;

    for(auto t : mTables) {
        if(hall == 0  || t.hall == hall) {
            copy.append(t);
        }
    }

    int r = 0;
    int c = 0;
    int rows = (copy.size() + ui->tblTables->columnCount() - 1) / ui->tblTables->columnCount();
    ui->tblTables->setRowCount(rows);

    for(auto t : copy) {
        auto *item = new QTableWidgetItem();
        item->setData(WaiterRoleId, t.id);
        item->setData(WaiterRoleState, t.tableState);
        item->setData(WaiterRoleAmount, t.amount);
        item->setData(WaiterRoleStaff, t.staffName);
        item->setData(WaiterRoleName, t.name);
        ui->tblTables->setItem(r, c, item);
        ++c;

        if(c >= ui->tblTables->columnCount()) {
            c = 0;
            ++r;
        }
    }
}

void DlgTables::on_btnCancel_clicked()
{
    reject();
}

void DlgTables::on_tblTables_itemClicked(QTableWidgetItem *item)
{
    if(!item) {
        return;
    }

    if(item->data(WaiterRoleId).toInt() == 0) {
        return;
    }

    mResult = item->data(WaiterRoleId).toInt();
    mTableName = item->data(WaiterRoleName).toString();
    accept();
}
