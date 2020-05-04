#include "settingsselection.h"
#include "ui_settingsselection.h"
#include "c5config.h"
#include <QListWidgetItem>

SettingsSelection::SettingsSelection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsSelection)
{
    ui->setupUi(this);
}

SettingsSelection::~SettingsSelection()
{
    delete ui;
}

void SettingsSelection::addSettingsId(int settingsId, const QString &settingsName)
{
    QListWidgetItem *item = new QListWidgetItem(ui->lst);
    item->setFont(font());
    item->setSizeHint(QSize(100, 50));
    item->setText(settingsName);
    item->setData(Qt::UserRole, settingsId);
}

void SettingsSelection::on_lst_clicked(const QModelIndex &index)
{
    if (index.row() == ui->lst->count() - 1) {
        reject();
        return;
    }
    __c5config.fSettingsName = ui->lst->item(index.row())->text();
    __c5config.initParamsFromDb();
    accept();
}
