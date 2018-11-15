#include "c5grid.h"
#include "ui_c5grid.h"
#include "c5tablemodel.h"
#include "c5filtervalues.h"
#include "excel.h"
#include "c5filterwidget.h"
#include "c5printing.h"
#include "c5printpreview.h"
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
    connect(fTableView->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(tableViewHeaderClicked(int)));
    connect(fTableView->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(tableViewHeaderResized(int,int,int)));
    connect(fTableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChanged(QItemSelection,QItemSelection)));
    fFilterWidget = 0;
    ui->tblTotal->setVisible(false);
}

C5Grid::~C5Grid()
{
    if (fFilterWidget) {
        delete fFilterWidget;
    }
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
                    insertJoinTable(leftJoinTables, leftJoinTablesMap, tableOfField, mainTable);
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
        if (fFilterWidget) {
            fWhereCondition = fFilterWidget->condition();
        }
        int p = 0;
        QRegExp re("\\b[a-z]*\\.");
        re.setMinimal(true);
        while ((p = re.indexIn(fWhereCondition, p)) != -1) {
            QString table = re.cap(0);
            p += re.matchedLength();
            table = table.remove(table.length() - 1, 1);
            if (table != mainTable) {
                insertJoinTable(leftJoinTables, leftJoinTablesMap, table, mainTable);
            }
        }
        fSqlQuery += " from " + fMainTable + " ";
        foreach (QString s, leftJoinTables) {
            fSqlQuery += s + " ";
        }
        if (!fWhereCondition.isEmpty()) {
            if (fWhereCondition.contains("where", Qt::CaseInsensitive)) {
                fSqlQuery += " and " + fWhereCondition;
            } else {
                fSqlQuery += " where " + fWhereCondition;
            }
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
        if (!fHavindCondition.isEmpty()) {
            fSqlQuery += fHavindCondition;
        }
    }
    fModel->translate(fTranslation);
    refreshData();
}

void C5Grid::buildQuery(const QString &query)
{
    fSqlQuery = query;
    buildQuery();
}

void C5Grid::setFilter(int column, const QString &filter)
{
    fModel->setFilter(column, filter);
}

QWidget *C5Grid::widget()
{
    return ui->wd;
}

QHBoxLayout *C5Grid::hl()
{
    return ui->hl;
}

int C5Grid::rowId()
{
    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return 0;
    }
    return fModel->data(ml.at(0).row(), 0, Qt::EditRole).toInt();
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
    Q_UNUSED(id);
}

void C5Grid::removeWithId(int id, int row)
{
    Q_UNUSED(id);
    Q_UNUSED(row);
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

void C5Grid::restoreColumnsWidths()
{
    QSettings s(_ORGANIZATION_, QString("%1\\%2\\reports\\%3\\columnswidth")
                .arg(_APPLICATION_)
                .arg(_MODULE_)
                .arg(fLabel));
    ui->tblTotal->setColumnCount(fModel->columnCount());
    for (int i = 0; i < ui->tblTotal->columnCount(); i++) {
        ui->tblTotal->setItem(0, i, new QTableWidgetItem());
        QString colName = fModel->nameForColumnIndex(i);
        if (s.contains(colName)) {
            ui->tblView->setColumnWidth(i, s.value(colName).toInt());
        }
    }
}

QStringList C5Grid::dbParams()
{
    return fDBParams;
}

QString C5Grid::reportAdditionalTitle()
{
    return "";
}

void C5Grid::insertJoinTable(QStringList &joins, QMap<QString, QString> &joinsMap, const QString &table, const QString &mainTable)
{
    QString j;
    for (int i = 0; i < fLeftJoinTables.count(); i++) {
        QString tmpJoinTable = fLeftJoinTables.at(i);
        int pos = tmpJoinTable.indexOf(QRegExp("\\[.*\\]"));
        tmpJoinTable = tmpJoinTable.mid(pos, tmpJoinTable.length() - pos);
        tmpJoinTable.replace("[", "").replace("]", "");
        if (tmpJoinTable == table) {
            if (!joinsMap.contains(tmpJoinTable)) {
                j = fLeftJoinTables.at(i).mid(0, fLeftJoinTables.at(i).length() - (fLeftJoinTables.at(i).length() - fLeftJoinTables.at(i).indexOf(" [")));
                QRegExp rx("=.*$");
                rx.indexIn(j, 0);
                QString otherTable = rx.cap(0).trimmed();
                otherTable.remove(0, 1);
                otherTable = otherTable.mid(0, otherTable.indexOf(".", 0));
                if (otherTable != mainTable && !otherTable.isEmpty()) {
                    if (!joinsMap.contains(otherTable)) {
                        insertJoinTable(joins, joinsMap, otherTable, mainTable);
                    }
                }
                joinsMap[tmpJoinTable] = j;
                joins << joinsMap[tmpJoinTable];
            }
            break;
        }
    }
}

int C5Grid::sumOfColumnsWidghtBefore(int column)
{
    int sum = 0;
    for (int i = 0; i < column; i++) {
        sum += fTableView->columnWidth(i);
    }
    return sum;
}

void C5Grid::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
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

void C5Grid::removeRow(int columnWithId)
{
    int row = 0;
    QModelIndexList ml = ui->tblView->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        C5Message::info(tr("Nothing was selected"));
        return;
    }
    row = ml.at(0).row();
    removeWithId(fModel->data(row, columnWithId, Qt::EditRole).toInt(), row);
}

void C5Grid::print()
{
    QFont font(qApp->font());
    font.setPointSize(20);
    C5Printing p;
    QSize paperSize(2000, 2800);
    p.setFont(font);
    int page = p.currentPageIndex();
    int startFrom = 0;
    bool stopped = false;
    int columnsWidth = 0;
    qreal scaleFactor = 0.45;
    qreal rowScaleFactor = 0.79;
    for (int i = 0; i < fModel->columnCount(); i++) {
        columnsWidth += fTableView->columnWidth(i);
    }
    columnsWidth /= scaleFactor;
    if (columnsWidth > 2000) {
        p.setSceneParams(paperSize.height(), paperSize.width(), QPrinter::Landscape);
    } else {
        p.setSceneParams(paperSize.width(), paperSize.height(), QPrinter::Portrait);
    }
    do {
        p.setFontBold(true);
        p.ltext(QString("%1 %2")
                .arg(fLabel)
                .arg(reportAdditionalTitle()), 0);
//#ifdef QT_DEBUG
//        p.line(0, p.fTop, 500, p.fTop);
//        p.br();
//        p.line(0, p.fTop, 1000, p.fTop);
//        p.br();
//#endif
        p.br();
        p.setFontBold(false);
        p.line(0, p.fTop, columnsWidth, p.fTop);
        for (int c = 0; c < fModel->columnCount(); c++) {
            if (c > 0) {
                p.ltext(fModel->headerData(c, Qt::Horizontal, Qt::DisplayRole).toString(), (sumOfColumnsWidghtBefore(c) / scaleFactor) + 1);
                p.line(sumOfColumnsWidghtBefore(c) / scaleFactor, p.fTop, sumOfColumnsWidghtBefore(c) / scaleFactor, p.fTop + (fTableView->verticalHeader()->defaultSectionSize() / rowScaleFactor));
            } else {
                p.ltext(fModel->headerData(c, Qt::Horizontal, Qt::DisplayRole).toString(), 1);
                p.line(0, p.fTop, 0, p.fTop + (fTableView->verticalHeader()->defaultSectionSize() / rowScaleFactor));
            }
        }
        p.line(columnsWidth, p.fTop, columnsWidth, p.fTop + (fTableView->verticalHeader()->defaultSectionSize() / rowScaleFactor));
        p.br();
        p.line(0, p.fTop, columnsWidth, p.fTop);
        for (int r = startFrom; r < fModel->rowCount(); r++) {
            p.line(0, p.fTop, columnsWidth, p.fTop);
            for (int c = 0; c < fModel->columnCount(); c++) {
                if (c > 0) {
                    p.ltext(fModel->data(r, c, Qt::DisplayRole).toString(), (sumOfColumnsWidghtBefore(c) / scaleFactor) + 1);
                    p.line(sumOfColumnsWidghtBefore(c) / scaleFactor, p.fTop, sumOfColumnsWidghtBefore(c) / scaleFactor, p.fTop + (fTableView->rowHeight(r) / rowScaleFactor));
                } else {
                    p.ltext(fModel->data(r, c, Qt::DisplayRole).toString(), 1);
                    p.line(0, p.fTop, 0, p.fTop + (fTableView->rowHeight(r) / rowScaleFactor));
                }
            }
            //last vertical line
            p.line(columnsWidth, p.fTop, columnsWidth, p.fTop + (fTableView->rowHeight(r) / rowScaleFactor));
            if (p.checkBr(p.fLineHeight * 4) || r >= fModel->rowCount() - 1) {
                p.line(0, p.fTop + (fTableView->rowHeight(r) / rowScaleFactor), columnsWidth, p.fTop + (fTableView->rowHeight(r) / rowScaleFactor));\
                p.br();
                startFrom = r + 1;
                stopped = startFrom >= fModel->rowCount() - 1;
                p.fTop = p.fNormalHeight - p.fLineHeight;
                p.ltext(QString("%1 %2, %3 %4, %5/%6")
                        .arg("Page")
                        .arg(page + 1)
                        .arg(tr("Printed"))
                        .arg(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR2))
                        .arg(hostinfo)
                        .arg(hostusername()), 0);
                p.rtext(fDBParams.at(1));
                if (r < fModel->rowCount() - 1) {
                    p.br(p.fLineHeight * 4);
                }
                page++;
                break;
            } else {
                //p.line(0, p.fTop + (fTableView->rowHeight(r) / scaleFactor), sumOfColumnsWidghtBefore(fModel->columnCount()) / scaleFactor, p.fTop + (fTableView->rowHeight(r) / scaleFactor));
                p.br();
            }
        }
        if (fModel->rowCount() == 0) {
            stopped = true;
        }
    } while (!stopped);

    C5PrintPreview pp(&p, fDBParams, this);
    pp.exec();
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

void C5Grid::tableViewHeaderClicked(int index)
{
    fModel->sort(index);
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
    fTableView->selectionModel()->clear();
    fModel->saveDataChanges();
    fTableView->viewport()->update();
}

void C5Grid::refreshData()
{
    fModel->execQuery(fSqlQuery);
    restoreColumnsWidths();
    sumColumnsData();
}

void C5Grid::on_tblView_clicked(const QModelIndex &index)
{
    cellClicked(index);
}

void C5Grid::on_tblView_doubleClicked(const QModelIndex &index)
{
    QList<QVariant> values = fModel->getRowValues(index.row());
    emit tblDoubleClicked(index.row(), index.column(), values);
}
