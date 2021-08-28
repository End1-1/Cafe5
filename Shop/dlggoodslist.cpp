#include "dlggoodslist.h"
#include "ui_dlggoodslist.h"

DlgGoodsList::DlgGoodsList() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgGoodsList)
{
    ui->setupUi(this);
}

DlgGoodsList::~DlgGoodsList()
{
    delete ui;
}

bool DlgGoodsList::getGoods(int &id)
{
    DlgGoodsList g;
    g.exec();
    return true;
}

void DlgGoodsList::on_leSearch_textChanged(const QString &arg1)
{
    QList<int> cols;
    if (ui->chName->isChecked()) {
        cols.append(2);
    }
    if (ui->chScancode->isChecked()) {
        cols.append(1);
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
