#include "c5grid.h"
#include "ui_c5grid.h"
#include "c5tablemodel.h"
#include "c5filtervalues.h"
#include "excel.h"
#include "c5filterwidget.h"
#include "c5gridgilter.h"
#include <QMenu>

C5Grid::C5Grid(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    fDb(dbParams),
    ui(new Ui::C5Grid)
{
    ui->setupUi(this);
    fDBParams = dbParams;
    fModel = new C5TableModel(fDb, this);
    ui->tblView->setModel(fModel);
    fSimpleQuery = true;
    fTableView = ui->tblView;
    fTableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(fTableView->horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tableViewContextMenuRequested(QPoint)));
    connect(fTableView->horizontalHeader(), SIGNAL(clicked(QModelIndex)), this, SLOT(tableViewHeaderClicked(QModelIndex)));
    connect(fTableView->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(tableViewHeaderResized(int,int,int)));
    connect(fTableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChanged(QItemSelection,QItemSelection)));
    fFilterWidget = 0;
    ui->tblTotal->setVisible(false);
}

C5Grid::~C5Grid()
{
    delete ui;
}

void C5Grid::setTableForUpdate(const QString &table, const QList<int> &columns)
{
    fModel->fTableForUpdate = table;
    fModel->fColumnsForUpdate = columns;
}

void C5Grid::postProcess()
{
    buildQuery();
}

void C5Grid::buildQuery()
{
    QString mainTable = fMainTable.mid(fMainTable.indexOf(QRegExp("\\b[a-z,A-Z]*$")), fMainTable.length() - 1);
    QStringList leftJoinTables;
    QMap<QString, QString> leftJoinTablesMap;
    QStringList groupFields;
    if (!fSimpleQuery) {
        fSqlQuery = "select ";
        bool first = true;
        foreach (QString s, fColumnsFields) {
            if (fColumnsVisible[s]) {
                if (first) {
                    first = false;
                } else {
                    fSqlQuery += ",";
                }
                fSqlQuery += s;
                int pos = s.indexOf(QRegExp("\\b[a-z,A-Z]*\\."));
                QString tableOfField = s.mid(pos, s.indexOf(".", pos) - pos);
                if (tableOfField != mainTable) {
                    for (int i = 0; i < fLeftJoinTables.count(); i++) {
                        QString tmpJoinTable = fLeftJoinTables.at(i);
                        pos = tmpJoinTable.indexOf(QRegExp("\\[.*\\]"));
                        tmpJoinTable = tmpJoinTable.mid(pos, tmpJoinTable.length() - pos);
                        tmpJoinTable.replace("[", "").replace("]", "");
                        if (tmpJoinTable == tableOfField) {
                            if (!leftJoinTablesMap.contains(tmpJoinTable)) {
                                leftJoinTablesMap[tmpJoinTable] = fLeftJoinTables.at(i).mid(0, fLeftJoinTables.at(i).length() - (fLeftJoinTables.at(i).length() - fLeftJoinTables.at(i).indexOf(" [")));
                                leftJoinTables << leftJoinTablesMap[tmpJoinTable];
                            }
                            break;
                        }
                    }
                }
                if (fColumnsGroup.contains(s)) {
                    pos = s.indexOf(" as");
                    s = s.mid(0, pos);
                    if (!groupFields.contains(s)) {
                        groupFields << s;
                    }
                }
            }
        }
        fSqlQuery += " from " + fMainTable + " ";
        foreach (QString s, leftJoinTables) {
            fSqlQuery += s + " ";
        }
        if (groupFields.count() > 0) {
            first = true;
            fSqlQuery += " group by ";
            foreach (QString s, groupFields) {
                if (first) {
                    first = false;
                } else {
                    fSqlQuery += ",";
                }
                fSqlQuery += s;
            }
        }
    }
    fModel->translate(fTranslation);
    refreshData();
}

QWidget *C5Grid::widget()
{
    return ui->wd;
}

QHBoxLayout *C5Grid::hl()
{
    return ui->hl;
}

int C5Grid::rowId(int column)
{
    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        C5Message::info(tr("Nothing was selected"));
        return 0;
    }
    return fModel->data(ml.at(0).row(), column, Qt::EditRole).toInt();
}

int C5Grid::rowId(int &row, int column)
{
    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        C5Message::info(tr("Nothing was selected"));
        return 0;
    }
    row = ml.at(0).row();
    return fModel->data(row, column, Qt::EditRole).toInt();
}

void C5Grid::cellClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
}

void C5Grid::callEditor(int id)
{
    Q_UNUSED(id)
}

void C5Grid::sumColumnsData()
{
    if (fColumnsSum.count() == 0) {
        return;
    }
    QMap<QString, double> values;
    fModel->sumForColumns(fColumnsSum, values);
    for (QMap<QString, double>::const_iterator it = values.begin(); it != values.end(); it++) {
        int idx = fModel->indexForColumnName(it.key());
        ui->tblTotal->item(0, idx)->setData(Qt::DisplayRole, float_str(it.value(), 2));
    }
    QStringList vheader;
    vheader << QString::number(fModel->rowCount());
    ui->tblTotal->setVerticalHeaderLabels(vheader);
    ui->tblTotal->setVisible(true);
}

void C5Grid::restoreColumnsVisibility()
{
    QSettings s(_ORGANIZATION_, QString("%1\\%2\\reports\\%3\\visiblecolumns")
                .arg(_APPLICATION_)
                .arg(_MODULE_)
                .arg(fLabel));
    for (QMap<QString, bool>::iterator it = fColumnsVisible.begin(); it != fColumnsVisible.end(); it++) {
        if (s.contains(it.key())) {
            it.value() = s.value(it.key()).toBool();
        }
    }
}

QStringList C5Grid::dbParams()
{
    return fDBParams;
}

void C5Grid::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    qDebug() << selected.indexes();
    qDebug() << deselected.indexes();
}

void C5Grid::filterByColumn()
{
    QSet<QString> filterValues;
    fModel->uniqueValuesForColumn(fFilterColumn, filterValues);
    QStringList sortedValues = filterValues.toList();
    std::sort(sortedValues.begin(), sortedValues.end());
    if (C5FilterValues::filterValues(sortedValues)) {
        fModel->setFilter(fFilterColumn, sortedValues.join("|"));
    }
    sumColumnsData();
}

void C5Grid::removeFilterForColumn()
{
    fModel->removeFilter(fFilterColumn);
}

int C5Grid::newRow()
{
    int row = 0;
    QModelIndexList ml = ui->tblView->selectionModel()->selectedIndexes();
    if (ml.count() > 0) {
        row = ml.at(0).row();
    }
    fModel->insertRow(row);
    ui->tblView->setCurrentIndex(fModel->index(row + 1, 0));
    return row;
}

void C5Grid::editRow(int columnWidthId)
{
    int row = 0;
    QModelIndexList ml = ui->tblView->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        C5Message::info(tr("Nothing was selected"));
        return;
    }
    row = ml.at(0).row();
    callEditor(fModel->data(row, columnWidthId, Qt::EditRole).toInt());
}

void C5Grid::print()
{

}

void C5Grid::exportToExcel()
{
    int colCount = fModel->columnCount();
    int rowCount = fModel->rowCount();
    if (colCount == 0 || rowCount == 0) {
        C5Message::error(tr("Empty report!"));
        return;
    }
    Excel e;
    for (int i = 0; i < colCount; i++) {
        e.setValue(fModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString(), 1, i + 1);
        e.setColumnWidth(i + 1, fTableView->columnWidth(i) / 7);
    }
    QColor color = QColor::fromRgb(200, 200, 250);
    e.setBackground(e.address(0, 0), e.address(0, colCount - 1),
                     color.red(), color.green(), color.blue());
    e.setFontBold(e.address(0, 0), e.address(0, colCount - 1));
    e.setHorizontalAlignment(e.address(0, 0), e.address(0, colCount - 1), Excel::hCenter);

    for (int j = 0; j < rowCount; j++) {
        for (int i = 0; i < colCount; i++) {
            e.setValue(fModel->data(j, i, Qt::EditRole).toString(), j + 2, i + 1);
        }
        color = fModel->data(j, 0, Qt::BackgroundColorRole).value<QColor>();
        e.setBackground(e.address(j + 1, 0), e.address(j + 1, colCount - 1),
                         color.red(), color.green(), color.blue());
    }

    e.setFontSize(e.address(0, 0), e.address(rowCount , colCount ), 10);
    e.show();
}

void C5Grid::clearFilter()
{
    fModel->clearFilter();
}

void C5Grid::setSearchParameters()
{
    if (fFilterWidget) {
        if (C5GridGilter::filter(fFilterWidget, fWhereCondition, fColumnsVisible, fTranslation)) {
            QSettings s(_ORGANIZATION_, QString("%1\\%2\\reports\\%3\\visiblecolumns")
                        .arg(_APPLICATION_)
                        .arg(_MODULE_)
                        .arg(fLabel));
            for (QMap<QString, bool>::const_iterator it = fColumnsVisible.begin(); it != fColumnsVisible.end(); it++) {
                s.setValue(it.key(), it.value());
            }
            buildQuery();
        }
    }
}

void C5Grid::tableViewContextMenuRequested(const QPoint &point)
{
    fFilterColumn = fTableView->columnAt(point.x());
    QString colName = fModel->headerData(fFilterColumn, Qt::Horizontal, Qt::DisplayRole).toString();
    QMenu m;
    m.addAction(QIcon(":/filter_set.png"), QString("%1 '%2'").arg(tr("Set filter")).arg(colName), this, SLOT(filterByColumn()));
    m.addAction(QIcon(":/filter_clear.png"), QString("%1 '%2'").arg(tr("Remove filter")).arg(colName), this, SLOT(removeFilterForColumn()));
    m.exec(fTableView->mapToGlobal(point));
}

void C5Grid::tableViewHeaderClicked(const QModelIndex &index)
{
    fModel->sort(index.column());
}

void C5Grid::tableViewHeaderResized(int column, int oldSize, int newSize)
{
    Q_UNUSED(oldSize);
    ui->tblTotal->setColumnWidth(column, newSize);
    QString columnName = fModel->nameForColumnIndex(column);
    QSettings s(_ORGANIZATION_, QString("%1\\%2\\reports\\%3\\columnswidth")
                .arg(_APPLICATION_)
                .arg(_MODULE_)
                .arg(fLabel));
    s.setValue(columnName, newSize);
}

void C5Grid::saveDataChanges()
{
    fModel->saveDataChanges();
    fTableView->viewport()->update();
}

void C5Grid::refreshData()
{
    QSettings s(_ORGANIZATION_, QString("%1\\%2\\reports\\%3\\columnswidth")
                .arg(_APPLICATION_)
                .arg(_MODULE_)
                .arg(fLabel));
    fModel->execQuery(fSqlQuery);
    ui->tblTotal->setColumnCount(fModel->columnCount());
    for (int i = 0; i < ui->tblTotal->columnCount(); i++) {
        ui->tblTotal->setItem(0, i, new QTableWidgetItem());
        QString colName = fModel->nameForColumnIndex(i);
        if (s.contains(colName)) {
            ui->tblView->setColumnWidth(i, s.value(colName).toInt());
        }
    }
    sumColumnsData();
}

void C5Grid::on_tblView_clicked(const QModelIndex &index)
{
    cellClicked(index);
}
