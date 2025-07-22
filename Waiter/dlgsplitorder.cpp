#include "dlgsplitorder.h"
#include "ui_dlgsplitorder.h"
#include "c5orderdriver.h"
#include "c5config.h"
#include "c5utils.h"
#include "dlgpassword.h"
#include <QScrollBar>

DlgSplitOrder::DlgSplitOrder(C5User *user) :
    C5Dialog(),
    ui(new Ui::DlgSplitOrder),
    fUser(user)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowFullScreen);
}

DlgSplitOrder::~DlgSplitOrder()
{
    delete ui;
}

void DlgSplitOrder::configOrder(int table)
{
    fHttp->createHttpQuery("/engine/waiter/order.php",
    QJsonObject{{"action", "open"}, {"table", table},
        {"current_staff", fUser->id()},
        {"locksrc", hostinfo},
        {"service_factor", __c5config.getValue(param_service_factor).toDouble()},},
    SLOT(openO1Response(QJsonObject)));
}

void DlgSplitOrder::on_btnChoseTable_clicked()
{

}

void DlgSplitOrder::on_btnExit_clicked()
{
    accept();
}

void DlgSplitOrder::on_btnLeftToRight_clicked()
{
}

bool DlgSplitOrder::moveItem(C5OrderDriver *or1, C5OrderDriver *or2, int row)
{
    return false;
}

void DlgSplitOrder::on_btnAllLeftToRigh_clicked()
{
}

void DlgSplitOrder::on_btnRightToLeft_clicked()
{
}

void DlgSplitOrder::on_btnAllRightToLeft_clicked()
{
}

void DlgSplitOrder::on_btnSave_clicked()
{
}

void DlgSplitOrder::on_btnScrollUp1_clicked()
{
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->value() + 300);
}

void DlgSplitOrder::on_btnScrollDown1_clicked()
{
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->value() - 300);
}

void DlgSplitOrder::on_btnScrollUp2_clicked()
{
    ui->scrollArea_2->verticalScrollBar()->setValue(ui->scrollArea_2->verticalScrollBar()->value() + 300);
}

void DlgSplitOrder::on_btnScrollDown2_clicked()
{
    ui->scrollArea_2->verticalScrollBar()->setValue(ui->scrollArea_2->verticalScrollBar()->value() - 300);
}
