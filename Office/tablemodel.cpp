#include "tablemodel.h"
#include "rawmessage.h"
#include <QLocale>

const quint8 cdt_int = 1;
const quint8 cdt_double = 2;
const quint8 cdt_string = 3;

TableModel::TableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    resetModel();
}

void TableModel::setSourceData(QByteArray b)
{
    resetModel();
    beginResetModel();
    fData = b;
    RawMessage rm(nullptr);
    quint8 reply;
    rm.readUByte(reply, fData);
    if (reply < 3) {
        QString err;
        rm.readString(err, fData);
        fRowCount = 1;
        fColumnCount = 1;
        fColumnsCaption.append("Error");
        fErrorState = true;
        fErrorString = err;
        endResetModel();
        return;
    }

    rm.readUByte(fColumnCount, fData);
    rm.readUInt(fRowCount, fData);
    for (quint8 i = 0; i < fColumnCount; i++) {
        quint8 t;
        rm.readUByte(t, fData);
        fColumnsDataType.append(t);
    }
    for (quint8 i = 0; i < fColumnCount; i++){
        quint16 colWidth;
        rm.readUShort(colWidth, fData);
        fColumnsWidths.append(colWidth);
    }
    for (quint8 i = 0; i < fColumnCount; i++) {
        QString columnName;
        rm.readString(columnName, fData);
        fColumnsCaption.append(columnName);
    }
    for (quint8 i = 0; i < fColumnCount; i++) {
        quint8 bs;
        rm.readUByte(bs, fData);
        fColumnSum.append(bs == 1);
    }
    for (quint32 i = 0; i < fRowCount; i++) {
        fRows.append(i);
    }
    fDataMap = reinterpret_cast<quint32*>(&fData.data()[fData.length() - sizeof(quint32) * fRowCount * fColumnCount - sizeof(quint32) + 4]);
    sumOfColumns();
    endResetModel();
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return fRows.count();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return fColumnCount;
}

int TableModel::rowCount()
{
    return rowCount(QModelIndex());
}

int TableModel::columnCount()
{
    return columnCount(QModelIndex());
}

int TableModel::columnWidth(int index)
{
    return fColumnsWidths.at(index);
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    int row = fRows.at(index.row());
    return data(row, index.column(), role);
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        switch (orientation) {
        case Qt::Horizontal:
            return fColumnsCaption.at(section);
        case Qt::Vertical:
            return section + 1;
        }
    }
    return QVariant();
}

void TableModel::quickFilter(const QString &text)
{
    beginResetModel();
    fRows.clear();
    for (int r = 0; r < fRowCount; r++) {
        for (int c = 0; c < fColumnCount; c++) {
            if (data(r, c, Qt::EditRole).toString().contains(text, Qt::CaseInsensitive)) {
                fRows.append(r);
                break;
            }
        }
    }
    endResetModel();
}

void TableModel::resetModel()
{
    fErrorState = false;
    fErrorString.clear();
    fColumnsCaption.clear();
    fRowCount = 0;
    fColumnCount = 0;
    fColumnSum.clear();
}

void TableModel::sumOfColumns()
{
    fSumOfColumns.clear();
    for (int i = 0; i < fColumnSum.count(); i++) {
        if (fColumnSum.at(i)) {
            fSumOfColumns.insert(i, 0);
        }
    }
    for (int r = 0, c = rowCount(); r < c; r++) {
       for (QMap<int, double>::iterator kv = fSumOfColumns.begin(); kv != fSumOfColumns.end(); kv++) {
           kv.value() += data(createIndex(r, kv.key()), Qt::EditRole).toDouble();
       }
    }
}

QVariant TableModel::data(int row, int column, int role) const
{
    switch (role) {
    case Qt::EditRole: {
        if (fErrorState) {
            return fErrorString;
        }
        int offcet = (row * fColumnCount) + column;
        int addr = fDataMap[offcet];
        switch (fColumnsDataType.at(column)) {
        case cdt_int: {
            const int *v = reinterpret_cast<const int*>(&fData.data()[addr]);
            return *v;
        }
        case cdt_double: {
            const double *v = reinterpret_cast<const double*>(&fData.data()[addr]);
            return *v;
            }
        case cdt_string: {
            const char *v = &fData.data()[addr + 4];
            return v;
        }
        }
        break;
    }
    case Qt::DisplayRole: {
        if (fErrorState) {
            return fErrorString;
        }
        int offcet = (row * fColumnCount) + column;
        int addr = fDataMap[offcet];
        switch (fColumnsDataType.at(column)) {
        case cdt_int: {
            const int *v = reinterpret_cast<const int*>(&fData.data()[addr]);
            return int_str(*v);
        }
        case cdt_double: {
            const double *v = reinterpret_cast<const double*>(&fData.data()[addr]);
            return float_str(*v, 3);
            }
        case cdt_string: {
            const char *v = &fData.data()[addr + 4];
            return v;
        }
        }
        break;
    }
    }
    return QVariant();
}
