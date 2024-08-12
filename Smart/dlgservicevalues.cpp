#include "dlgservicevalues.h"
#include "ui_dlgservicevalues.h"
#include "c5utils.h"
#include <QJsonArray>

DlgServiceValues::DlgServiceValues(QWidget *parent)
    : QDialog(parent, Qt::FramelessWindowHint)
    , ui(new Ui::DlgServiceValues)
{
    ui->setupUi(this);
}

DlgServiceValues::~DlgServiceValues()
{
    delete ui;
}

void DlgServiceValues::config(const QJsonObject &jo)
{
    QJsonArray ja = jo["service"].toArray();
    for (int i = 0; i < ja.size(); i++) {
        const QJsonObject js = ja.at(i).toObject();
        auto *item = new QListWidgetItem(ui->lst);
        item->setText(float_str(js["f_value"].toDouble(), 2));
        item->setData(Qt::UserRole, js["f_value"].toDouble());
        ui->lst->addItem(item);
    }
    QListWidgetItem *cancelItem = new QListWidgetItem(ui->lst);
    cancelItem->setIcon(QIcon(":/cancel.png"));
    cancelItem->setText(tr("Cancel"));
    ui->lst->addItem(cancelItem);
}

void DlgServiceValues::on_lst_clicked(const QModelIndex &index)
{
    if (index.row() == ui->lst->count() - 1) {
        reject();
    }
}
