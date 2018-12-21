#include "dlgreportslist.h"
#include "ui_dlgreportslist.h"
#include "c5config.h"
#include <QJsonObject>

DlgReportsList::DlgReportsList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgReportsList)
{
    ui->setupUi(this);
}

DlgReportsList::~DlgReportsList()
{
    delete ui;
}

bool DlgReportsList::getReport(const QJsonArray &ja, QString &fileName)
{
    DlgReportsList *d = new DlgReportsList(__mainWindow);
    for (int i = 0; i < ja.count(); i++) {
        QListWidgetItem *item = new QListWidgetItem(d->ui->lst);
        item->setSizeHint(QSize(100, 50));
        item->setText(ja.at(i).toObject()["caption"].toString());
        item->setData(Qt::UserRole, ja.at(i).toObject()["file"].toString());
        d->ui->lst->addItem(item);
    }
    QListWidgetItem *item = new QListWidgetItem(d->ui->lst);
    item->setSizeHint(QSize(100, 50));
    item->setText(tr("Cancel"));
    item->setIcon(QIcon(":/cancel.png"));
    d->ui->lst->addItem(item);
    bool result = d->exec() == QDialog::Accepted;
    if (result) {
        fileName = d->fResult;
    }
    delete d;
    return result;
}
void DlgReportsList::on_lst_clicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    if (index.row() == ui->lst->count() - 1) {
        reject();
        return;
    }
    fResult = index.data(Qt::UserRole).toString();
    accept();
}
