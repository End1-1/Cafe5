#include "c5tablemodel.h"
#include "c5utils.h"
#include "c5database.h"
#include <QIcon>
#include <QTableView>
#include <QApplication>
#include <QElapsedTimer>

C5TableModel::C5TableModel(const QStringList &dbParams, QObject *parent) :
    QAbstractTableModel(parent),
    fDBParams(dbParams)
{
    fTableView = dynamic_cast<QTableView *>(parent);
    fSortAsc = true;
    fLastSortedColumn = -1;
    fCheckboxes = false;
    fSingleCheckBoxSelection = true;
}

void C5TableModel::translate(const QHash<QString, QString> &t)
{
    fTranslateColumn = t;
}

void C5TableModel::execQuery(const QString &query, QObject *echoError)
{
    QElapsedTimer timer;
    timer.start();
    beginResetModel();
    clearModel();
    C5Database db(fDBParams);
    connect( &db, SIGNAL(queryError(QString)), echoError, SLOT(sqlError(QString)));
    QStringList queries = query.split(";", Qt::SkipEmptyParts);
    db.exec(query, fRawData, fColumnNameIndex);
    qDebug() << "Query executed in " << timer.elapsed() << "ms" ;
    fColumnType = db.fColumnType;
    for (int i = 0, count = static_cast<int> (fRawData.size()); i < count; i++) {
        if (fCheckboxes) {
            fRawData[i].insert(0, QJsonValue());
        }
        fProxyData << i;
    }
    for (QHash<QString, int>::const_iterator it = fColumnNameIndex.constBegin(); it != fColumnNameIndex.constEnd(); it++) {
        fColumnIndexName[it.value()] = it.key();
    }
    if (fCheckboxes) {
        for (int i = fColumnType.count(); i > -1; i--) {
            fColumnType[i + 1] = fColumnType[i];
            fColumnType.remove(i);
        }
        fColumnType[0] = QMetaType::Int;
        for (QHash<QString, int>::iterator it = fColumnNameIndex.begin(); it != fColumnNameIndex.end(); it++) {
            QString k = it.key();
            int v = it.value() + 1;
            fColumnNameIndex[k] = v;
            fColumnIndexName[v] = k;
        }
        fColumnNameIndex["X"] = 0;
        fColumnIndexName[0] = "X";
    }
    endResetModel();
    qDebug() << "Data loaded in " << timer.elapsed() << "ms";
}

void C5TableModel::setCheckboxes(bool v)
{
    fCheckboxes = v;
}

void C5TableModel::setSingleCheckBoxSelection(bool v)
{
    fSingleCheckBoxSelection = v;
}

void C5TableModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(order);
    if (fLastSortedColumn == column) {
        fSortAsc = !fSortAsc;
    } else {
        fSortAsc = true;
    }
    fLastSortedColumn = column;
    QMultiMap<QString, int > data_s;
    QMultiMap<int, int> data_i;
    QMultiMap<double, int> data_d;
    foreach (int i, fProxyData) {
        const QJsonValue &v = fRawData[i][column];
        switch (fColumnType[column]) {
            case QMetaType::Int:
                data_i.insert(v.toInt(), i);
                break;
            case QMetaType::Double:
                data_d.insert(v.toDouble(), i);
                break;
            default:
                data_s.insert(v.toString(), i);
                break;
        }
    }
    beginResetModel();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (!data_s.isEmpty()) {
        fProxyData = data_s.values().toVector();
    } else  if (!data_i.isEmpty()) {
        fProxyData = data_i.values().toVector();
    } else if (!data_d.isEmpty()) {
        fProxyData = data_d.values().toVector();
    }
#else
    if (!data_s.isEmpty()) {
        fProxyData = data_s.values();
    } else  if (!data_i.isEmpty()) {
        fProxyData = data_i.values();
    } else if (!data_d.isEmpty()) {
        fProxyData = data_d.values();
    }
#endif
    if (!fSortAsc) {
        std::reverse(fProxyData.begin(), fProxyData.end());
    }
    endResetModel();
}

void C5TableModel::setExternalData(const QHash<QString, int> &columnNameIndex,
                                   const QHash<QString, QString> &translation)
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
    for (int i = 0, count = static_cast<int>(fRawData.size()); i < count; i++) {
        fProxyData << i;
    }
    for (QHash<QString, int>::const_iterator it = fColumnNameIndex.constBegin(); it != fColumnNameIndex.constEnd(); it++) {
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
                return fTranslateColumn.contains(fColumnIndexName[section]) ? fTranslateColumn[fColumnIndexName[section]] :
                       fColumnIndexName[section];
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
    if (fRawData.empty() == true) {
        return "WASTED";
    }
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
            return fRawData.at(fProxyData.at(index.row())).at(index.column()).toVariant();
        case Qt::BackgroundRole:
            if (fColorData.contains(fProxyData.at(index.row()))) {
                return fColorData[fProxyData.at(index.row())][index.column()];
            }
            return QVariant(QApplication::palette().color(QPalette::Base));
        case Qt::FontRole: {
            QFont fo;
            if (fTableView) {
                if (fCellFont.contains(index)) {
                    fo = fCellFont[index];
                } else {
                    fo = fTableView->font();
                }
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
            fRawData[fProxyData.at(index.row())][index.column()] = value.toJsonValue();
            break;
        case Qt::CheckStateRole:
            if (fCheckboxes && fSingleCheckBoxSelection && value.toInt() > 0) {
                for (int i = 0; i < fRawData.size(); i++) {
                    fRawData[i][0] = 0;
                }
            }
            fRawData[fProxyData.at(index.row())][index.column()] = value.toJsonValue();
            if (fTableView != nullptr) {
                fTableView->viewport()->update();
            }
            break;
        case Qt::FontRole:
            fCellFont[index] = value.value<QFont>();
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
        f = f ^fEditableFlags[index.column()];
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
    beginInsertRows(QModelIndex(), row < 0 ? 0 : row, row < 0 ? 0 : row);
    QJsonArray emptyRow;
    for (int i = 0; i < fColumnIndexName.count(); i++) {
        emptyRow << QJsonValue();
    }
    int insertPos = row + 1;
    if (insertPos > static_cast<int>(fRawData.size())) {
        insertPos = static_cast<int>(fRawData.size());
    }
    auto pos = fRawData.begin() + insertPos;
    if (pos >= fRawData.end()) {
        fRawData.push_back(emptyRow);
    } else {
        fRawData.insert(pos, emptyRow);
    }
    fProxyData.resize(fRawData.size());
    std::iota(fProxyData.begin(), fProxyData.end(), 0);
    endInsertRows();
}

void C5TableModel::insertColumn(int column)
{
    beginInsertColumns(QModelIndex(), column, column);
    for (int i = 0; i < fRawData.size(); i++) {
        fRawData[i].insert(column, QJsonValue());
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
    fRawData.erase(fRawData.begin() + fProxyData.at(row));
    fProxyData.clear();
    for (int i = 0, count = static_cast<int>(fRawData.size()); i < count; i++) {
        fProxyData << i;
    }
    endRemoveRows();
}

QJsonArray C5TableModel::getRowValues(int row)
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
            for (QMap<QString, QVariant>::const_iterator it = fAddDataToUpdate[row].begin(); it != fAddDataToUpdate[row].end();
                    it++) {
                db[":" + it.key()] = it.value();
            }
        }
        if (fRawData.at(row).at(0).toString().toInt() == 0) {
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
    if (column == -1) {
        fFilters.clear();
    } else {
        fFilters.remove(-1);
    }
    fFilters[column] = filter;
    fProxyData.clear();
    for (int i = 0; i < fRawData.size(); i++) {
        fProxyData << i;
    }
    filterData();
}

void C5TableModel::removeFilter(int column)
{
    fFilters.remove(column);
    fProxyData.clear();
    for (int i = 0; i < fRawData.size(); i++) {
        fProxyData << i;
    }
    filterData();
}

void C5TableModel::clearFilter()
{
    fFilters.clear();
    fProxyData.clear();
    for (int i = 0; i < fRawData.size(); i++) {
        fProxyData << i;
    }
    filterData();
}

void C5TableModel::uniqueValuesForColumn(int column, QSet<QString> &values)
{
    for (int i = 0; i < fProxyData.count(); i++) {
        values << dataDisplay(fProxyData.at(i), column);
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

void C5TableModel::sumForColumnsIndexes(const QList<int> &columns, QMap<int, double> &values)
{
    for (int idx : columns) {
        double total = 0;
        for (int i = 0, count = fProxyData.count(); i < count; i++) {
            total += fRawData.at(fProxyData.at(i)).at(idx).toDouble();
        }
        values[idx] = total;
    }
}

void C5TableModel::insertSubTotals(QList<int> cols, const QList<int> &totalCols, bool title, bool insertempty)
{
    QFont f = fTableView->font();
    f.setBold(true);
    if (totalCols.count() == 0) {
        return;
    }
    QVector<QJsonValue> emptyRow;
    for (int i = 0; i < columnCount(); i++) {
        emptyRow << QJsonValue();
    }
    QList<double> totals;
    for (int i = 0; i < totalCols.count(); i++) {
        totals << 0.000;
    }
    QString currName;
    int r = 0;
    while (r < fProxyData.count()) {
        if (r == 0) {
            currName += subtotalRowData(r, cols);
        } else {
            if (isRowDataEmpty(r, cols)) {
                r++;
                continue;
            }
            if (!isRowDataEmpty(r, cols)
                    && subtotalRowData(r, cols).compare(currName) != 0) {
                QVector<QJsonValue> newrow = emptyRow;
                //newrow[0] = tr("SUBTOTAL") + " " + currName;
                for (int c = 0; c < totalCols.count(); c++) {
                    newrow[totalCols.at(c)] = totals[c];
                }
                insertRow(r - 1);
                for (int c = 0; c < columnCount(); c++) {
                    setData(r, c, newrow[c]);
                    //fCellFont[createIndex(r, c)] = f;
                }
                r++;
                if (insertempty) {
                    insertRow(r - 1);
                    //fTableView->setSpan(r, 0, 1, columnCount());
                    r++;
                }
                currName = subtotalRowData(r, cols);
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
        QVector<QJsonValue> newrow = emptyRow;
        if (title) {
            //newrow[0] = tr("Subtotal") + " " + currName;
        }
        for (int c = 0; c < totalCols.count(); c++) {
            newrow[totalCols.at(c)] = totals[c];
        }
        insertRow(r - 1);
        for (int c = 0; c < columnCount(); c++) {
            setData(r, c, newrow[c]);
            //fCellFont[createIndex(r, c)] = f;
        }
    }
}

void C5TableModel::resetProxyData()
{
    beginResetModel();
    fProxyData.clear();
    for (int i = 0, count = static_cast<int>(fRawData.size()); i < count; i++) {
        fProxyData << i;
    }
    endResetModel();
}

void C5TableModel::filterData()
{
    QElapsedTimer et;
    et.start();
    beginResetModel();
    QVector<int> ps = std::move(fProxyData);
    fProxyData.clear();
    fProxyData.reserve(ps.size());
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QVector<int> columns = fFilters.keys().toVector();
#else
    QVector<int> columns = fFilters.keys();
#endif
    if (columns.count() > 1 && columns.contains(-1)) {
        columns.removeOne(-1);
    }
    if (fFilters.count() == 0) {
        for (int i = 0, count = static_cast<int>(fRawData.size()); i < count; i++) {
            fProxyData << i;
        }
        endResetModel();
        return;
    }
    //SEARCH EVERYWHERE
    if (fFilters.contains(-1)) {
        QRegularExpression regex(".*" + QRegularExpression::escape(fFilters[-1]) + ".*",
                                 QRegularExpression::CaseInsensitiveOption);
        for (int r = 0; r < ps.count(); r++) {
            QJsonArray &rowArray = fRawData[r];
            QStringList rowTextList;
            rowTextList.reserve(rowArray.size());
            for (const QJsonValue &val : rowArray) {
                rowTextList.append(val.toString());
            }
            if (regex.match(rowTextList.join("🍆")).hasMatch()) {
                fProxyData.append(r);
            }
        }
    } else {
        for (int r = 0; r < ps.count(); r++) {
            for (const auto &[c, v] : fFilters.asKeyValueRange()) {
                QStringList patterns = v.split('|');
                for (QString &p : patterns) {
                    p = QRegularExpression::escape(p);
                }
                QRegularExpression regex(".*(" + patterns.join('|') + ").*",
                                         QRegularExpression::CaseInsensitiveOption);
                const QJsonValue &val = fRawData[r][c];
                if (regex.match(val.toVariant().toString()).hasMatch()) {
                    fProxyData.append(r);
                    break;
                }
            }
        }
    }
    endResetModel();
    qDebug() << "Search completed with" << et.elapsed();
}

bool C5TableModel::isRowDataEmpty(int row, const QList<int> &cols)
{
    if (cols.count() == 0) {
        for (int i = 0; i < columnCount(); i++) {
            if (data(row, i, Qt::EditRole).toString().isEmpty() == false) {
                return false;
            }
        }
    } else {
        for (int i = 0; i < cols.count(); i++) {
            if (data(row, cols.at(i), Qt::EditRole).toString().isEmpty() == false) {
                return false;
            }
        }
    }
    return true;
}

QString C5TableModel::subtotalRowData(int row, const QList<int> &cols)
{
    QString r;
    for (int c : cols) {
        r += data(row, c, Qt::EditRole).toString();
    }
    return r;
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

QString C5TableModel::dataDisplay(int row, int column) const
{
    QJsonValue v = fRawData.at(row).at(column);
    switch (fColumnType[column]) {
        case QMetaType::Int:
            return QString::number(v.toInt());
        case QMetaType::QDate:
            return QDate::fromString(v.toString(), FORMAT_DATE_TO_STR_MYSQL).toString(FORMAT_DATE_TO_STR);
        case QMetaType::QDateTime:
            return QDateTime::fromString(v.toString(), FORMAT_DATETIME_TO_STR_MYSQL).toString(FORMAT_DATETIME_TO_STR);
        case QMetaType::QTime:
            return QTime::fromString(v.toString()).toString(FORMAT_TIME_TO_STR);
        case QMetaType::Double:
            return float_str(v.toDouble(), 4);
        default:
            return v.toString();
    }
}
