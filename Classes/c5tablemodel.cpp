#include "c5tablemodel.h"

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
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Vertical) {
            return section + 1;
        } else {
            return fTranslateColumn.contains(fColumnIndexName[section]) ? fTranslateColumn[fColumnIndexName[section]] : fColumnIndexName[section];
        }
    }
    return QVariant();
}

QVariant C5TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
        return dataDisplay(index.row(), index.column());
    default:
        return QVariant();
    }
}

void C5TableModel::clearModel()
{
    fRawData.clear();
    fColumnNameIndex.clear();
    fColumnIndexName.clear();
}
