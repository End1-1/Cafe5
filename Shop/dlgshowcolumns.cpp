#include "dlgshowcolumns.h"
#include "ui_dlgshowcolumns.h"
#include "worder.h"
#include <QSettings>

static QSettings s(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);

DlgShowColumns::DlgShowColumns() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgShowColumns)
{
    ui->setupUi(this);
    ui->chGroup->setChecked(s.value("col" + QString::number(col_group)).toBool());
    ui->chUnit->setChecked(s.value("col" + QString::number(col_unit)).toBool());
    ui->chBox->setChecked(s.value("col" + QString::number(col_qtybox)).toBool());
}

DlgShowColumns::~DlgShowColumns()
{
    delete ui;
}

void DlgShowColumns::on_chGroup_clicked(bool checked)
{
    s.setValue("col" + QString::number(col_group), checked);
}

void DlgShowColumns::on_chUnit_clicked(bool checked)
{
    s.setValue("col" + QString::number(col_unit), checked);
}

void DlgShowColumns::on_chBox_clicked(bool checked)
{
    s.setValue("col" + QString::number(col_qtybox), checked);
}

void DlgShowColumns::on_btnClose_clicked()
{
    accept();
}
