#include "c5welcomepage.h"
#include "ui_c5welcomepage.h"
#include "c5storedoc.h"
#include "c5storeinventory.h"
#include "cr5documents.h"
#include "cr5materialsinstore.h"
#include "cr5consumptionbysales.h"
#include "cr5goodsmovement.h"
#include "cr5tstoreextra.h"
#include "cr5commonsales.h"

C5WelcomePage::C5WelcomePage(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5WelcomePage)
{
    ui->setupUi(this);
    fDbName = dbParams.at(1);
    fLabel = tr("Home");
    fIcon = ":/storehouse.png";
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
}

C5WelcomePage::~C5WelcomePage()
{
    delete ui;
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
