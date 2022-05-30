#include "dlggoodslist.h"
#include "ui_dlggoodslist.h"
#include <QKeyEvent>

DlgGoodsList::DlgGoodsList() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgGoodsList)
{
    ui->setupUi(this);
    setWindowState(windowState() | Qt::WindowMaximized);
    C5Database db(__c5config.dbParams());
    db[":f_store"] = __c5config.defaultStore();
    db.exec("select ss.f_goods, gg.f_name as f_groupname, g.f_scancode, g.f_name, "
            "ss.f_qty, g.f_saleprice, g.f_saleprice2 "
            "from a_store_sale ss "
            "left join c_goods g on g.f_id=ss.f_goods "
            "left join c_groups gg on gg.f_id=g.f_group "
            "where ss.f_store=:f_store "
            "union "
            "select g.f_id, gg.f_name as f_groupname, g.f_scancode, g.f_name, "
            "0, g.f_saleprice, g.f_saleprice2 "
            "from c_goods g "
            "left join c_groups gg on gg.f_id=g.f_group "
            "where g.f_service=1 and g.f_enabled=1 ");
    ui->tbl->setRowCount(db.rowCount());
    int row = 0;
    double totalRetail = 0, totalWholesale = 0;
    while (db.nextRow()) {
        for (int c = 0; c < db.columnCount(); c++) {
            ui->tbl->setData(row, c, db.getValue(c));
        }
        totalRetail += db.getDouble("f_saleprice") * db.getDouble("f_qty");
        totalWholesale += db.getDouble("f_saleprice2") * db.getDouble("f_qty");
        row++;
    }
    ui->leTotalRetail->setDouble(totalRetail);
    ui->leTotalRetail->setVisible(!__c5config.getValue(param_shop_hide_store_qty).toInt() == 1);
    ui->lbTotalRetail->setVisible(ui->leTotalRetail->isVisible());
    ui->tbl->setColumnHidden(6, __c5config.shopDenyF2());
    ui->leSearch->installEventFilter(this);
    ui->tbl->resizeColumnsToContents();
}

DlgGoodsList::~DlgGoodsList()
{
    delete ui;
}

bool DlgGoodsList::getGoods(int &id)
{
    DlgGoodsList g;
    if (g.exec() == QDialog::Accepted) {
        id = g.fGoodsId;
        return true;
    }
    return false;
}

bool DlgGoodsList::event(QEvent *event)
{
    if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        switch(ke->key()) {
        case Qt::Key_Up: {
            int r = ui->tbl->currentRow() - 1;
            do {
                if (ui->tbl->isRowHidden(r)) {
                    r--;
                } else {
                    break;
                }
            } while (r > -1);
            if (r < 0) {
                for (int i = 0; i < ui->tbl->rowCount(); i++) {
                    if (!ui->tbl->isRowHidden(i)) {
                        r = i;
                        break;
                    }
                }
            }
            ui->tbl->setCurrentCell(r, 0);
            break;
        }
        case Qt::Key_Down: {
            int r = ui->tbl->currentRow() + 1;
            do {
                if (ui->tbl->isRowHidden(r)) {
                    r++;
                } else {
                    break;
                }
            } while (r < ui->tbl->rowCount());
            if (r > ui->tbl->rowCount() - 1) {
                r = ui->tbl->rowCount() - 1;
            }
            ui->tbl->setCurrentCell(r, 0);
            break;
        }
        case Qt::Key_Enter:
        case Qt::Key_Return: {
            int r = ui->tbl->currentRow();
            if (r > -1) {
                fGoodsId = ui->tbl->getInteger(r, 0);
                accept();
            }
            break;
        }
        }
    }
    return QDialog::event(event);
}

void DlgGoodsList::on_leSearch_textChanged(const QString &arg1)
{
    ui->tbl->selectionModel()->clear();
    QList<int> cols;
    if (ui->chName->isChecked()) {
        cols.append(3);
    }
    if (ui->chScancode->isChecked()) {
        cols.append(2);
    }
    for (int r = 0; r < ui->tbl->rowCount(); r++) {
        bool hidden = true && !arg1.isEmpty();
        for (int c: cols) {
            if (ui->tbl->getString(r, c).contains(arg1, Qt::CaseInsensitive)) {
                hidden = false;
                break;
            }
        }
        ui->tbl->setRowHidden(r, hidden);
    }
}

void DlgGoodsList::on_btnExit_clicked()
{
    reject();
}

void DlgGoodsList::on_btnMinimize_clicked()
{
    showMinimized();
}
