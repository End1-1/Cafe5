#include "c5grid.h"
#include "ui_c5grid.h"
#include "c5tablemodel.h"
#include "c5filtervalues.h"
#include "c5filterwidget.h"
#include "c5printing.h"
#include "c5printpreview.h"
#include "c5gridgilter.h"
#include "c5editor.h"
#include "ce5editor.h"
#include "xlsxall.h"
#include <QMenu>
#include <QScrollBar>
#include <QClipboard>
#include <QShortcut>

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
    fTableTotal = ui->tblTotal;
    fTableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(fTableView->horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tableViewHeaderContextMenuRequested(QPoint)));
    connect(fTableView->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(tableViewHeaderClicked(int)));
    connect(fTableView->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(tableViewHeaderResized(int,int,int)));
    connect(fTableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChanged(QItemSelection,QItemSelection)));
    connect(fTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tableViewContextMenuRequested(QPoint)));
    connect(ui->tblTotal->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(tblValueChanged(int)));
    fFilterWidget = nullptr;
    fEditor = nullptr;
    ui->tblTotal->setVisible(false);
    ui->tblView->resizeRowsToContents();
    QShortcut *s = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Enter), this);
    connect(s, SIGNAL(activated()), this, SLOT(ctrlEnter()));
    s = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return), this);
    connect(s, SIGNAL(activated()), this, SLOT(ctrlEnter()));
}

C5Grid::~C5Grid()
{
    if (fFilterWidget) {
        delete fFilterWidget;
    }
    if (fEditor) {
        delete fEditor;
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
    if (fFilterWidget) {
        fFilterWidget->restoreFilter(fFilterWidget);
        fWhereCondition = fFilterWidget->condition();
    }
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
        fSqlQuery += fOrderCondition;
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

void C5Grid::hotKey(const QString &key)
{
    if (key == "Ctrl++") {
        fTableView->selectAll();
        return;
    }
    C5Widget::hotKey(key);
}

void C5Grid::changeDatabase(const QStringList &dbParams)
{
    C5Widget::changeDatabase(dbParams);
    fDb.close();
    fDb.setDatabase(dbParams.at(0), dbParams.at(1), dbParams.at(2), dbParams.at(3));
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

void C5Grid::callEditor(const QString &id)
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
        double value = it.value();
        ui->tblTotal->setData(0, idx, value);
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
        ui->tblTotal->setItem(0, i, new C5TableWidgetItem());
        QString colName = fModel->nameForColumnIndex(i);
        QString fullColName = colName;
        for (QMap<QString, bool>::const_iterator it = fColumnsVisible.begin(); it != fColumnsVisible.end(); it++) {
            QString c = it.key().toLower();
            if (c.contains(" as ")) {
                int pos = c.indexOf(" as ");
                if (fullColName == c.mid(pos + 4, c.length() - pos)) {
                    fullColName = c;
                    break;
                }
            } else if (c.contains(".")) {
                int pos = c.indexOf(".");
                if (fullColName == c.mid(pos + 1, c.length() - pos)) {
                    fullColName = c;
                    break;
                }
            } else if (c == fullColName) {
                fullColName = c;
                break;
            }
        }
        if (!fColumnsVisible[fullColName] || ui->tblView->columnWidth(i) == 0) {
            ui->tblView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
            //continue; why this row was commented, i dont know
        }
        if (s.contains(colName)) {
            ui->tblView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
            int w = s.value(colName).toInt();
            ui->tblView->setColumnWidth(i, w);
        } else {
            ui->tblView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
            //qApp->processEvents();
            ui->tblView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
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

QMenu *C5Grid::buildTableViewContextMenu(const QPoint &point)
{
    QModelIndex index = fTableView->indexAt(point);
    QMenu *m = new QMenu(this);
    if (index.row() > -1 && index.column() > -1) {
        fFilterString = fModel->data(index, Qt::DisplayRole).toString();
        fFilterIndex = index;
        m->addAction(QIcon(":/filter_set.png"), QString("%1 '%2'").arg(tr("Filter")).arg(fFilterString), this, SLOT(filterByStringAndIndex()));
    }
    m->addAction(QIcon(":/copy.png"), tr("Copy selection"), this, SLOT(copySelection()));
    m->addAction(QIcon(":/copy.png"), tr("Copy all"), this, SLOT(copyAll()));
    return m;
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

void C5Grid::copySelection()
{
    QModelIndexList sel = fTableView->selectionModel()->selectedIndexes();
    if (sel.count() == 0) {
        return;
    }
    QString data;
    int currCol = -1;
    bool first = true;
    foreach (QModelIndex m, sel) {
        if (currCol < 0) {
            currCol = m.row();
        }
        if (currCol != m.row()) {
            currCol = m.row();
            first = true;
            data += "\r\n";
        }
        if (first) {
            first = false;
        } else {
            data += "\t";
        }
        data += m.data(Qt::DisplayRole).toString();
    }
    QClipboard *c = qApp->clipboard();
    c->setText(data);
}

void C5Grid::copyAll()
{
    QString data;
    for (int i = 0; i < fModel->columnCount(); i++) {
        data += fModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
        if (i < fModel->columnCount() - 1) {
            data += "\t";
        } else {
            data += "\r\n";
        }
    }
    for (int r = 0, rc = fModel->rowCount(); r < rc; r++) {
        for (int c = 0, cc = fModel->columnCount(); c < cc; c++) {
            data += fModel->data(r, c, Qt::DisplayRole).toString();
            if (c == cc - 1) {
                data += "\r\n";
            } else {
                data += "\t";
            }
        }
    }
    if (ui->tblTotal->isVisible()) {
        for (int i = 0; i < ui->tblTotal->columnCount(); i++) {
            data += ui->tblTotal->getString(0, i);
            if (i < fModel->columnCount() - 1) {
                data += "\t";
            } else {
                data += "\r\n";
            }
        }
    }
    QClipboard *c = qApp->clipboard();
    c->setText(data);
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

void C5Grid::filterByStringAndIndex()
{
    if (fFilterString.isEmpty()) {
        return;
    }
    fModel->setFilter(fFilterIndex.column(), fFilterString);
    fFilterString.clear();
    sumColumnsData();
}

void C5Grid::removeFilterForColumn()
{
    fModel->removeFilter(fFilterColumn);
}

void C5Grid::tblValueChanged(int pos)
{
    fTableView->horizontalScrollBar()->setValue(pos);
}

int C5Grid::newRow()
{
    if (fEditor == nullptr) {
        return -1;
    }
    C5Editor *e = C5Editor::createEditor(fDBParams, fEditor, 0);
    QList<QMap<QString, QVariant> > data;
    bool yes = e->getResult(data);
    fEditor->setParent(nullptr);
    delete e;
    if (!yes) {
        return -1;
    }
    int row = 0;
    QModelIndexList ml = ui->tblView->selectionModel()->selectedIndexes();
    if (ml.count() > 0) {
        row = ml.at(0).row();
    } else {
        row = fModel->rowCount();
    }
    for (int i = 0; i < data.count(); i++) {
        fModel->insertRow(row);
        if (ml.count() > 0) {
            row++;
        }
        for (QMap<QString, QVariant>::const_iterator it = data.at(i).begin(); it != data.at(i).end(); it++) {
            int col = fModel->indexForColumnName(it.key());
            if (col > -1) {
                fModel->setData(row, col, it.value());
            }
        }
    }
    ui->tblView->setCurrentIndex(fModel->index(row + 1, 0));
    return row;
}

bool C5Grid::currentRow(int &row)
{
    row = -1;
    QModelIndexList ml = ui->tblView->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        C5Message::info(tr("Nothing was selected"));
        return false;
    }
    row = ml.at(0).row();
    return true;
}

void C5Grid::editRow(int columnWidthId)
{
    int row = 0;
    if (!currentRow(row)) {
        return;
    }
    callEditor(fModel->data(row, columnWidthId, Qt::EditRole).toString());
}

void C5Grid::removeRow(int columnWithId)
{
    int row = 0;
    if (!currentRow(row)) {
        return;
    }
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
    qreal scaleFactor = 0.40;
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
        p.br();
        p.setFontBold(false);
        p.line(0, p.fTop, columnsWidth, p.fTop);
        for (int c = 0; c < fModel->columnCount(); c++) {
            if (fTableView->columnWidth(c) == 0) {
                continue;
            }
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
                if (fTableView->columnWidth(c) == 0) {
                    continue;
                }
                int s = fTableView->columnSpan(r, c) - 1;
                if (c > 0) {
                    p.ltext(fModel->data(r, c, Qt::DisplayRole).toString(), (sumOfColumnsWidghtBefore(c) / scaleFactor) + 1);
                    p.line(sumOfColumnsWidghtBefore(c + s) / scaleFactor, p.fTop, sumOfColumnsWidghtBefore(c + s) / scaleFactor, p.fTop + (fTableView->rowHeight(r) / rowScaleFactor));
                } else {
                    p.ltext(fModel->data(r, c, Qt::DisplayRole).toString(), 1);
                    p.line(0, p.fTop, 0, p.fTop + (fTableView->rowHeight(r) / rowScaleFactor));
                }
                c += s;
            }
            //last vertical line
            p.line(columnsWidth, p.fTop, columnsWidth, p.fTop + (fTableView->rowHeight(r) / rowScaleFactor));
            if (ui->tblTotal->isVisible() && r == fModel->rowCount() - 1) {
                p.setFontBold(true);
                if (p.checkBr(ui->tblTotal->rowHeight(0))) {
                    p.line(0, p.fTop + (ui->tblTotal->rowHeight(0) / rowScaleFactor), columnsWidth, p.fTop + (ui->tblTotal->rowHeight(0) / rowScaleFactor));\
                    p.br();
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
                    page++;
                } else {
                    p.br();
                }
                p.line(0, p.fTop, columnsWidth, p.fTop);
                p.line(0, p.fTop + (ui->tblTotal->rowHeight(0) / rowScaleFactor), columnsWidth, p.fTop + (ui->tblTotal->rowHeight(0) / rowScaleFactor));\
                for (int c = 0; c < fModel->columnCount(); c++) {
                    if (fTableView->columnWidth(c) == 0) {
                        continue;
                    }
                    if (c > 0) {
                        p.ltext(ui->tblTotal->getString(0, c), (sumOfColumnsWidghtBefore(c) / scaleFactor) + 1);
                        p.line(sumOfColumnsWidghtBefore(c) / scaleFactor, p.fTop, sumOfColumnsWidghtBefore(c) / scaleFactor, p.fTop + (fTableView->verticalHeader()->defaultSectionSize() / rowScaleFactor));
                    } else {
                        p.ltext(ui->tblTotal->getString(0, c), 1);
                        p.line(0, p.fTop, 0, p.fTop + (fTableView->verticalHeader()->defaultSectionSize() / rowScaleFactor));
                    }
                }
                p.line(columnsWidth, p.fTop, columnsWidth, p.fTop + (ui->tblTotal->rowHeight(0) / rowScaleFactor));
            }
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
                if (r < fModel->rowCount() - 1) {
                    page++;
                }
                break;
            } else {
                p.br();
            }
        }
        if (fModel->rowCount() == 0) {
            stopped = true;
        }
    } while (!stopped);

    C5PrintPreview pp(&p, fDBParams);
    pp.exec();
}

void C5Grid::exportToExcel()
{
    int colCount = fModel->columnCount();
    int rowCount = fModel->rowCount();
    if (colCount == 0 || rowCount == 0) {
        C5Message::info(tr("Empty report!"));
        return;
    }
    XlsxDocument d;
    XlsxSheet *s = d.workbook()->addSheet("Sheet1");
    /* HEADER */
    QColor color = QColor::fromRgb(200, 200, 250);
    QFont headerFont(qApp->font());
    headerFont.setBold(true);
    d.style()->addFont("header", headerFont);
    d.style()->addBackgrounFill("header", color);
    for (int i = 0; i < colCount; i++) {
        s->addCell(1, i + 1, fModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString(), d.style()->styleNum("header"));
        s->setColumnWidth(i + 1, fTableView->columnWidth(i) / 7);
    }
    //e.setHorizontalAlignment(e.address(0, 0), e.address(0, colCount - 1), Excel::hCenter);
    /* BODY */
    QFont bodyFont(qApp->font());
    d.style()->addFont("body", bodyFont);
    for (int j = 0; j < rowCount; j++) {
        for (int i = 0; i < colCount; i++) {
            s->addCell(j + 2, i + 1, fModel->data(j, i, Qt::EditRole), d.style()->styleNum("body"));
        }
    }
    /* TOTALS ROWS */
    if (ui->tblTotal->isVisible()) {
        QFont totalFont(qApp->font());
        totalFont.setBold(true);
        d.style()->addFont("footer", headerFont);
        color = QColor::fromRgb(193, 206, 221);
        d.style()->addBackgrounFill("footer", color);
        for (int i = 0; i < colCount; i++) {
            s->addCell(1 + fModel->rowCount() + 1, i + 1, ui->tblTotal->getData(0, i), d.style()->styleNum("footer"));
        }
    }
    QString err;
    if (!d.save(err, true)) {
        if (!err.isEmpty()) {
            C5Message::error(err);
        }
    }
}

void C5Grid::clearFilter()
{
    fModel->clearFilter();
    sumColumnsData();
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
    QMenu *m = buildTableViewContextMenu(point);
    m->exec(fTableView->mapToGlobal(point));
    delete m;
}

void C5Grid::tableViewHeaderContextMenuRequested(const QPoint &point)
{
    fFilterColumn = fTableView->columnAt(point.x());
    QString colName = fModel->headerData(fFilterColumn, Qt::Horizontal, Qt::DisplayRole).toString();
    QMenu m;
    m.addAction(QIcon(":/filter_set.png"), QString("%1 '%2'").arg(tr("Set filter")).arg(colName), this, SLOT(filterByColumn()));
    m.addAction(QIcon(":/filter_clear.png"), QString("%1 '%2'").arg(tr("Remove filter")).arg(colName), this, SLOT(removeFilterForColumn()));
    m.addAction(QIcon(":/expand.png"), tr("Autofit columns widths"), this, SLOT(autofitColumns()));
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

void C5Grid::ctrlEnter()
{
    QModelIndex m = ui->tblView->currentIndex();
    on_tblView_doubleClicked(m);
}

void C5Grid::autofitColumns()
{
    fTableView->resizeColumnsToContents();
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
    if (fSimpleQuery) {
        for (int i = 0; i < fModel->columnCount(); i++) {
            fColumnsVisible[fModel->fColumnIndexName[i]] = true;
        }
    }
    restoreColumnsWidths();
    sumColumnsData();
    if (!ui->tblTotal->isVisible()) {
        ui->tblView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
}

void C5Grid::on_tblView_clicked(const QModelIndex &index)
{
    cellClicked(index);
}

bool C5Grid::on_tblView_doubleClicked(const QModelIndex &index)
{
    if (index.row() < 0 || index.column() < 0) {
        return false;
    }
    QList<QVariant> values = fModel->getRowValues(index.row());
    if (fEditor) {
        if (values.count() > 0) {
            C5Editor *e = C5Editor::createEditor(fDBParams, fEditor, values.at(0).toInt());
            QList<QMap<QString, QVariant> > data;
            bool yes = e->getResult(data);
            fEditor->setParent(nullptr);
            delete e;
            if (!yes) {
                return false;
            }
            int row = index.row();
            for (int i = 0; i < data.count(); i++) {
                if (i > 0) {
                    fModel->insertRow(row);
                    row++;
                }
                for (QMap<QString, QVariant>::const_iterator it = data.at(i).begin(); it != data.at(i).end(); it++) {
                    int col = fModel->indexForColumnName(it.key());
                    if (col > -1) {
                        fModel->setData(row, col, it.value());
                    }
                }
            }
        }
    }
    emit tblDoubleClicked(index.row(), index.column(), values);
    return fEditor != nullptr;
}
