#ifndef C5TABLEMODEL_H
#define C5TABLEMODEL_H

#include "c5database.h"
#include <QAbstractTableModel>
#include <QSet>

class C5TableModel : public QAbstractTableModel
{
public:
    C5TableModel(C5Database &db, QObject *parent = 0);

    void translate(const QMap<QString, QString> &t);

    void execQuery(const QString &query);

    void setExternalData(const QMap<QString, int> &columnNameIndex, const QMap<QString, QString> &translation);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    int indexForColumnName(const QString &column);

    QString nameForColumnIndex(int index);

    virtual QVariant data(const QModelIndex &index, int role) const;

    inline QVariant data(int row, int column, int role) const {return data(index(row, column), role); }

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    virtual bool setData(int row, int column, const QVariant &value, int role = Qt::EditRole);

    void setRowToUpdate(int row, const QString &field = "", const QVariant &value = QVariant(), const QColor &rowColor = QColor::fromRgb(210, 150, 60));

    void setRowColor(int row, const QColor &color);

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    void setEditableFlag(int column, Qt::ItemFlags flag);

    void insertRow(int row);

    void removeRow(int row, const QModelIndex &parent = QModelIndex());

    QList<QVariant> getRowValues(int row);

    void saveDataChanges();

    inline bool hasUpdates() {return fRowToUpdate.count() > 0;}

    void setFilter(int column, const QString &filter);

    void removeFilter(int column);

    void clearFilter();

    void uniqueValuesForColumn(int column, QSet<QString> &values);

    void sumForColumns(const QStringList &columns, QMap<QString, double> &values);

    QMap<QString, int> fColumnNameIndex;

    QMap<int, QString> fColumnIndexName;

    QString fTableForUpdate;

    QList<int> fColumnsForUpdate;

    QList<QList<QVariant> > fRawData;

private:
    C5Database &fDb;

    QList<int> fProxyData;

    QMap<int, QList<QColor> > fColorData;

    QMap<QString, QString> fTranslateColumn;

    QMap<int, Qt::ItemFlags> fEditableFlags;

    QSet<int> fRowToUpdate;

    QMap<int, QMap<QString, QVariant> > fAddDataToUpdate;

    QMap<int, QString> fFilters;

    void clearModel();

    inline QVariant dataDisplay(int row, int column) const;

    void filterData();

};

#endif // C5TABLEMODEL_H
