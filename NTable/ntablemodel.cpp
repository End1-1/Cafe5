#include "ntablemodel.h"
#include <QIcon>

NTableModel::NTableModel( QObject *parent) :
    QAbstractTableModel(parent)
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
    return mRawData.size();
}

int NTableModel::columnCount(const QModelIndex &index) const
{
    return mColumnsNames.size();
}

QVariant NTableModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        return mRawData.at(index.row()).toArray().at(index.column()).toString();
    }

    return QVariant();
}

QVariant NTableModel::data(int row, int column, int role) const
{
    return data(index(row, column), role);
}

void NTableModel::setDatasource(const QJsonArray &jcols, const QJsonArray &ja)
{
    beginResetModel();
    mColumnsNames = jcols;
    mRawData = ja;
    endResetModel();
}
