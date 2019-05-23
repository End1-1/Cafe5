#include "c5welcomepage.h"
#include "ui_c5welcomepage.h"
#include "c5storedoc.h"
#include "c5storeinventory.h"
#include "cr5documents.h"
#include "cr5materialsinstore.h"
#include "cr5consumptionbysales.h"
#include "cr5storedocuments.h"
#include "cr5goodsmovement.h"
#include "c5mainwindow.h"
#include "cr5goods.h"
#include "cr5goodsgroup.h"
#include "cr5goodsunit.h"
#include "cr5goodsstorages.h"
#include "cr5goodspartners.h"
#include "cr5menunames.h"
#include "cr5dishpart1.h"
#include "cr5dishpart2.h"
#include "cr5dish.h"
#include "cr5tstoreextra.h"
#include "cr5commonsales.h"
#include "c5cache.h"
#include "c5selector.h"

C5WelcomePage::C5WelcomePage(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5WelcomePage)
{
    ui->setupUi(this);
    setDatabaseName(dbParams.at(1));
    fLabel = tr("Home");
    fIcon = ":/storehouse.png";

}

C5WelcomePage::~C5WelcomePage()
{
    delete ui;
}

void C5WelcomePage::setDatabaseName(const QString &database)
{
    C5Database db(fDBParams);
    db[":f_name"] = database;
    db.exec("select f_db from s_db where f_name=:f_name");
    if (!db.nextRow()) {
        return;
    }
    fDbName = db.getString(0);
    ui->btnNewStoreInput->setVisible(pr(fDbName, cp_t2_store_input));
    ui->btnNewStoreOutput->setVisible(pr(fDbName, cp_t2_store_output));
    ui->btnNewStoreMovement->setVisible(pr(fDbName, cp_t2_store_move));
    ui->btnNewStoreOutput->setVisible(pr(fDbName, cp_t2_store_inventory));
    ui->btnDocs->setVisible(pr(fDbName, cp_t3_documents));
    ui->btnMaterialsInStore->setVisible(pr(fDbName, cp_t3_store));
    ui->btnMovementInStore->setVisible(pr(fDbName, cp_t3_store_movement));
    ui->btnTStoreExtra->setVisible(pr(fDbName, cp_t3_tstore_extra));
    ui->btnGoodsOutputBySales->setVisible(pr(fDbName, cp_t2_count_output_of_sale));
    ui->btnSalesCommon->setVisible(pr(fDbName, cp_t3_sales_common));
    ui->btnStoreDocs->setVisible(pr(fDbName, cp_t3_documents_store));
    ui->btnGoodsGroups->setVisible(pr(fDbName, cp_t6_groups));
    ui->btnGoods->setVisible(pr(fDbName, cp_t6_goods));
    ui->btnStorages->setVisible(pr(fDbName, cp_t6_storage));
    ui->btnUnits->setVisible(pr(fDbName, cp_t6_units));
    ui->btnPartners->setVisible(pr(fDbName, cp_t6_partners));
    ui->btnMenuName->setVisible(pr(fDbName, cp_t4_menu));
    ui->btnDishDepts->setVisible(pr(fDbName, cp_t4_part1));
    ui->btnTypesOfDishes->setVisible(pr(fDbName, cp_t4_part2));
    ui->btnDishes->setVisible(pr(fDbName, cp_t4_dishes));
    ui->btnDatabase->setText(database);
}

void C5WelcomePage::on_btnNewStoreInput_clicked()
{
    C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
    sd->setMode(C5StoreDoc::sdInput);
}

void C5WelcomePage::on_btnNewStoreOutput_clicked()
{
    C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
    sd->setMode(C5StoreDoc::sdOutput);
}

void C5WelcomePage::on_btnNewStoreMovement_clicked()
{
    C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
    sd->setMode(C5StoreDoc::sdMovement);
}

void C5WelcomePage::on_btnNewStoreInventory_clicked()
{
    __mainWindow->createTab<C5StoreInventory>(fDBParams);
}

void C5WelcomePage::on_btnDocs_clicked()
{
    __mainWindow->createTab<CR5Documents>(fDBParams);
}

void C5WelcomePage::on_btnMaterialsInStore_clicked()
{
    __mainWindow->createTab<CR5MaterialsInStore>(fDBParams);
}

void C5WelcomePage::on_btnMovementInStore_clicked()
{
    __mainWindow->createTab<CR5GoodsMovement>(fDBParams);
}

void C5WelcomePage::on_btnTStoreExtra_clicked()
{
    __mainWindow->createTab<CR5TStoreExtra>(fDBParams);
}

void C5WelcomePage::on_btnGoodsOutputBySales_clicked()
{
    __mainWindow->createTab<CR5ConsumptionBySales>(fDBParams);
}

void C5WelcomePage::on_btnSalesCommon_clicked()
{
    __mainWindow->createTab<CR5CommonSales>(fDBParams);
}

void C5WelcomePage::on_btnDatabase_clicked()
{
    QList<QVariant> values;
    if (!C5Selector::getValue(fDBParams, cache_s_db, values)) {
        return;
    }
    setDatabaseName(values.at(1).toString());
}

void C5WelcomePage::on_btnStoreDocs_clicked()
{
    __mainWindow->createTab<CR5StoreDocuments>(fDBParams);
}

void C5WelcomePage::on_btnStorages_clicked()
{
    __mainWindow->createTab<CR5GoodsStorages>(fDBParams);
}

void C5WelcomePage::on_btnGoodsGroups_clicked()
{
    __mainWindow->createTab<CR5GoodsGroup>(fDBParams);
}

void C5WelcomePage::on_btnGoods_clicked()
{
    __mainWindow->createTab<CR5Goods>(fDBParams);
}

void C5WelcomePage::on_btnUnits_clicked()
{
    __mainWindow->createTab<CR5GoodsUnit>(fDBParams);
}

void C5WelcomePage::on_btnPartners_clicked()
{
    __mainWindow->createTab<CR5GoodsPartners>(fDBParams);
}

void C5WelcomePage::on_btnMenuName_clicked()
{
    __mainWindow->createTab<CR5MenuNames>(fDBParams);
}

void C5WelcomePage::on_btnDishDepts_clicked()
{
    __mainWindow->createTab<CR5DishPart1>(fDBParams);
}

void C5WelcomePage::on_btnTypesOfDishes_clicked()
{
    __mainWindow->createTab<CR5DishPart2>(fDBParams);
}

void C5WelcomePage::on_btnDishes_clicked()
{
    __mainWindow->createTab<CR5Dish>(fDBParams);
}
