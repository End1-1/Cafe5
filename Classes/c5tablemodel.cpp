#include "c5tablemodel.h"
#include "c5utils.h"
#include "c5database.h"
#include <QIcon>
#include <QTableView>

C5TableModel::C5TableModel(const QStringList &dbParams, QObject *parent) :
    QAbstractTableModel(parent),
    fDBParams(dbParams)
{
    fTableView = dynamic_cast<QTableView*>(parent);
    fSortAsc = true;
    fLastSortedColumn = -1;
    fCheckboxes = false;
    fSingleCheckBoxSelection = true;
}

void C5TableModel::translate(const QHash<QString, QString> &t)
{
    fTranslateColumn = t;
}

void C5TableModel::execQuery(const QString &query)
{
    beginResetModel();
    clearModel();
    C5Database db(fDBParams);
    if (db.exec(query, fRawData, fColumnNameIndex)) {
        for (int i = 0, count = fRawData.count(); i < count; i++) {
            if (fCheckboxes) {
                fRawData[i].insert(0, QVariant());
            }
            fProxyData << i;
        }
        for (QHash<QString, int>::const_iterator it = fColumnNameIndex.begin(); it != fColumnNameIndex.end(); it++) {
            fColumnIndexName[it.value()] = it.key();
        }
        if (fCheckboxes) {
            for (QHash<QString, int>::iterator it = fColumnNameIndex.begin(); it != fColumnNameIndex.end(); it++) {
                QString k = it.key();
                int v = it.value() + 1;
                fColumnNameIndex[k] = v;
                fColumnIndexName[v] = k;
            }
            fColumnNameIndex["X"] = 0;
            fColumnIndexName[0] = "X";
        }
    } else {

    }
    endResetModel();
}

void C5TableModel::setCheckboxes(bool v)
{
    fCheckboxes = v;
}

void C5TableModel::setSingleCheckBoxSelection(bool v)
{
    fSingleCheckBoxSelection = v;
}

void C5TableModel::sort(int column)
{
    if (fLastSortedColumn == column) {
        fSortAsc = !fSortAsc;
    } else {
        fSortAsc = true;
    }
    fLastSortedColumn = column;
    QMap<QVariant, int> data;
    foreach (int i, fProxyData) {
        data.insertMulti(fRawData[i][column], i);
    }
    beginResetModel();
    fProxyData = data.values();
    if (!fSortAsc) {
        std::reverse(fProxyData.begin(), fProxyData.end());
    }
    endResetModel();
}

void C5TableModel::setExternalData(const QHash<QString, int> &columnNameIndex, const QHash<QString, QString> &translation)
{
    beginResetModel();
    fProxyData.clear();
    fColumnNameIndex.clear();
    fColumnIndexName.clear();
    fRowToUpdate.clear();
    fAddDataToUpdate.clear();
    fColorData.clear();
    fFilters.clear();
    fColumnNameIndex = columnNameIndex;
    for (int i = 0, count = fRawData.count(); i < count; i++) {
        fProxyData << i;
    }
    for (QHash<QString, int>::const_iterator it = fColumnNameIndex.begin(); it != fColumnNameIndex.end(); it++) {
        fColumnIndexName[it.value()] = it.key();
    }
    endResetModel();
    translate(translation);
}

int C5TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return fProxyData.count();
}

int C5TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return fColumnNameIndex.count();
}

QVariant C5TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        if (orientation == Qt::Vertical) {
            return section + 1;
        } else {
            return fTranslateColumn.contains(fColumnIndexName[section]) ? fTranslateColumn[fColumnIndexName[section]] : fColumnIndexName[section];
        }
    case Qt::DecorationRole:
        if (orientation == Qt::Horizontal) {
            if (fFilters.contains(section)) {
                return QIcon(":/filter_set.png");
            }
        }
    }
    return QVariant();
}

int C5TableModel::indexForColumnName(const QString &column)
{
    if (fColumnNameIndex.contains(column)) {
        return fColumnNameIndex[column];
    }
    return -1;
}

QString C5TableModel::nameForColumnIndex(int index)
{
    return fColumnIndexName[index];
}

QVariant C5TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    switch (role) {
    case Qt::CheckStateRole: {
        if (fCheckboxes) {
            if (index.column() == 0) {
                return fRawData.at(fProxyData.at(index.row())).at(index.column()).toInt() == 0 ? Qt::Unchecked : Qt::Checked;
            }
        }
        return QVariant();
    }
    case Qt::DisplayRole:
        if (fCheckboxes) {
            if (index.column() == 0) {
                return "";
            }
        }
        return dataDisplay(fProxyData.at(index.row()), index.column());
    case Qt::EditRole:
        return fRawData.at(fProxyData.at(index.row())).at(index.column());
    case Qt::BackgroundColorRole:
        if (fColorData.contains(fProxyData.at(index.row()))) {
            return fColorData[fProxyData.at(index.row())][index.column()];
        }
        return QVariant(QColor(Qt::white));
    case Qt::FontRole: {
        QFont fo = fTableView->font();
        if (fCellFont.contains(index)) {
            fo = fCellFont[index];
        }
        return fo;
    }
    default:
        return QVariant();
    }
}

bool C5TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role) {
    case Qt::EditRole:
        fRawData[fProxyData.at(index.row())][index.column()] = value;
        break;
    case Qt::CheckStateRole:
        if (fCheckboxes && fSingleCheckBoxSelection && value > 0) {
            for (int i = 0; i < fRawData.count(); i++) {
                fRawData[i][0] = 0;
            }
        }
        fRawData[fProxyData.at(index.row())][index.column()] = value;
        if (fTableView != nullptr) {
            fTableView->viewport()->update();
        }
        break;
    default:
        QAbstractTableModel::setData(index, value, role);
        break;
    }
    return true;
}

bool C5TableModel::setData(int row, int column, const QVariant &value, int role)
{
    return setData(createIndex(row, column), value, role);
}

void C5TableModel::setRowToUpdate(int row, const QString &field, const QVariant &value, const QColor &rowColor)
{
    fRowToUpdate << fProxyData.at(row);
    if (field.length() > 0) {
        fAddDataToUpdate[fProxyData.at(row)][field] = value;
    }
    setRowColor(row, rowColor);
}

void C5TableModel::setRowColor(int row, const QColor &color)
{
    if (fColorData.contains(fProxyData.at(row))) {
        for (int i = 0; i < fColumnIndexName.count(); i++) {
            fColorData[fProxyData.at(row)][i] = color;
        }
    } else {
        QList<QColor> colors;
        for (int i = 0; i < fColumnIndexName.count(); i++) {
            colors << color;
        }
        fColorData[fProxyData.at(row)] = colors;
    }
}

Qt::ItemFlags C5TableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractTableModel::flags(index);
    if (fEditableFlags.contains(index.column())) {
        f = f ^ fEditableFlags[index.column()];
    } else {

    }
    if (fCheckboxes) {
        f = f | Qt::ItemIsUserCheckable;
    }
    return f;
}

void C5TableModel::setEditableFlag(int column, Qt::ItemFlags flag)
{
    fEditableFlags[column] = flag;
}

void C5TableModel::insertRow(int row)
{
    beginInsertRows(QModelIndex(), row, row);
    QList<QVariant> emptyRow;
    for (int i = 0; i < fColumnIndexName.count(); i++) {
        emptyRow << QVariant();
    }
    fRawData.insert(row + 1, emptyRow);
    fProxyData.clear();
    for (int i = 0, count = fRawData.count(); i < count; i++) {
        fProxyData << i;
    }
    endInsertRows();
}

void C5TableModel::insertColumn(int column)
{
    beginInsertColumns(QModelIndex(), column, column);
    for (int i = 0; i < fRawData.count(); i++) {
        fRawData[i].insert(column, QVariant());
    }
    endInsertColumns();
}

void C5TableModel::insertColumn(int column, const QString &header)
{
    for (int i = columnCount(); i > column; i--) {
        fColumnIndexName[i] = fColumnIndexName[i - 1];
        fColumnNameIndex[fColumnIndexName[i]] = i;
    }
    insertColumn(column);
    fColumnIndexName[column] = header;
    fColumnNameIndex[header] = columnCount();
}

void C5TableModel::removeRow(int row, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row);
    fRawData.removeAt(fProxyData.at(row));
    fProxyData.removeAt(row);
    endRemoveRows();
}

QList<QVariant> C5TableModel::getRowValues(int row)
{
    return fRawData.at(fProxyData.at(row));
}

void C5TableModel::saveDataChanges()
{
    C5Database db(fDBParams);
    QList<int> rows = fRowToUpdate.values();
    std::sort(rows.begin(), rows.end());
    for (int i = 0, count = rows.count(); i < count; i++) {
        int row = rows.at(i);
        for (int j = 0; j < fColumnsForUpdate.count(); j++) {
            db[":" + fColumnIndexName[fColumnsForUpdate.at(j)]] = fRawData.at(row).at(fColumnsForUpdate.at(j));
        }
        if (fAddDataToUpdate.contains(row)) {
            for (QMap<QString, QVariant>::const_iterator it = fAddDataToUpdate[row].begin(); it != fAddDataToUpdate[row].end(); it++) {
                db[":" + it.key()] = it.value();
            }
        }
        if (fRawData.at(row).at(0).toString() == 0) {
            fRawData[row][0] = db.insert(fTableForUpdate, true);;
        } else {
            db.update(fTableForUpdate, where_id(fRawData.at(row).at(0).toInt()));
        }
    }
    fRowToUpdate.clear();
    fAddDataToUpdate.clear();
    fColorData.clear();
}

void C5TableModel::setFilter(int column, const QString &filter)
{
    fFilters[column] = filter;
    fProxyData.clear();
    for (int i = 0; i < fRawData.count(); i++) {
        fProxyData << i;
    }
    filterData();
}

void C5TableModel::removeFilter(int column)
{
    fFilters.remove(column);
    fProxyData.clear();
    for (int i = 0; i < fRawData.count(); i++) {
        fProxyData << i;
    }
    filterData();
}

void C5TableModel::clearFilter()
{
    fFilters.clear();
    fProxyData.clear();
    for (int i = 0; i < fRawData.count(); i++) {
        fProxyData << i;
    }
    filterData();
}

void C5TableModel::uniqueValuesForColumn(int column, QSet<QString> &values)
{
    for (int i = 0; i < fProxyData.count(); i++) {
        values << dataDisplay(fProxyData.at(i), column).toString();
    }
}

void C5TableModel::sumForColumns(const QStringList &columns, QMap<QString, double> &values)
{
    foreach (QString s, columns) {
        if (fColumnNameIndex.contains(s)) {
            double total = 0;
            int idx = fColumnNameIndex[s];
            for (int i = 0, count = fProxyData.count(); i < count; i++) {
                total += fRawData.at(fProxyData.at(i)).at(idx).toDouble();
            }
            values[s] = total;
        }
    }
}

void C5TableModel::insertSubTotals(int col, const QList<int> &totalCols)
{
    QFont f = fTableView->font();
    f.setBold(true);
    if (totalCols.count() == 0) {
        return;
    }
    QList<QVariant> emptyRow;
    for (int i = 0; i < columnCount(); i++) {
        emptyRow << QVariant();
    }
    QList<double> totals;
    for (int i = 0; i < totalCols.count(); i++) {
        totals << 0.000;
    }
    QString currName;
    int r = 0;
    QList<int> subtotalSpans;
    while (r < fProxyData.count()) {
        if (r == 0) {
            currName = data(r, col, Qt::EditRole).toString();
        } else {
            if (data(r, col, Qt::EditRole).toString().compare(currName) != 0) {
                QList<QVariant> newrow = emptyRow;
                newrow[0] = tr("SUBTOTAL") + " " + currName;
                for (int c = 0; c < totalCols.count(); c++) {
                    newrow[totalCols.at(c)] = totals[c];
                }
                insertRow(r - 1);
                for (int c = 0; c < columnCount(); c++) {
                    setData(r, c, newrow[c]);
                    fCellFont[createIndex(r, c)] = f;
                }
                subtotalSpans.append(r);
                r++;
                insertRow(r - 1);
                fTableView->setSpan(r, 0, 1, columnCount());
                r++;
                currName = data(r, col, Qt::EditRole).toString();
                for (int i = 0; i < totalCols.count(); i++) {
                    totals[i] = 0.000;
                }
            }
        }
        for (int c = 0; c < totalCols.count(); c++) {
            totals[c] += data(r, totalCols.at(c), Qt::EditRole).toDouble();
        }
        r++;
    }
    if (r > 0) {
        QList<QVariant> newrow = emptyRow;
        newrow[0] = tr("Subtotal") + " " + currName;
        for (int c = 0; c < totalCols.count(); c++) {
            newrow[totalCols.at(c)] = totals[c];
        }
        insertRow(r - 1);
        for (int c = 0; c < columnCount(); c++) {
            setData(r, c, newrow[c]);
            fCellFont[createIndex(r, c)] = f;
        }
        subtotalSpans.append(r);
    }
    for (int r: subtotalSpans) {
        fTableView->setSpan(r, 0, 1, totalCols[0]);
    }
}

void C5TableModel::filterData()
{
    beginResetModel();
    QList<int> ps = fProxyData;
    fProxyData.clear();
    QList<int> columns = fFilters.keys();
    if (columns.contains(-1)) {
        columns.removeFirst();
    }
    QMap<int, QStringList> filter;
    for (QMap<int, QString>::const_iterator it = fFilters.begin(); it != fFilters.end(); it++) {
        filter[it.key()] = it.value().split("|", QString::SkipEmptyParts);
        if (filter[it.key()].count() == 0) {
            filter.remove(it.key());
        }
    }
    for (int r = 0, count = ps.count(); r < count; r++) {
        int row = ps.at(r);
        bool found = filter.count() == 0;
        if (found) {
            goto FOUND;
        }
        if (fFilters.contains(-1)) {
            for (int c = 0; c < fColumnIndexName.count(); c++) {
                foreach (QString searchStr, filter[-1]) {
                    if (dataDisplay(row, c).toString().contains(searchStr, Qt::CaseInsensitive)) {
                        found = true;
                        goto FOUND;
                    }
                }
            }
        }
        found = columns.count() > 0;
        foreach (int col, columns) {
            bool found2 = false;
            foreach (QString searchStr, filter[col]) {
                found2 = found2 || dataDisplay(row, col).toString().contains(searchStr, Qt::CaseInsensitive);
                if (found2) {
                    break;
                }
            }
            found = found && found2;
            if (!found) {
                goto FOUND;
            }
        }
        FOUND:
        if (found) {
            fProxyData << row;
        }
    }
    endResetModel();
}

void C5TableModel::clearModel()
{
    fProxyData.clear();
    fRawData.clear();
    fColumnNameIndex.clear();
    fColumnIndexName.clear();
    fRowToUpdate.clear();
    fAddDataToUpdate.clear();
    fColorData.clear();
    fFilters.clear();
    fCellFont.clear();
}

QVariant C5TableModel::dataDisplay(int row, int column) const
{
    QVariant v = fRawData.at(row).at(column);
    switch (v.type()) {
    case QVariant::Int:
        return v.toString();
    case QVariant::Date:
        return v.toDate().toString(FORMAT_DATE_TO_STR);
    case QVariant::DateTime:
        return v.toDateTime().toString(FORMAT_DATETIME_TO_STR);
    case QVariant::Time:
        return v.toTime().toString(FORMAT_TIME_TO_STR);
    case QVariant::Double:
        return float_str(v.toDouble(), 4);
    default:
        return v.toString();
    }
}
