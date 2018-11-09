#include "c5storeinventory.h"
#include "ui_c5storeinventory.h"

C5StoreInventory::C5StoreInventory(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::C5StoreInventory)
{
    ui->setupUi(this);
}

C5StoreInventory::~C5StoreInventory()
{
    delete ui;
}
