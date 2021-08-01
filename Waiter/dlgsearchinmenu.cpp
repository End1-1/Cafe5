#include "dlgsearchinmenu.h"
#include "ui_dlgsearchinmenu.h"

DlgSearchInMenu::DlgSearchInMenu() :
   C5Dialog(C5Config::dbParams()),
    ui(new Ui::DlgSearchInMenu)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 100, 150, 200, 100);
    connect(ui->kb, SIGNAL(textChanged(QString)), this, SLOT(searchDish(QString)));
    connect(ui->kb, SIGNAL(accept()), this, SLOT(kbdAccept()));
    connect(ui->kb, SIGNAL(reject()), this, SLOT(reject()));

    for (int id: dbmenuname->list()) {
        QPushButton *b = new QPushButton();
        b->setText(dbmenuname->name(id));
        b->setProperty("id", id);
        ui->hl->insertWidget(0, b);
        connect(b, SIGNAL(clicked()), this, SLOT(menuClicked()));
    }
}

DlgSearchInMenu::~DlgSearchInMenu()
{
    delete ui;
}

void DlgSearchInMenu::buildMenu(int menuid)
{
    ui->tbl->clearContents();
    ui->tbl->setRowCount(0);
    if (!menu5->fMenuList.data.contains(menuid)) {
        return;
    }
    QMap<int, DPart1> &m = menu5->fMenuList.data[menuid];
    for (QMap<int, DPart1>::const_iterator im = m.begin(); im != m.end(); im++) {
        for (const DPart2 &p2: im.value().data) {
            extractDishes(p2);
        }
    }
    if (ui->leDishName->text().length() > 0) {
        searchDish(ui->leDishName->text());
    }
}

void DlgSearchInMenu::extractDishes(const DPart2 &p2)
{
    int row = ui->tbl->addEmptyRow();
    for (int d: p2.data2.data) {
        int dishid = dbmenu->dishid(d);
        ui->tbl->setInteger(row, 0, d);
        ui->tbl->setString(row, 1, dbdish->part1name(dishid));
        ui->tbl->setString(row, 2, dbdish->part2name(dishid));
        ui->tbl->setString(row, 3, dbdish->name(dishid));
        ui->tbl->setDouble(row, 4, dbmenu->price(d));
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
    buildMenu(btn->property("id").toInt());
}

void DlgSearchInMenu::kbdAccept()
{
    QModelIndexList ml = ui->tbl->selectionModel()->selectedRows();
    if (ml.count() == 0) {
        return;
    }
    int menuid = ui->tbl->getInteger(ml.at(0).row(), 0);
    emit dish(menuid);
    accept();
}
