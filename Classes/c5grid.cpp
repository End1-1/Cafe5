#include "c5grid.h"
#include "ui_c5grid.h"
#include "c5tablemodel.h"
#include "c5filtervalues.h"
#include "c5filterwidget.h"
#include "c5gridgilter.h"
#include "c5utils.h"
#include "nloadingdlg.h"
#include "c5message.h"
#include "c5htmlprint.h"
#include <QMenu>
#include <QScrollBar>
#include <QClipboard>
#include <QShortcut>
#include <QFileDialog>
#include <QDesktopServices>
#include <QTextDocument>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QXlsx/header/xlsxdocument.h>

C5Grid::C5Grid(QWidget *parent) :
    C5OfficeWidget(parent),
    ui(new Ui::C5Grid)
{
    ui->setupUi(this);
    fCheckboxes = false;
    fModel = new C5TableModel(ui->tblView);
    ui->tblView->setModel(fModel);
    fSimpleQuery = true;
    fTableView = ui->tblView;
    fTableTotal = ui->tblTotal;
    fTableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(fTableView->horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(tableViewHeaderContextMenuRequested(QPoint)));
    connect(fTableView->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(tableViewHeaderClicked(int)));
    connect(fTableView->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(tableViewHeaderResized(int,
            int, int)));
    connect(fTableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this,
            SLOT(selectionChanged(QItemSelection, QItemSelection)));
    connect(fTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tableViewContextMenuRequested(QPoint)));
    connect(ui->tblTotal->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(tblValueChanged(int)));
    fFilterWidget = nullptr;
    ui->tblTotal->setVisible(false);
    ui->tblView->resizeRowsToContents();
    QShortcut *s = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Enter), this);
    connect(s, SIGNAL(activated()), this, SLOT(ctrlEnter()));
    s = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return), this);
    connect(s, SIGNAL(activated()), this, SLOT(ctrlEnter()));
    fLoadingDlg = new NLoadingDlg(tr("Query"), this);
    connect(this, &C5Grid::refreshed, fLoadingDlg, &NLoadingDlg::hide);
}

C5Grid::~C5Grid()
{
    if(fFilterWidget) {
        delete fFilterWidget;
    }

    delete ui;
}

void C5Grid::setTableForUpdate(const QString &table, const QList<int>& columns)
{
    fModel->fTableForUpdate = table;
    fModel->fColumnsForUpdate = columns;
}

void C5Grid::postProcess()
{
    if(fFilterWidget) {
        fFilterWidget->restoreFilter(fFilterWidget);
        fWhereCondition = fFilterWidget->condition();
    }

    buildQuery();
}

void C5Grid::buildQuery()
{
    fLoadingDlg->resetSeconds();
    fLoadingDlg->open();
    QString mainTable = fMainTable.mid(fMainTable.indexOf(QRegularExpression("\\b[a-z,A-Z]*$")), fMainTable.length() - 1);
    QStringList leftJoinTables;
    QMap<QString, QString> leftJoinTablesMap;
    QStringList groupFields;

    if(fSimpleQuery) {
        if(fSqlQuery.isEmpty()) {
            fLoadingDlg->hide();
            return;
        }
    } else {
        fSqlQuery = "select ";
        bool first = true;

        foreach(QString s, fColumnsFields) {
            if(fColumnsVisible[s]) {
                if(first) {
                    first = false;
                } else {
                    fSqlQuery += ",";
                }

                fSqlQuery += s;
                int pos = -1;

                do {
                    pos = s.indexOf(QRegularExpression("\\b[a-z,A-Z]*\\."), pos + 1);
                    QString tableOfField = s.mid(pos, s.indexOf(".", pos) - pos);

                    if(tableOfField != mainTable) {
                        insertJoinTable(leftJoinTables, leftJoinTablesMap, tableOfField, mainTable);
                    }
                } while(pos > 0);

                if(fColumnsGroup.contains(s)) {
                    pos = s.indexOf(" as");
                    s = s.mid(0, pos);

                    if(!groupFields.contains(s)) {
                        groupFields << s;
                    }
                }
            }
        }

        if(fFilterWidget) {
            fWhereCondition = fFilterWidget->condition();
        }

        int p = 0;
        QRegularExpression re("\\b[a-z]*\\.");
        re.setPatternOptions(QRegularExpression::PatternOption::DotMatchesEverythingOption);

        while(true) {
            QRegularExpressionMatch match = re.match(fWhereCondition, p);

            if(!match.hasMatch()) {
                break;
            }

            QString table = match.captured(0);
            p += match.capturedLength();
            table = table.remove(table.length() - 1, 1);

            if(table != mainTable) {
                insertJoinTable(leftJoinTables, leftJoinTablesMap, table, mainTable);
            }
        }

        fSqlQuery += " from " + fMainTable + " ";

        foreach(QString s, leftJoinTables) {
            fSqlQuery += s + " ";
        }

        if(!fWhereCondition.isEmpty()) {
            if(fSqlQuery.contains("%where%")) {
                fSqlQuery.replace("%where%", fWhereCondition);
            } else {
                if(fWhereCondition.contains("where", Qt::CaseInsensitive)) {
                    fSqlQuery += " and " + fWhereCondition;
                } else {
                    fSqlQuery += " where " + fWhereCondition;
                }
            }
        }

        if(groupFields.count() > 0) {
            first = true;
            fSqlQuery += " group by ";

            foreach(QString s, groupFields) {
                if(first) {
                    first = false;
                } else {
                    fSqlQuery += ",";
                }

                fSqlQuery += s;
            }
        }

        fSqlQuery += fOrderCondition;

        foreach(const QString &o, fColumnsOrder) {
            if(fColumnsVisible[o]) {
                if(fSqlQuery.contains("order by ", Qt::CaseInsensitive)) {
                    fSqlQuery += ",";
                } else {
                    fSqlQuery += " order by ";
                }

                fSqlQuery += columnName(o);
            }
        }

        if(!fHavindCondition.isEmpty()) {
            fSqlQuery += fHavindCondition;
        }
    }

    fModel->translate(fTranslation);
    refreshData();
    emit refreshed();
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
    if(key == "Ctrl++") {
        fTableView->selectAll();
        return;
    }

    C5Widget::hotKey(key);
}

void C5Grid::setCheckboxes(bool v)
{
    fCheckboxes = v;
}

void C5Grid::setSimpleQuery(const QString &sql)
{
    fSimpleQuery = true;
    fSqlQuery = sql;
    fOrderCondition.clear();
    refreshData();

    for(auto *a : fToolBar->actions()) {
        if(a->property("filter").toBool()) {
            a->setVisible(false);
        }
    }
}

bool C5Grid::on_tblView_doubleClicked(const QModelIndex &index)
{
    if(index.row() < 0 || index.column() < 0) {
        return false;
    }

    QJsonArray values = fModel->getRowValues(index.row());

    if(tblDoubleClicked(index.row(), index.column(), values)) {
        return false;
    }

    return true;
}

QWidget* C5Grid::widget()
{
    return ui->wd;
}

QHBoxLayout* C5Grid::hl()
{
    return ui->hl;
}

int C5Grid::rowId()
{
    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();

    if(ml.count() == 0) {
        return 0;
    }

    return fModel->data(ml.at(0).row(), 0, Qt::EditRole).toInt();
}

int C5Grid::rowId(int column)
{
    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();

    if(ml.count() == 0) {
        C5Message::info(tr("Nothing was selected"));
        return 0;
    }

    return fModel->data(ml.at(0).row(), column, Qt::EditRole).toInt();
}

int C5Grid::rowId(int& row, int column)
{
    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();

    if(ml.count() == 0) {
        C5Message::info(tr("Nothing was selected"));
        return 0;
    }

    row = ml.at(0).row();
    return fModel->data(row, column, Qt::EditRole).toInt();
}

void C5Grid::cellClicked(const QModelIndex &index)
{
    emit tblSingleClick(index);
}

void C5Grid::callEditor(const QString &id)
{
    Q_UNUSED(id);
}

void C5Grid::removeWithId(int id, int row)
{
    Q_UNUSED(id);
    fModel->removeRow(row);
}

void C5Grid::sumColumnsData()
{
    if(fColumnsSum.count() == 0 && fColumnsSumIndex.count() == 0) {
        return;
    }

    if(fColumnsSum.count() > 0) {
        QMap<QString, double> values;
        fModel->sumForColumns(fColumnsSum, values);

        for(QMap<QString, double>::const_iterator it = values.begin(); it != values.end(); it++) {
            int idx = fModel->indexForColumnName(it.key());
            double value = it.value();
            ui->tblTotal->setData(0, idx, value);
        }
    }

    if(fColumnsSumIndex.count() > 0) {
        QMap<int, double> values;
        fModel->sumForColumnsIndexes(fColumnsSumIndex, values);

        for(int idx : fColumnsSumIndex) {
            ui->tblTotal->setData(0, idx, values[idx]);
        }
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

    for(QMap<QString, bool>::iterator it = fColumnsVisible.begin(); it != fColumnsVisible.end(); it++) {
        if(s.contains(it.key())) {
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

    for(int i = 0; i < ui->tblTotal->columnCount(); i++) {
        ui->tblTotal->setItem(0, i, new C5TableWidgetItem());
        QString colName = fModel->nameForColumnIndex(i);

        if(colName.isEmpty()) {
            continue;
        }

        QString fullColName = colName;

        for(QMap<QString, bool>::const_iterator it = fColumnsVisible.begin(); it != fColumnsVisible.end(); it++) {
            QString c = it.key().toLower();

            if(c.contains(" as ")) {
                int pos = c.indexOf(" as ");

                if(fullColName == c.mid(pos + 4, c.length() - pos)) {
                    fullColName = c;
                    break;
                }
            } else if(c.contains(".")) {
                int pos = c.indexOf(".");

                if(fullColName == c.mid(pos + 1, c.length() - pos)) {
                    fullColName = c;
                    break;
                }
            } else if(c == fullColName) {
                fullColName = c;
                break;
            }
        }

        if(!fColumnsVisible[fullColName] || ui->tblView->columnWidth(i) == 0) {
            ui->tblView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
            //continue; why this row was commented, i dont know
        }

        if(s.contains(colName)) {
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

QString C5Grid::reportAdditionalTitle()
{
    if(fFilterWidget) {
        return fFilterWidget->conditionText();
    }

    return "";
}

QMenu* C5Grid::buildTableViewContextMenu(const QPoint &point)
{
    QModelIndex index = fTableView->indexAt(point);
    QMenu *m = new QMenu(this);

    if(index.row() > -1 && index.column() > -1) {
        fFilterString = fModel->data(index, Qt::DisplayRole).toString();
        fFilterIndex = index;
        m->addAction(QIcon(":/filter_set.png"), QString("%1 '%2'").arg(tr("Filter")).arg(fFilterString), this,
                     SLOT(filterByStringAndIndex()));
    }

    m->addAction(QIcon(":/copy.png"), tr("Copy selection"), this, SLOT(copySelection()));
    m->addAction(QIcon(":/copy.png"), tr("Copy all"), this, SLOT(copyAll()));
    return m;
}

bool C5Grid::tblDoubleClicked(int row, int column, const QJsonArray &values)
{
    emit tblDoubleClick(row, column, values);
    return false;
}

void C5Grid::executeSql(const QString &sql)
{
    fModel->execQuery(sql, this);

    if(fSimpleQuery) {
        for(int i = 0; i < fModel->columnCount(); i++) {
            fColumnsVisible[fModel->fColumnIndexName[i]] = true;
        }
    }

    restoreColumnsWidths();
    sumColumnsData();

    if(!ui->tblTotal->isVisible()) {
        ui->tblView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }

    completeRefresh();
}

void C5Grid::completeRefresh()
{
    fTableView->clearSpans();
}

void C5Grid::insertJoinTable(QStringList &joins, QMap<QString, QString>& joinsMap, const QString &table,
                             const QString &mainTable)
{
    QString j;

    for(int i = 0; i < fLeftJoinTables.count(); i++) {
        QString tmpJoinTable = fLeftJoinTables.at(i);
        int pos = tmpJoinTable.indexOf(QRegularExpression("\\[.*\\]"));
        tmpJoinTable = tmpJoinTable.mid(pos, tmpJoinTable.length() - pos);
        tmpJoinTable.replace("[", "").replace("]", "");

        if(tmpJoinTable == table) {
            if(!joinsMap.contains(tmpJoinTable)) {
                j = fLeftJoinTables.at(i).mid(0, fLeftJoinTables.at(i).length() - (fLeftJoinTables.at(i).length() - fLeftJoinTables.at(
                                                  i).indexOf(" [")));
                QRegularExpression rx("=.*$");
                QRegularExpressionMatch match = rx.match(j, 0);
                QString otherTable = match.captured(0).trimmed();
                otherTable.remove(0, 1);
                otherTable = otherTable.mid(0, otherTable.indexOf(".", 0));

                if(otherTable != mainTable && !otherTable.isEmpty()) {
                    if(!joinsMap.contains(otherTable)) {
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

    for(int i = 0; i < column; i++) {
        sum += fTableView->columnWidth(i);
    }

    return sum;
}

QString C5Grid::columnName(const QString &s) const
{
    int pos = s.indexOf(" as");

    if(pos > -1) {
        return s.mid(0, pos);
    } else {
        return s;
    }
}

void C5Grid::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
}

void C5Grid::copySelection()
{
    QModelIndexList sel = fTableView->selectionModel()->selectedIndexes();

    if(sel.count() == 0) {
        return;
    }

    QString data;
    int currCol = -1;
    bool first = true;

    foreach(QModelIndex m, sel) {
        if(currCol < 0) {
            currCol = m.row();
        }

        if(currCol != m.row()) {
            currCol = m.row();
            first = true;
            data += "\r\n";
        }

        if(first) {
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

    for(int i = 0; i < fModel->columnCount(); i++) {
        data += fModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();

        if(i < fModel->columnCount() - 1) {
            data += "\t";
        } else {
            data += "\r\n";
        }
    }

    for(int r = 0, rc = fModel->rowCount(); r < rc; r++) {
        for(int c = 0, cc = fModel->columnCount(); c < cc; c++) {
            data += fModel->data(r, c, Qt::DisplayRole).toString();

            if(c == cc - 1) {
                data += "\r\n";
            } else {
                data += "\t";
            }
        }
    }

    if(ui->tblTotal->isVisible()) {
        for(int i = 0; i < ui->tblTotal->columnCount(); i++) {
            data += ui->tblTotal->getString(0, i);

            if(i < fModel->columnCount() - 1) {
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
    QStringList sortedValues = filterValues.values();
    std::sort(sortedValues.begin(), sortedValues.end());

    if(C5FilterValues::filterValues(sortedValues, mUser)) {
        if(fModel->fColumnType[fFilterColumn] == QMetaType::Double) {
            for(int i = 0; i < sortedValues.length(); i++) {
                sortedValues[i] = QVariant(str_float(sortedValues[i])).toString();
            }
        }

        fModel->setFilter(fFilterColumn, sortedValues.join("|"));
    }

    sumColumnsData();
}

void C5Grid::filterByStringAndIndex()
{
    if(fFilterString.isEmpty()) {
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

bool C5Grid::currentRow(int& row)
{
    row = -1;
    QModelIndexList ml = ui->tblView->selectionModel()->selectedIndexes();

    if(ml.count() == 0) {
        C5Message::info(tr("Nothing was selected"));
        return false;
    }

    row = ml.at(0).row();
    return true;
}

void C5Grid::editRow(int columnWidthId)
{
    int row = 0;

    if(!currentRow(row)) {
        return;
    }

    callEditor(fModel->data(row, columnWidthId, Qt::EditRole).toString());
}

void C5Grid::removeRow(int columnWithId)
{
    int row = 0;

    if(!currentRow(row)) {
        return;
    }

    removeWithId(fModel->data(row, columnWithId, Qt::EditRole).toInt(), row);
}

QString  C5Grid::makeColumns()
{
    QString c;
    QTextStream s(&c);

    for(int i = 0; i < fModel->columnCount(); ++i) {
        int w = fTableView->columnWidth(i);

        if(w == 0)
            continue;

        s << "<col style='width:" << w << "px'>";
    }

    return c;
}

QString C5Grid::makeHeaders()
{
    QString h;
    QTextStream s(&h);

    for(int c = 0; c < fModel->columnCount(); ++c) {
        if(fTableView->columnWidth(c) == 0)
            continue;

        s << "<th>"
          << fModel->headerData(c, Qt::Horizontal, Qt::DisplayRole).toString()
          << "</th>";
    }

    return h;
}

QString C5Grid::makeRows()
{
    QString r;
    QTextStream s(&r);

    for(int row = 0; row < fModel->rowCount(); ++row) {
        s << "<tr>";

        for(int col = 0; col < fModel->columnCount(); ++col) {
            if(fTableView->columnWidth(col) == 0)
                continue;

            QVariant v = fModel->data(row, col, Qt::EditRole);
            QFont f = fModel->data(row, col, Qt::FontRole).value<QFont>();
            s << "<td"
              << (f.bold() ? " style='font-weight:bold'" : "")
              << ">"
              << v.toString()
              << "</td>";
        }

        s << "</tr>";
    }

    return r;
}

void C5Grid::print()
{
    QString html = loadTemplate("grid_a4.html");
    QMap<QString, QString> vars;
    vars["title"] = fLabel;
    vars["filter"] = fFilterWidget
                     ? "<div>" + fFilterWidget->filterText() + "</div>"
                     : "";
    vars["additional_title"] = reportAdditionalTitle().isEmpty()
                               ? ""
                               : "<div>" + reportAdditionalTitle() + "</div>";
    vars["columns"] = makeColumns();
    vars["headers"] = makeHeaders();
    vars["rows"] = makeRows();
    vars["page_orientation"] = "A4 landscape";
    // vars["page_orientation"] =
    //     columnsWidth > paperSize.width()
    //     ? "A4 landscape"
    //     : "A4 portrait";
    vars["footer_left"] =
        QString("Page %1, %2")
        .arg(1)
        .arg(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR2));
    vars["footer_right"] =
        hostinfo + "/" + hostusername();
    html = applyTemplate(html, vars);
    QTextDocument doc;
    doc.setHtml(html);
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize::A4);
    printer.setFullPage(false);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, [&](QPrinter * p) { doc.print(p); });
    preview.exec();
}

void C5Grid::exportToExcel()
{
    int colCount = fModel->columnCount();
    int rowCount = fModel->rowCount();

    if(colCount == 0 || rowCount == 0) {
        C5Message::info(tr("Empty report!"));
        return;
    }

    QXlsx::Document d;
    d.addSheet("Sheet1");
    /* HEADER */
    QColor color = QColor::fromRgb(200, 200, 250);
    QFont headerFont(qApp->font());
    headerFont.setBold(true);
    QXlsx::Format hf;
    hf.setFont(headerFont);
    hf.setBorderStyle(QXlsx::Format::BorderThin);
    hf.setPatternBackgroundColor(color);

    for(int i = 0; i < colCount; i++) {
        d.write(1, i + 1, fModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString(), hf);
        d.setColumnWidth(i + 1, fTableView->columnWidth(i) / 7);
    }

    /* BODY */
    QFont bodyFont(qApp->font());
    QXlsx::Format bf;
    bf.setFont(bodyFont);
    bf.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    bf.setBorderStyle(QXlsx::Format::BorderThin);

    for(int j = 0; j < rowCount; j++) {
        for(int i = 0; i < colCount; i++) {
            if(fModel->data(j, i, Qt::EditRole).typeId() == QMetaType::Double) {
                if(fModel->data(j, i, Qt::EditRole).toDouble() == 0) {
                    // continue;
                }
            }

            d.write(j + 2, i + 1, fModel->data(j, i, Qt::EditRole), bf);
        }
    }

    /* MERGE cells */
    QMap<int, QList<int> > skiprow, skipcol;

    for(int r = 0; r < rowCount; r++) {
        for(int c = 0; c < colCount; c++) {
            if(fTableView->columnSpan(r, c) > 1 || fTableView->rowSpan(r, c) > 1) {
                int rs = -1, cs = -1;

                if(fTableView->columnSpan(r, c) > 1 && skipcol[r].contains(c) == false) {
                    cs = fTableView->columnSpan(r, c) - 1;

                    for(int i = c + 1; i < c + cs + 1; i++) {
                        skipcol[r].append(i);
                    }
                }

                if(fTableView->rowSpan(r, c) > 1 && skiprow[c].contains(r) == false) {
                    rs = fTableView->rowSpan(r, c) - 1;

                    for(int i = r + 1; i < r + rs + 1; i++) {
                        skiprow[c].append(i);
                    }
                }

                if(rs == -1 && cs == -1) {
                    continue;
                }

                rs = rs < 0 ? 0 : rs;
                cs = cs < 0 ? 0 : cs;
                d.mergeCells(QString("%1%2:%3:%4")
                             .arg(columnNumberToLetter(c + 1))
                             .arg(r + 2)
                             .arg(c + 1 + cs)
                             .arg(r + 2 + rs));
            }
        }
    }

    /* TOTALS ROWS */
    if(ui->tblTotal->isVisible()) {
        for(int i = 0; i < colCount; i++) {
            d.write(1 + fModel->rowCount() + 1, i + 1, ui->tblTotal->getData(0, i), hf);
        }
    }

    QString filename = QFileDialog::getSaveFileName(nullptr, "", "", "*.xlsx");

    if(filename.isEmpty()) {
        return;
    }

    d.saveAs(filename);
    QDesktopServices::openUrl(filename);
}

void C5Grid::clearFilter()
{
    if(fFilterWidget) {
        fFilterWidget->clearFilter(fFilterWidget);
        fFilterWidget->saveFilter(fFilterWidget);
    }

    fModel->clearFilter();
    sumColumnsData();
}

void C5Grid::setSearchParameters()
{
    if(fFilterWidget) {
        if(C5GridGilter::filter(mUser, fFilterWidget, fWhereCondition, fColumnsVisible, fTranslation)) {
            QSettings s(_ORGANIZATION_, QString("%1\\%2\\reports\\%3\\visiblecolumns")
                        .arg(_APPLICATION_)
                        .arg(_MODULE_)
                        .arg(fLabel));

            for(QMap<QString, bool>::const_iterator it = fColumnsVisible.begin(); it != fColumnsVisible.end(); it++) {
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
    m.addAction(QIcon(":/filter_set.png"), QString("%1 '%2'").arg(tr("Set filter")).arg(colName), this,
                SLOT(filterByColumn()));
    m.addAction(QIcon(":/filter_clear.png"), QString("%1 '%2'").arg(tr("Remove filter")).arg(colName), this,
                SLOT(removeFilterForColumn()));
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
    QString sqlQuery = fSqlQuery;

    if(fSimpleQuery) {
        if(fFilterWidget) {
            fWhereCondition = fFilterWidget->condition();
        }

        if(!fWhereCondition.isEmpty()) {
            if(sqlQuery.contains("%where%")) {
                sqlQuery.replace("%where%", fWhereCondition);
            } else if(fSqlQuery.contains("where")) {
                if(fWhereCondition.contains("where")) {
                    fWhereCondition.replace("where", " ");
                }

                sqlQuery += " and " + fWhereCondition;
            } else {
                if(!fWhereCondition.contains("where", Qt::CaseInsensitive)) {
                    fWhereCondition = " where " + fWhereCondition;
                }

                sqlQuery += fWhereCondition;
            }
        }

        if(!fGroupCondition.isEmpty()) {
            sqlQuery += fGroupCondition;
        }

        if(!fHavindCondition.isEmpty()) {
            sqlQuery += fHavindCondition;
        }

        if(!fOrderCondition.isEmpty()) {
            sqlQuery += fOrderCondition;
        }
    }

    fModel->setCheckboxes(fCheckboxes);
    executeSql(sqlQuery);
}

void C5Grid::on_tblView_clicked(const QModelIndex &index)
{
    cellClicked(index);
}

void C5Grid::sqlError(const QString &errorMessage)
{
    C5Message::error(errorMessage);
}
