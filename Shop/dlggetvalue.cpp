#include "dlggetvalue.h"
#include "ui_dlggetvalue.h"

DlgGetValue::DlgGetValue(const QStringList &values, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgGetValue)
{
    ui->setupUi(this);
    for (const QString &s : values) {
        QListWidgetItem *item = new QListWidgetItem(ui->lst);
        item->setText(s);
        item->setSizeHint(QSize(300, 50));
        ui->lst->addItem(item);
    }
}

DlgGetValue::~DlgGetValue()
{
    delete ui;
}

QString DlgGetValue::value() const
{
    auto *item = ui->lst->currentItem();
    if (!item) {
        return "INVALID ITEM";
    }
    return item->text();
}

void DlgGetValue::on_btnCancel_clicked()
{
    reject();
}

void DlgGetValue::on_btnOk_clicked()
{
    auto *item = ui->lst->currentItem();
    if (!item) {
        return;
    }
    accept();
}
