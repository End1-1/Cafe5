#include "storedecomplectation.h"
#include "ui_storedecomplectation.h"

const int col_complete_checkbox = 0;
const int col_complete_uuid = 1;
const int col_complete_goods_id = 2;
const int col_complete_barcode = 3;
const int col_complete_name = 4;
const int col_complete_qty = 5;
const int col_complete_price = 6;
const int col_complete_amount = 7;

const int col_input_uuid = 0;
const int col_input_goods_id = 1;
const int col_input_barcode = 2;
const int col_input_name = 3;
const int col_input_qty = 4;
const int col_input_price = 5;
const int col_input_amount = 6;

StoreDecomplectation::StoreDecomplectation(QWidget *parent)
    : C5Widget(parent), ui(new Ui::StoreDecomplectation)
{
    ui->setupUi(this);
}

StoreDecomplectation::~StoreDecomplectation() { delete ui; }

QToolBar* StoreDecomplectation::toolBar()
{
    createToolBar();
    fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(save()));
    fToolBar->addAction(QIcon(":/draft.png"), tr("Draft"), this, SLOT(draft()));
    return fToolBar;
}

int StoreDecomplectation::addGoodsRow()
{
    int row = ui->tblComplete->rowCount();
    ui->tblComplete->setRowCount(row + 1);
    ui->tblComplete->createCheckbox(row, col_complete_checkbox);
    ui->tblComplete->createLineEdit(row, col_complete_qty);
    return row;
}

void StoreDecomplectation::save()
{
}

void StoreDecomplectation::draft()
{
}

void StoreDecomplectation::on_btnPlus_clicked()
{
    int row = addGoodsRow();
}

void StoreDecomplectation::on_btnRemove_clicked()
{
}
