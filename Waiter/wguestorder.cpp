#include "wguestorder.h"
#include "ui_wguestorder.h"

WGuestOrder::WGuestOrder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WGuestOrder)
{
    ui->setupUi(this);
}

WGuestOrder::~WGuestOrder()
{
    delete ui;
}

WOrder *WGuestOrder::worder()
{
    return ui->worder;
}
