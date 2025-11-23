#include "armsoftexportoptions.h"
#include "ui_armsoftexportoptions.h"
#include "c5config.h"

ArmSoftExportOptions::ArmSoftExportOptions(QWidget *parent) :
    C5Dialog(),
    ui(new Ui::ArmSoftExportOptions)
{
    ui->setupUi(this);
    ui->lesExpenseAcc->setText(__c5config.getRegValue("lesexpenseacc", "").toString());
    ui->lesIncomeAcc->setText(__c5config.getRegValue("lesincomeacc", "").toString());
    ui->lemExpenseAcc->setText(__c5config.getRegValue("lemexpenseacc", "").toString());
    ui->lemIncomeAcc->setText(__c5config.getRegValue("lemincomeacc", "").toString());
    ui->leVATTypet->setText(__c5config.getRegValue("levattypet", "").toString());
    ui->leVATTypeb->setText(__c5config.getRegValue("levattypeb", "").toString());
}

ArmSoftExportOptions::~ArmSoftExportOptions()
{
    delete ui;
}

void ArmSoftExportOptions::on_btnCancel_clicked()
{
    reject();
}

void ArmSoftExportOptions::on_btnOk_clicked()
{
    __c5config.setRegValue("lesexpenseacc", ui->lesExpenseAcc->text());
    __c5config.setRegValue("lesincomeacc", ui->lesIncomeAcc->text());
    __c5config.setRegValue("lemexpenseacc", ui->lemExpenseAcc->text());
    __c5config.setRegValue("lemincomeacc", ui->lemIncomeAcc->text());
    __c5config.setRegValue("levattypet", ui->leVATTypet->text());
    __c5config.setRegValue("levattypeb", ui->leVATTypeb->text());
    accept();
}
