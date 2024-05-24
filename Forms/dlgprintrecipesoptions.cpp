#include "dlgprintrecipesoptions.h"
#include "ui_dlgprintrecipesoptions.h"

DlgPrintRecipesOptions::DlgPrintRecipesOptions(const QStringList &dbParams, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgPrintRecipesOptions)
{
    Q_UNUSED(dbParams);
    ui->setupUi(this);
}

DlgPrintRecipesOptions::~DlgPrintRecipesOptions()
{
    delete ui;
}

void DlgPrintRecipesOptions::on_btnPrintWithPrices_clicked()
{
    done(1);
}

void DlgPrintRecipesOptions::on_btnPrintWithoutPrices_clicked()
{
    done(2);
}

void DlgPrintRecipesOptions::on_btnCancel_clicked()
{
    reject();
}
