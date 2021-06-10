#include "worder.h"
#include "ui_worder.h"
#include "c5ordertabledelegate.h"
#include "c5waiterorderdoc.h"
#include "c5utils.h"

#define PART2_COL_WIDTH 150
#define PART2_ROW_HEIGHT 60
#define PART3_ROW_HEIGHT 80
#define PART4_ROW_HEIGHT 80

WOrder::WOrder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WOrder)
{
    ui->setupUi(this);
}

WOrder::~WOrder()
{
    delete ui;
}

void WOrder::config(C5WaiterOrderDoc *order)
{
    fOrder = order;
    ui->tblOrder->setItemDelegate(new C5OrderTableDelegate());
    ui->tblOrder->setColumnWidth(0, ui->tblOrder->width() - 2);
    ui->tblOrder->verticalHeader()->setDefaultSectionSize(PART4_ROW_HEIGHT);
}

void WOrder::itemsToTable(int guest)
{
    ui->tblOrder->clear();
    ui->tblOrder->setRowCount(fOrder->fItems.count());
    for (int i = 0; i < fOrder->fItems.count(); i++) {
        ui->tblOrder->setItem(i, 0, new QTableWidgetItem());
        ui->tblOrder->item(i, 0)->setData(Qt::UserRole, fOrder->fItems[i].toObject());
        if (fOrder->iInt("f_state", i) != DISH_STATE_OK) {
            ui->tblOrder->setRowHidden(i, true);
        } else {
            bool h = false;
            if (guest > 0) {
                h = guest != fOrder->iInt("f_guest", i);
            }
            ui->tblOrder->setRowHidden(i, h);
        }
    }
    ui->leTotal->setText(float_str(fOrder->hString("f_amounttotal").toDouble(), 2));
    ui->lePrepaiment->setText(fOrder->prepayment(guest));
}

void WOrder::setOrder(C5WaiterOrderDoc *order, int guest)
{
    fOrder = order;
    fGuest = guest;
}

void WOrder::addItem(const QJsonObject &o)
{
    int row = ui->tblOrder->rowCount();
    ui->tblOrder->setRowCount(row + 1);
    ui->tblOrder->setItem(row, 0, new QTableWidgetItem());
    ui->tblOrder->item(row, 0)->setData(Qt::UserRole, o);
    ui->tblOrder->setCurrentCell(ui->tblOrder->rowCount() - 1, 0);
    ui->lePrepaiment->setText(fOrder->prepayment(fGuest));
}

void WOrder::updateItem(const QJsonObject &o, int index)
{
    if (index > -1) {
        ui->tblOrder->item(index, 0)->setData(Qt::UserRole, o);
        ui->tblOrder->viewport()->update();
    }
    ui->tblOrder->scrollToItem(ui->tblOrder->item(index, 0));
    ui->tblOrder->setCurrentItem(ui->tblOrder->item(index, 0));
    ui->lePrepaiment->setText(fOrder->prepayment(fGuest));
}

int WOrder::rowCount()
{
    return ui->tblOrder->rowCount();
}

bool WOrder::currentRow(int &row)
{
    QModelIndexList ml = ui->tblOrder->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return false;
    }
    row = ml.at(0).row();
    return true;
}

void WOrder::updatePayment(int guest)
{
    ui->leTotal->setText(float_str(fOrder->hString("f_amounttotal").toDouble(), 2));
    ui->lePrepaiment->setText(fOrder->prepayment(guest));
    ui->tblOrder->viewport()->update();
}
