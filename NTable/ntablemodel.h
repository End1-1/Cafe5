#ifndef NTABLEMODEL_H
#define NTABLEMODEL_H

#include <QAbstractTableModel>
#include <QJsonArray>
#include <QJsonObject>

class NTableModel : public QAbstractTableModel
{
public:
    NTableModel(QObject *parent = 0);

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    virtual int rowCount(const QModelIndex &index = QModelIndex()) const override;

    virtual int columnCount(const QModelIndex &index = QModelIndex()) const override;

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    bool setData(int row, int column, const QVariant &value);

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    QVariant data(int row, int column, int role = Qt::DisplayRole) const;

    const QJsonArray rowData(int row) const;

    void setDatasource(const QJsonArray &jcols, const QJsonArray &ja);

    void setSumColumns(const QJsonArray &jcolsum);

    void setFilter(const QString &filter);

    void setColumnFilter(const QString &filter, int column);

    QMap<int, double> fColSum;

    QJsonArray fSumColumnsSpecial;

    void setCheckedBox(bool v, bool multiSelect);

    void check(int row);

    void check(int row, bool value);

    bool checked(int &row);

    const QSet<QString> &uniqueValuesForColumn(int column);

private:
    QMap<int, QSet<QString> > mUniqueValuesForColumn;

    QJsonArray mColumnsNames;

    QJsonArray mRawData;

    QList<int> fColumnsOfDouble;

    QList<int> mProxyRows;

    QList<bool> mCheckboxValues;

    bool mCheckboxMode;

    bool mCheckboxModeMultySelect;

    Qt::SortOrder mSortOrder;

    int mLastColumnSorted;

    void countSum();

};

#endif // NTABLEMODEL_H
