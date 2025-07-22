#include "worderw.h"
#include "ui_worderw.h"
#include "c5ordertabledelegate.h"
#include "c5orderdriver.h"
#include "c5utils.h"
#include "dishitem.h"
#include "dlgorder.h"
#include "dlgguestinfo.h"
#include <QJsonObject>

#define PART2_COL_WIDTH 150
#define PART2_ROW_HEIGHT 60
#define PART3_ROW_HEIGHT 80
#define PART4_ROW_HEIGHT 50

WOrder::WOrder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WOrder)
{
    ui->setupUi(this);
    fSelected = false;
    fOrderDriver = new C5OrderDriver();
    fDlg = nullptr;
    setChanges();
}

WOrder::~WOrder()
{
    delete ui;
    delete fOrderDriver;
}

void WOrder::setDlg(DlgOrder *dlg)
{
    fDlg = dlg;
}

void WOrder::checkAllItems(bool v)
{
    for(auto *w : fItems) {
        w->setChecked(v);
    }
}

QList<int> WOrder::checkedItems()
{
    QList<int> result;

    for(int i = 0; i < fItems.count(); i++) {
        if(fOrderDriver->dishesValue("f_state", i).toInt() == DISH_STATE_OK
                || fOrderDriver->dishesValue("f_state", i).toInt() == DISH_STATE_SET) {
            if(fItems.at(i)->isChecked()) {
                result.append(i);
            }
        }
    }

    return result;
}

void WOrder::updateItem(int index)
{
    for(auto *item : qAsConst(fItems)) {
        item->setChanges();
    }
}

void WOrder::setCurrentRow(int row)
{
    //  TODO
    // FIX MIGRATION
    // for(int i = 0, count = ui->vl->count(); i < count; i++) {
    //     QLayoutItem *l = ui->vl->itemAt(i);
    //     DishItem *d = dynamic_cast<DishItem*>(l->widget());
    //     if(d) {
    //         if(i == row) {
    //             d->clearFocus(row);
    //         } else {
    //             d->clearFocus(-1);
    //         }
    //     }
    // }
}

void WOrder::setSelected(bool v)
{
    fSelected = v;
    setChanges();

    if(!fSelected) {
        for(int i = 0, count = ui->vl->count(); i < count; i++) {
            QLayoutItem *l = ui->vl->itemAt(i);
            DishItem *d = dynamic_cast<DishItem*>(l->widget());

            if(d) {
                d->clearFocus();
            }
        }
    }
}

bool WOrder::isSelected()
{
    return fSelected;
}

void WOrder::setCheckMode(bool v)
{
    for(int i = 0, count = ui->vl->count(); i < count; i++) {
        QLayoutItem *l = ui->vl->itemAt(i);
        DishItem *d = dynamic_cast<DishItem*>(l->widget());

        if(d) {
            d->setCheckMode(v);
        }
    }
}

DishItem* WOrder::dishWidget(int i)
{
    return fItems[i];
}

void WOrder::setChanges()
{
    ui->frameHeader->setProperty("guest_selected", fSelected ? "1" : "2");
    ui->frameHeader->style()->polish(ui->frameHeader);
}

QPoint WOrder::focused(int index)
{
    if(!fSelected) {
        emit activated();
    }

    QPoint p(0, 0);

    for(int i = 0, count = ui->vl->count(); i < count; i++) {
        QLayoutItem *l = ui->vl->itemAt(i);
        DishItem *d = dynamic_cast<DishItem*>(l->widget());

        if(d) {
            d->clearFocus();

            if(d->isFocused()) {
                p = d->mapTo(fDlg, QPoint(0, 0));
            }
        }
    }

    return p;
}

void WOrder::on_btnActivate_clicked()
{
    emit activated();
}

void WOrder::on_btnEditGuestName_clicked()
{
    DlgGuestInfo gi;
    gi.exec();

    if(gi.fCode > 0) {
    }
}
