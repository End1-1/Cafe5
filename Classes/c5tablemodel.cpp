#include "c5tablemodel.h"
#include "c5utils.h"
#include <QIcon>

C5TableModel::C5TableModel(C5Database &db, QObject *parent) :
    QAbstractTableModel(parent),
    fDb(db)
{

}

void C5TableModel::translate(const QMap<QString, QString> &t)
{
    fTranslateColumn = t;
}

void C5TableModel::execQuery(const QString &query)
{
    beginResetModel();
    clearModel();
    fDb.exec(query, fRawData, fColumnNameIndex);
    for (int i = 0, count = fRawData.count(); i < count; i++) {
        fProxyData << i;
    }
    for (QMap<QString, int>::const_iterator it = fColumnNameIndex.begin(); it != fColumnNameIndex.end(); it++) {
        fColumnIndexName[it.value()] = it.key();
    }
    endResetModel();
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

QVariant C5TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    switch (role) {
    case Qt::DisplayRole:
        return dataDisplay(fProxyData.at(index.row()), index.column());
    case Qt::EditRole:
        return dataDisplay(fProxyData.at(index.row()), index.column());
    case Qt::BackgroundColorRole:
        if (fColorData.contains(fProxyData.at(index.row()))) {
            return fColorData[fProxyData.at(index.row())][index.column()];
        }
        return QVariant(QColor(Qt::white));
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
    }
    return true;
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
    if (fEditableFlags.contains(index.column())) {
        return QAbstractTableModel::flags(index) ^ fEditableFlags[index.column()];
    } else {
        return QAbstractTableModel::flags(index);
    }
}

void C5TableModel::setEditableFlag(int column, Qt::ItemFlags flag)
{
    fEditableFlags[column] = flag;
}

void C5TableModel::insertRow(int row)
{
    beginInsertRows(QModelIndex(), row + 1, row + 1);
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

void C5TableModel::saveDataChanges()
{
    for (int i = 0, count = fRowToUpdate.count(); i < count; i++) {
        int row = fRowToUpdate.toList().at(i);
        if (fRawData.at(row).at(0).toString() == 0) {
            fDb[":f_id"] = 0;
            fRawData[row][0] = fDb.insert(fTableForUpdate, true);;
        }
        for (int j = 0; j < fColumnsForUpdate.count(); j++) {
            fDb[":" + fColumnIndexName[fColumnsForUpdate.at(j)]] = fRawData.at(row).at(fColumnsForUpdate.at(j));
        }
        if (fAddDataToUpdate.contains(row)) {
            for (QMap<QString, QVariant>::const_iterator it = fAddDataToUpdate[row].begin(); it != fAddDataToUpdate[row].end(); it++) {
                fDb[":" + it.key()] = it.value();
            }
        }
        fDb.update(fTableForUpdate, where_id(fRawData.at(row).at(0).toInt()));
    }
    fRowToUpdate.clear();
    fAddDataToUpdate.clear();
    fColorData.clear();
}

void C5TableModel::setFilter(int column, const QString &filter)
{
    fFilters[column] = filter;
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

void C5TableModel::filterData()
{
    beginResetModel();
    QList<int> ps = fProxyData;
    fProxyData.clear();
    QList<int> columns = fFilters.keys();
    if (columns.contains(-1)) {
        columns.removeFirst();
    }
    for (int r = 0, count = ps.count(); r < count; r++) {
        int row = ps.at(r);
        bool found = fFilters.count() == 0;
        if (found) {
            goto FOUND;
        }
        if (fFilters.contains(-1)) {
            for (int c = 0; c < fColumnIndexName.count(); c++) {
                if (dataDisplay(row, c).toString().contains(fFilters[-1], Qt::CaseInsensitive)) {
                    found = true;
                    goto FOUND;
                }
            }
        }
        found = columns.count() > 0;
        foreach (int col, columns) {
            found = found && dataDisplay(row, col).toString().contains(fFilters[col], Qt::CaseInsensitive);
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
        return float_str(v.toDouble(), 2);
    default:
        return v.toString();
    }
}
