#include "dlgordermore.h"
#include "ui_dlgordermore.h"

DlgOrderMore::DlgOrderMore(C5User *user, QWidget *parent)
    : C5Dialog(user, parent)
    , ui(new Ui::DlgOrderMore)
{
    ui->setupUi(this);
    setWindowTitle(tr("Menu"));
}

DlgOrderMore::~DlgOrderMore()
{
    delete ui;
}

void DlgOrderMore::setOptions(const QString &serviceText, bool serviceEnabled,
                              const QString &discountText, bool discountEnabled)
{
    ui->btnMoreService->setText(serviceText);
    ui->btnMoreService->setEnabled(serviceEnabled);
    ui->btnMoreDiscount->setText(discountText);
    ui->btnMoreDiscount->setEnabled(discountEnabled);
}

void DlgOrderMore::on_btnMoreService_clicked()
{
    mChoice = Service;
    accept();
}

void DlgOrderMore::on_btnMoreDiscount_clicked()
{
    mChoice = Discount;
    accept();
}
