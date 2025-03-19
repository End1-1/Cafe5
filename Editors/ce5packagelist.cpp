#include "ce5packagelist.h"
#include "ui_ce5packagelist.h"
#include "c5cache.h"
#include "c5selector.h"

CE5PackageList::CE5PackageList(const QStringList &dbParams, int package) :
    C5Dialog(dbParams),
    ui(new Ui::CE5PackageList)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidths(5, 0, 200, 0, 300, 80);
    ui->lePackage->setSelector(fDBParams, ui->lePackageName, cache_dish_package);
    setPackage(package);
    connect(ui->lePackage, SIGNAL(done(QVector<QJsonValue>)), this, SLOT(packageChanged(QVector<QJsonValue>)));
}

CE5PackageList::~CE5PackageList()
{
    delete ui;
}

void CE5PackageList::setPackage(int id)
{
    ui->lePackage->setValue(id);
    ui->tbl->setRowCount(0);
    setWindowTitle(QString("%1 %2").arg(tr("Package")).arg(ui->lePackageName->text()));
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select f_price from d_package where f_id=:f_id");
    if (db.nextRow()) {
        ui->lePrice->setDouble(db.getDouble(0));
    }
    db[":f_package"] = ui->lePackage->getInteger();
    db.exec("select pl.f_id, t.f_name as type_name, pl.f_dish, d.f_name as dish_name, pl.f_price "
            "from d_package_list pl "
            "left join d_dish d on d.f_id=pl.f_dish "
            "left join d_part2 t on t.f_id=d.f_part "
            "where pl.f_package=:f_package");
    while (db.nextRow()) {
        addDish(db.getInt(0), db.getString(1), db.getInt(2), db.getString(3), db.getDouble(4));
    }
    countPrices();
}

void CE5PackageList::addDish(int row, const QString &typeName, int dishCode, const QString &dishName, double price)
{
    int r = ui->tbl->addEmptyRow();
    ui->tbl->setData(r, 0, row);
    ui->tbl->setData(r, 1, typeName);
    ui->tbl->setData(r, 2, dishCode);
    ui->tbl->setData(r, 3, dishName);
    C5LineEdit *l = ui->tbl->createLineEdit(r, 4);
    l->setDouble(price);
    connect(l, SIGNAL(textChanged(QString)), this, SLOT(itemPriceChanged(QString)));
}

void CE5PackageList::countPrices()
{
    double price = 0;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        price += ui->tbl->lineEdit(i, 4)->getDouble();
    }
    ui->leSum->setDouble(price);
}

void CE5PackageList::itemPriceChanged(const QString &str)
{
    Q_UNUSED(str);
    countPrices();
}

void CE5PackageList::packageChanged(const QVector<QJsonValue> &val)
{
    if (val.count() > 0) {
        setPackage(val.at(0).toInt());
    }
}

void CE5PackageList::on_btnAdd_clicked()
{
    QJsonArray values;
    if (!C5Selector::getValue(fDBParams, cache_dish, values)) {
        return;
    }
    addDish(0, values.at(2).toString(), values.at(0).toInt(), values.at(1).toString(), 0);
    countPrices();
}

void CE5PackageList::on_btnSave_clicked()
{
    C5Database db(fDBParams);
    db[":f_package"] = ui->lePackage->getInteger();
    db.exec("delete from d_package_list where f_package=:f_package");
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        db[":f_package"] = ui->lePackage->getInteger();
        db[":f_dish"] = ui->tbl->getInteger(i, 2);
        db[":f_price"] = ui->tbl->lineEdit(i, 4)->getDouble();
        ui->tbl->setInteger(i, 0, db.insert("d_package_list"));
    }
    db[":f_id"] = ui->lePackage->getInteger();
    db[":f_price"] = ui->lePrice->getDouble();
    db.exec("update d_package set f_price=:f_price where f_id=:f_id");
    C5Message::info(tr("Saved"));
}

void CE5PackageList::on_btnRemove_clicked()
{
    QModelIndexList ml = ui->tbl->selectionModel()->selectedRows();
    for (int i = ml.count() - 1; i > -1; i--) {
        ui->tbl->removeRow(ml.at(i).row());
    }
    countPrices();
}
