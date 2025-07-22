#include "dlgdataonline.h"
#include "ui_dlgdataonline.h"
#include "c5grid.h"
#include "c5tablemodel.h"
#include "dataonline.h"

QHash<QString, DlgDataOnline*> DlgDataOnline::fInstances;

DlgDataOnline::DlgDataOnline(const QString &table) :
    C5Dialog(),
    ui(new Ui::DlgDataOnline)
{
    ui->setupUi(this);
    fTableModel = new C5TableModel(this);
    ui->tableView->setModel(fTableModel);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(ui->tableView, SIGNAL(tblDoubleClick(int, int, QVector<QJsonValue>)), this, SLOT(tblDoubleClicked(int, int,
            QVector<QJsonValue>)));
    //connect(fGrid, SIGNAL(tblSingleClick(QModelIndex)), this, SLOT(tblSingleClick(QModelIndex)));
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this,
            SLOT(selectionChanged(QItemSelection, QItemSelection)));
    fReset = true;
    fSearchColumn = -1;
    DataOnline d;
    fQuery = d.fTableQueries[table];
    refresh();
}

DlgDataOnline::~DlgDataOnline()
{
    delete ui;
}

bool DlgDataOnline::get(const QString &table, DataResult &result, bool multiselect,
                        int searchcolumn)
{
    QString hash = __c5config.dbParams().join(',') + table;

    if(!fInstances.contains(hash)) {
        fInstances.insert(hash, new DlgDataOnline(table));
    }

    auto *d = fInstances[hash];
    d->fMultipleSelection = multiselect;
    d->fSearchColumn = searchcolumn;
    d->ui->leFilter->setFocus();
    d->fValues.clear();

    if(d->exec()) {
        result.fColumns = DataOnline().fColumnNames[table];
        result.fRows = d->fValues;
        return true;
    }

    return false;
}

void DlgDataOnline::on_leFilter_textChanged(const QString &arg1)
{
    fTableModel->setFilter(fSearchColumn, arg1);
}

void DlgDataOnline::on_btnCheck_clicked()
{
    for(int i = 0; i < fTableModel->rowCount(); i++) {
        if(fTableModel->data(i, 0, Qt::CheckStateRole) == Qt::Checked) {
            fValues.append(fTableModel->getRowValues(i));
            accept();
            return;
        }
    }
}

void DlgDataOnline::sqlError(const QString &errorMessage)
{
    C5Message::error(errorMessage);
}

void DlgDataOnline::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    for(QModelIndex i : deselected.indexes()) {
        fTableModel->setData(i.row(), 0, 0, Qt::CheckStateRole);
    }

    for(QModelIndex i : selected.indexes()) {
        fTableModel->setData(i.row(), 0, 1, Qt::CheckStateRole);
    }
}

bool DlgDataOnline::tblDoubleClicked(int row, int column, const QJsonArray &values)
{
    Q_UNUSED(column);
    Q_UNUSED(row);

    if(values.count() == 0) {
        return false;
    }

    fValues.append(values);
    accept();
    return true;
}

void DlgDataOnline::refresh()
{
    fTableModel->setCheckboxes(true);
    fTableModel->setSingleCheckBoxSelection(!fMultipleSelection);
    fTableModel->execQuery(fQuery, this);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setColumnWidth(0, 25);
}

void DlgDataOnline::on_btnRefreshCache_clicked()
{
    refresh();
}
