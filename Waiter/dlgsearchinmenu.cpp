#include "dlgsearchinmenu.h"
#include "ui_dlgsearchinmenu.h"
#include "c5tabledata.h"

DlgSearchInMenu::DlgSearchInMenu() :
    C5Dialog(C5Config::dbParams()),
    ui(new Ui::DlgSearchInMenu)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 100, 200, 350, 100);
    connect(ui->kb, SIGNAL(textChanged(QString)), this, SLOT(searchDish(QString)));
    connect(ui->kb, SIGNAL(accept()), this, SLOT(kbdAccept()));
    connect(ui->kb, SIGNAL(reject()), this, SLOT(reject()));
    QPushButton *firstBtn = nullptr;
    QJsonArray ja = objs("d_menu_names");
    for (int i = 0; i < ja.size(); i++) {
        QJsonObject jo = ja.at(i).toObject();
        QPushButton *b = new QPushButton();
        if (!firstBtn) {
            firstBtn = b;
        }
        b->setText(jo["f_name"].toString());
        b->setProperty("id", jo["f_id"].toString());
        ui->hl->insertWidget(0, b);
        connect(b, SIGNAL(clicked()), this, SLOT(menuClicked()));
    }
    if (firstBtn) {
        firstBtn->click();
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
    QJsonArray ja = C5TableData::instance()->dishes(menuid, 0);
    for (int i = 0; i < ja.size(); i++) {
        int row = ui->tbl->addEmptyRow();
        const QJsonObject &j = ja.at(i).toObject();
        ui->tbl->setInteger(row, 0, j["f_dish"].toInt());
        ui->tbl->setString(row, 1, j["f_part1name"].toString());
        ui->tbl->setString(row, 2, j["f_part2name"].toString());
        ui->tbl->setString(row, 3, j["f_name"].toString());
        ui->tbl->setDouble(row, 4, j["f_price"].toDouble());
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
    QPushButton *btn = static_cast<QPushButton *>(sender());
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
