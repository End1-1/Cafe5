#include "dlgsearchinmenu.h"
#include "ui_dlgsearchinmenu.h"
#include "c5menu.h"

DlgSearchInMenu::DlgSearchInMenu() :
   C5Dialog(C5Config::dbParams()),
    ui(new Ui::DlgSearchInMenu)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 100, 100, 150, 200, 100);
    connect(ui->kb, SIGNAL(textChanged(QString)), this, SLOT(searchDish(QString)));
    connect(ui->kb, SIGNAL(accept()), this, SLOT(kbdAccept()));
    connect(ui->kb, SIGNAL(reject()), this, SLOT(reject()));
    QStringList menus = C5Menu::fMenu.keys();
    foreach (QString s, menus) {
        QPushButton *b = new QPushButton();
        b->setText(s);
        ui->hl->insertWidget(0, b);
        connect(b, SIGNAL(clicked()), this, SLOT(menuClicked()));
    }
}

DlgSearchInMenu::~DlgSearchInMenu()
{
    delete ui;
}

void DlgSearchInMenu::buildMenu(const QString &name)
{
    ui->tbl->clearContents();
    ui->tbl->setRowCount(0);
    for (QMap<QString, QMap<QString, QMap<QString, QList<QJsonObject> > > >::const_iterator im = C5Menu::fMenu.begin(); im != C5Menu::fMenu.end(); im++) {
        if (im.key() != name && !name.isEmpty()) {
            continue;
        }
        for (QMap<QString, QMap<QString, QList<QJsonObject> > >::const_iterator ip1 = im.value().begin(); ip1 != im.value().end(); ip1++) {
            for (QMap<QString, QList<QJsonObject> >::const_iterator ip2 = ip1.value().begin(); ip2 != ip1.value().end(); ip2++) {
                foreach (QJsonObject o, ip2.value()) {
                    int row = ui->tbl->addEmptyRow();
                    ui->tbl->setString(row, 0, im.key());
                    ui->tbl->setString(row, 1, ip1.key());
                    ui->tbl->setString(row, 2, ip2.key());
                    ui->tbl->setString(row, 3, o["f_name"].toString());
                    ui->tbl->setDouble(row, 4, o["f_price"].toString().toDouble());
                    ui->tbl->item(row, 0)->setData(Qt::UserRole, o);
                }
            }
        }
    }
    if (ui->leDishName->text().length() > 0) {
        searchDish(ui->leDishName->text());
    }
}

void DlgSearchInMenu::searchDish(const QString &name)
{
    ui->leDishName->setText(name);
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        bool hidden = true;
        if (ui->tbl->getString(i, 3).contains(name, Qt::CaseInsensitive)) {
            hidden = false;
        }
        ui->tbl->setRowHidden(i, hidden);
    }
}

void DlgSearchInMenu::menuClicked()
{
    QPushButton *btn = static_cast<QPushButton*>(sender());
    buildMenu(btn->text());
}

void DlgSearchInMenu::kbdAccept()
{
    QModelIndexList ml = ui->tbl->selectionModel()->selectedRows();
    if (ml.count() == 0) {
        return;
    }
    QJsonObject o = ui->tbl->item(ml.at(0).row(), 0)->data(Qt::UserRole).toJsonObject();
    emit dish(o);
    accept();
}
