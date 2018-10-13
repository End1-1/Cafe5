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
    fTableView = ui->tblView;
}

C5Grid::~C5Grid()
{
    delete ui;
}

void C5Grid::setDatabase(const QString &host, const QString &db, const QString &username, const QString &password)
{
    fDb.setDatabase(host, db, username, password);
}

void C5Grid::setTableForUpdate(const QString &table, const QList<int> &columns)
{
    fModel->fTableForUpdate = table;
    fModel->fColumnsForUpdate = columns;
}

void C5Grid::buildQuery()
{
    if (!fSimpleQuery) {

    }
    fModel->translate(fTranslation);
    refreshData();
}

int C5Grid::newRow()
{
    int row = 0;
    QModelIndexList ml = ui->tblView->selectionModel()->selectedRows();
    if (ml.count() > 0) {
        row = ml.at(0).row();
    }
    fModel->insertRow(row);
    ui->tblView->setCurrentIndex(fModel->index(row + 1, 0));
    return row;
}

void C5Grid::saveDataChanges()
{
    fModel->saveDataChanges();
    fTableView->viewport()->update();
}

void C5Grid::refreshData()
{
    fModel->execQuery(fSqlQuery);
}
