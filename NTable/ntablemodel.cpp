#include "ntablemodel.h"
#include "c5utils.h"
#include <QIcon>

NTableModel::NTableModel( QObject *parent) :
    QAbstractTableModel(parent),
    mCheckboxMode(false)
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
    return mProxyRows.size();
}

int NTableModel::columnCount(const QModelIndex &index) const
{
    return mColumnsNames.size();
}

bool NTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
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
    case Qt::DisplayRole:
        if (index.column() == 0 && mCheckboxMode) {
            return QVariant();
        }
        v = mRawData.at(row).toArray().at(index.column());
        if (fColumnsOfDouble.contains(index.column())) {
            return float_str(str_float(v.toString()), 2);
        }
        return v;
    }

    return QVariant();
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
    mColumnsNames = jcols;
    mRawData = ja;
    mProxyRows.clear();
    for (int i = 0; i < ja.size(); i++) {
        mProxyRows.append(i);
    }
    endResetModel();
}

void NTableModel::setSumColumns(const QJsonObject &jcolsum)
{
    fColSum = jcolsum;
    fColumnsOfDouble.clear();
    for (const QString &s: fColSum.keys()) {
        fColumnsOfDouble.append(s.toInt());
    }
}

void NTableModel::setFilter(const QString &filter)
{
    beginResetModel();
    mProxyRows.clear();
    for (int i = 0; i < mRawData.size(); i++) {
        const QJsonArray &ja = mRawData.at(i).toArray();
        for (int j = 0; j < ja.size(); j++) {
            if (ja[j].toString().contains(filter, Qt::CaseInsensitive)) {
                mProxyRows.append(i);
                break;
            }
        }
    }
    QStringList keys = fColSum.keys();
    if (keys.length() > 0) {
        QMap<int, double> sums;
        for (const QString &k: keys) {
            sums[k.toInt()] = 0;
        }
        for (int i = 0; i < mProxyRows.size(); i++) {
            for (QMap<int, double>::iterator it = sums.begin(); it != sums.end(); it++) {
                it.value() += str_float(mRawData[mProxyRows[i]].toArray()[it.key()].toString());
            }
        }
        for (QMap<int, double>::const_iterator it = sums.constBegin(); it != sums.constEnd(); it++) {
            fColSum[QString::number(it.key())] = it.value();
        }
    }
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
