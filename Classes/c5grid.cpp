#include "c5grid.h"
#include "ui_c5grid.h"
#include "c5tablemodel.h"

C5Grid::C5Grid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::C5Grid)
{
    ui->setupUi(this);
    fModel = new C5TableModel(fDb, this);
    ui->tblView->setModel(fModel);
    fSimpleQuery = true;
}

C5Grid::~C5Grid()
{
    delete ui;
}

void C5Grid::setDatabase(const QString &host, const QString &db, const QString &username, const QString &password)
{
    fDb.setDatabase(host, db, username, password);
}

void C5Grid::buildQuery()
{
    if (!fSimpleQuery) {

    }
    fModel->translate(fTranslation);
    refreshData();
}

void C5Grid::refreshData()
{
    fModel->execQuery(fSqlQuery);
}
