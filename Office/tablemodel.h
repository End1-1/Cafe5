#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>

#define float_str(value, f) QLocale().toString(value, 'f', f).remove(QRegExp("(?!\\d[\\.\\,][1-9]+)0+$")).remove(QRegExp("[\\.\\,]$"))
#define int_str(value) QLocale().toString(value)
#define str_float(value) QLocale().toDouble(value)

class TableModel : public QAbstractTableModel
{
public:
    TableModel(QObject *parent = nullptr);
    QMap<int, double> fSumOfColumns;
    void setSourceData(QByteArray b);
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    int rowCount();
    int columnCount();
    int columnWidth(int index);
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void quickFilter(const QString &text);

private:
    bool fErrorState;
    QString fErrorString;
    QByteArray fData;
    quint32 fRowCount;
    quint8 fColumnCount;
    QList<int> fRows;
    QList<bool> fColumnSum;
    QStringList fColumnsCaption;
    QList<quint16> fColumnsWidths;
    QList<quint8> fColumnsDataType;
    quint32 *fDataMap;
    void resetModel();
    void sumOfColumns();
    QVariant data(int row, int column, int role) const;
};

#endif // TABLEMODEL_H
