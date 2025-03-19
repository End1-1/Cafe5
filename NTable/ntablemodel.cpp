#include "ntablemodel.h"
#include "c5utils.h"
#include <QIcon>

NTableModel::NTableModel( QObject *parent) :
    QAbstractTableModel(parent),
    mCheckboxMode(false),
    mSortOrder(Qt::AscendingOrder),
    mLastColumnSorted(-1)
{
}

QVariant NTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            if (orientation == Qt::Vertical) {
                return section + 1;
            } else {
                return mColumnsNames[section].toString();
            }
        case Qt::DecorationRole:
            if (orientation == Qt::Horizontal) {
                //            if (fFilters.contains(section)) {
                //                return QIcon(":/filter_set.png");
                //            }
            }
    }
    return QVariant();
}

int NTableModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return mProxyRows.size();
}

int NTableModel::columnCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return mColumnsNames.size();
}

bool NTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);
    int row = mProxyRows[index.row()];
    QJsonArray ja = mRawData[row].toArray();
    ja.replace(0, value.toString());
    mRawData.replace(row, ja);
    return true;
}

bool NTableModel::setData(int row, int column, const QVariant &value)
{
    const QModelIndex &i = index(row, column);
    return setData(i, value, Qt::EditRole);
}

QVariant NTableModel::data(const QModelIndex &index, int role) const
{
    QVariant v;
    const int &row = mProxyRows.at(index.row());
    switch (role) {
        case Qt::CheckStateRole: {
            if (mCheckboxMode) {
                if (index.column() == 0) {
                    return mCheckboxValues.at(row) ? Qt::Checked : Qt::Unchecked;
                }
            }
            return QVariant();
        }
        case Qt::EditRole:
            if (index.column() == 0 && mCheckboxMode) {
                return QVariant();
            }
            return mRawData.at(row).toArray().at(index.column()).toVariant();
        case Qt::DisplayRole: {
            if (index.column() == 0 && mCheckboxMode) {
                return QVariant();
            }
            QJsonValue vv = mRawData.at(row).toArray().at(index.column());
            switch (vv.type()) {
                case QJsonValue::Double:
                    return float_str(vv.toDouble(), 2);
                default:
                    return vv.toString();
            }
            break;
        }
        case Qt::BackgroundRole: {
            if (mRowColors.contains(row)) {
                return QColor::fromRgb(mRowColors[row]);
            }
            return QColor(Qt::white).toRgb();
        }
    }
    return QVariant();
}

void NTableModel::sort(int column, Qt::SortOrder order)
{
    if (mLastColumnSorted > -1 && mLastColumnSorted != column) {
        order = Qt::AscendingOrder;
    } else {
        order = mSortOrder == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder;
    }
    mSortOrder = order;
    QMultiMap<QString, int> data_s;
    QMultiMap<double, int> data_d;
    QMultiMap<quint64, int> data_i;
    foreach (int i, mProxyRows) {
        QVariant v = mRawData.at(i).toArray().at(column).toVariant();
        switch (v.typeId()) {
            case QMetaType::Int:
            case QMetaType::LongLong:
            case QMetaType::Long:
                data_i.insert(v.toLongLong(), i);
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
    if (!data_s.isEmpty()) {
        mProxyRows = data_s.values();
    } else  if (!data_i.isEmpty()) {
        mProxyRows = data_i.values();
    } else if (!data_d.isEmpty()) {
        mProxyRows = data_d.values();
    }
    if (mSortOrder == Qt::DescendingOrder) {
        std::reverse(mProxyRows.begin(), mProxyRows.end());
    }
    endResetModel();
}

QVariant NTableModel::data(int row, int column, int role) const
{
    return data(index(row, column), role);
}

const QJsonArray NTableModel::rowData(int row) const
{
    row = mProxyRows[row];
    return mRawData.at(row).toArray();
}

void NTableModel::setDatasource(const QJsonArray &jcols, const QJsonArray &ja)
{
    beginResetModel();
    mUniqueValuesForColumn.clear();
    mColumnsNames = jcols;
    mRawData = ja;
    mProxyRows.clear();
    for (int i = 0; i < ja.size(); i++) {
        mProxyRows.append(i);
    }
    endResetModel();
}

void NTableModel::setSumColumns(const QJsonArray &jcolsum)
{
    fColSum.clear();
    fColumnsOfDouble.clear();
    for (int i = 0; i < jcolsum.size(); i++) {
        qDebug() << jcolsum.at(i);
        QJsonObject jo = jcolsum.at(i).toObject();
        fColSum[jo.keys().first().toInt()] = 0;
        fColumnsOfDouble.append(jcolsum.at(i).toInt());
    }
    countSum();
}

void NTableModel::setFilter(const QString &filter)
{
    beginResetModel();
    mProxyRows.clear();
    for (int i = 0; i < mRawData.size(); i++) {
        const QJsonArray &ja = mRawData.at(i).toArray();
        for (int j = 0; j < ja.size(); j++) {
            if (ja[j].toVariant().toString().contains(filter, Qt::CaseInsensitive)) {
                mProxyRows.append(i);
                break;
            }
        }
    }
    countSum();
    endResetModel();
}

void NTableModel::setColumnFilter(const QString &filter, int column)
{
    beginResetModel();
    mProxyRows.clear();
    QStringList filters = filter.split("|", Qt::SkipEmptyParts);
    for (int i = 0; i < mRawData.size(); i++) {
        const QJsonArray &ja = mRawData.at(i).toArray();
        if (filters.contains(ja[column].toVariant().toString())) {
            mProxyRows.append(i);
        }
    }
    countSum();
    endResetModel();
}

void NTableModel::setCheckedBox(bool v, bool multiSelect)
{
    beginResetModel();
    mCheckboxMode = v;
    mCheckboxModeMultySelect = multiSelect;
    mCheckboxValues.clear();
    for (int i = 0; i < mRawData.size(); i++) {
        mCheckboxValues.append(false);
    }
    endResetModel();
}

void NTableModel::check(int row)
{
    beginResetModel();
    if (!mCheckboxModeMultySelect) {
        for (int i = 0; i < mCheckboxValues.size(); i++) {
            mCheckboxValues[i] = false;
        }
    }
    row = mProxyRows[row];
    mCheckboxValues[row] = !mCheckboxValues[row];
    endResetModel();
}

void NTableModel::check(int row, bool value)
{
    beginResetModel();
    if (!mCheckboxModeMultySelect) {
        for (int i = 0; i < mCheckboxValues.size(); i++) {
            mCheckboxValues[i] = false;
        }
    }
    row = mProxyRows[row];
    mCheckboxValues[row] = value;
    endResetModel();
}

bool NTableModel::checked(int &row)
{
    if (!mCheckboxMode) {
        return false;
    }
    for (int i = 0; i < mProxyRows.size(); i++) {
        if (mCheckboxValues[mProxyRows[i]]) {
            row = i;
            return true;
        }
    }
    return false;
}

bool NTableModel::checkedRows(std::vector<int> &rows)
{
    if (!mCheckboxMode) {
        return false;
    }
    for (int i = 0; i < mProxyRows.size(); i++) {
        if (mCheckboxValues[mProxyRows[i]]) {
            rows.push_back(i);
        }
    }
    return !rows.empty();
}

const QSet<QString> &NTableModel::uniqueValuesForColumn(int column)
{
    if (!mUniqueValuesForColumn.contains(column)) {
        mUniqueValuesForColumn[column] = QSet<QString>();
    }
    QSet<QString> &s = mUniqueValuesForColumn[column];
    if (mUniqueValuesForColumn[column].isEmpty()) {
        for (int i = 0; i < mRawData.count(); i++) {
            s.insert(data(i, column, Qt::EditRole).toString());
        }
    }
    return s;
}

void NTableModel::countSum()
{
    for (QMap<int, double>::iterator it = fColSum.begin(); it != fColSum.end(); it++) {
        it.value() = 0;
    }
    for (int i = 0; i < mProxyRows.size(); i++) {
        for (QMap<int, double>::iterator it = fColSum.begin(); it != fColSum.end(); it++) {
            it.value() += mRawData[mProxyRows[i]].toArray()[it.key()].toDouble();
        }
    }
    for (int i = 0; i < fSumColumnsSpecial.size(); i++) {
        QJsonObject jo = fSumColumnsSpecial.at(i).toObject();
        if (jo["formula"].toString() == "totaldiff") {
            double d = 0;
            QJsonArray ja = jo["diffcols"].toArray();
            for (int j = 0; j < ja.size(); j++) {
                d += fColSum[ja.at(j).toInt()];
            }
            fColSum[jo["column"].toInt()] = d;
        } else if (jo["formula"].toString() == "percent") {
            QJsonArray jc = jo["cols"].toArray();
            fColSum[jo["column"].toInt()] = fColSum[jc.at(0).toInt()] / fColSum[jc.at(1).toInt()] * 100;
        }
    }
}
