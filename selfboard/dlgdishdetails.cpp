#include "dlgdishdetails.h"

#include "ui_dlgdishdetails.h"

#include <QPushButton>

DlgDishDetails::DlgDishDetails(const MenuDish &dish, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgDishDetails)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    ui->lblTitle->setText(dish.name);
    connect(ui->btnClose, &QPushButton::clicked, this, &QDialog::accept);
}

DlgDishDetails::~DlgDishDetails()
{
    delete ui;
}
