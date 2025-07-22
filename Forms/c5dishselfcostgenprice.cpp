#include "c5dishselfcostgenprice.h"
#include "ui_c5dishselfcostgenprice.h"
#include "c5message.h"
#include "c5database.h"
#include "proxytablewidgetdatabase.h"

C5DishSelfCostGenPrice::C5DishSelfCostGenPrice(QWidget *parent) :
    C5Widget(parent),
    ui(new Ui::C5DishSelfCostGenPrice)
{
    ui->setupUi(this);
    fLabel = tr("Calculation of the cost of dishes");
    fIcon = ":/menu.png";
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 100, 200, 300, 100, 100, 100);
}

C5DishSelfCostGenPrice::~C5DishSelfCostGenPrice()
{
    delete ui;
}

QToolBar *C5DishSelfCostGenPrice::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbSave;
        createStandartToolbar(btn);
    }
    return fToolBar;
}

void C5DishSelfCostGenPrice::on_btnStart_clicked()
{
    fGoodsRowMap.clear();
    QMap<int, bool> goods;
    C5Database db;
    db.exec("select distinct(d.f_goods), gr.f_name, g.f_name, u.f_name, 0, '-' "
            "from d_recipes d "
            "left join c_goods g on g.f_id=d.f_goods "
            "left join c_groups gr on gr.f_id=g.f_group "
            "left join c_units u on u.f_id=g.f_unit "
            "order by 2, 3 ");
    ui->tbl->setRowCount(0);
    ProxyTableWidgetDatabase::fillTableWidgetRowFromDatabase( &db, ui->tbl);
    db.first();
    int row = 0;
    while (db.nextRow()) {
        ui->tbl->createLineEdit(row, 4);
        ui->tbl->lineEdit(row, 4)->setText(ui->tbl->getString(row, 4));
        fGoodsRowMap[db.getInt(0)] = row++;
        goods[db.getInt(0)] = false;
    }
    db[":f_date1"] = ui->deStart->date();
    db[":f_date2"] = ui->deEnd->date();
    db.exec("select s.f_goods, sum(s.f_qty), sum(s.f_total) "
            "from a_store s "
            "left join a_header h on h.f_id=s.f_document "
            "where h.f_state=1 and s.f_type=1 and h.f_date between :f_date1 and :f_date2 "
            "group by 1");
    while (db.nextRow()) {
        if (!fGoodsRowMap.contains(db.getInt(0))) {
            continue;
        }
        if (db.getDouble(1) > 0.0001) {
            ui->tbl->lineEdit(fGoodsRowMap[db.getInt(0)], 4)->setDouble(db.getDouble(2) / db.getDouble(1));
            ui->tbl->setString(fGoodsRowMap[db.getInt(0)], 5, "+");
        } else {
            ui->tbl->lineEdit(fGoodsRowMap[db.getInt(0)], 4)->setDouble(0);
            ui->tbl->setString(fGoodsRowMap[db.getInt(0)], 5, "+?");
        }
        goods[db.getInt(0)] = true;
    }
    if (ui->chWhatDoingIfNotExists->isChecked()) {
        for (QMap<int, bool>::const_iterator it = goods.begin(); it != goods.end(); it++) {
            if (it.value() == true) {
                continue;
            }
            db[":f_goods"] = it.key();
            db[":f_date"] = ui->deStart->date();
            db.exec("select  s.f_price "
                    "from a_store s "
                    "left join a_header h on h.f_id=s.f_document "
                    "where h.f_state=1 and s.f_type=1 and h.f_date < :f_date "
                    "and s.f_goods=:f_goods "
                    "order by h.f_date desc "
                    "limit 1 ");
            if (db.nextRow()) {
                if (!fGoodsRowMap.contains(it.key())) {
                    continue;
                }
                if (db.getDouble(0) > 0.0001) {
                    ui->tbl->lineEdit(fGoodsRowMap[it.key()], 4)->setDouble(db.getDouble(0));
                    ui->tbl->setString(fGoodsRowMap[it.key()], 5, "<");
                } else {
                    ui->tbl->lineEdit(fGoodsRowMap[it.key()], 4)->setDouble(0);
                    ui->tbl->setString(fGoodsRowMap[it.key()], 5, "<?");
                }
                goods[it.key()] = true;
            }
        }
    }
}

void C5DishSelfCostGenPrice::saveDataChanges()
{
    C5Database db;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        db[":f_goods"] = ui->tbl->getInteger(i, 0);
        db[":f_price"] = ui->tbl->lineEdit(i, 4)->getDouble();
        db.exec("update d_recipes set f_price=:f_price where f_goods=:f_goods");
        db[":f_id"] = ui->tbl->getInteger(i, 0);
        db[":f_price"] = ui->tbl->lineEdit(i, 4)->getDouble();
        db.exec("update c_goods set f_lastinputprice=:f_price where f_id=:f_id");
    }
    db.exec("update d_dish d set d.f_cost=(select sum(f_qty*f_price) from d_recipes r where r.f_dish=d.f_id)");
    C5Message::info(tr("The prices was updated"));
}

void C5DishSelfCostGenPrice::on_leSearch_textChanged(const QString &arg1)
{
    ui->tbl->search(arg1);
}
