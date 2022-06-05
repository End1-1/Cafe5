#ifndef C5TABLEMODEL_H
#define C5TABLEMODEL_H

#include <QAbstractTableModel>
#include <QSet>
#include <QColor>
#include <QHash>
#include <QVariant>
#include <QFont>

class QTableView;

class C5TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    C5TableModel(const QStringList &dbParams, QObject *parent = 0);

    void translate(const QHash<QString, QString> &t);

    void execQuery(const QString &query);

    void setCheckboxes(bool v);

    void setSingleCheckBoxSelection(bool v);

    void sort(int column);

    void setExternalData(const QHash<QString, int> &columnNameIndex, const QHash<QString, QString> &translation);

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

    void insertColumn(int column);

    void insertColumn(int column, const QString &header);

    void removeRow(int row, const QModelIndex &parent = QModelIndex());

    QList<QVariant> getRowValues(int row);

    void saveDataChanges();

    inline bool hasUpdates() {return fRowToUpdate.count() > 0;}

    void setFilter(int column, const QString &filter);

    void removeFilter(int column);

    void clearFilter();

    void uniqueValuesForColumn(int column, QSet<QString> &values);

    void sumForColumns(const QStringList &columns, QMap<QString, double> &values);

    void insertSubTotals(int column, const QList<int> &totalCols, bool title, bool insertempty);

    void resetProxyData();

    QHash<QString, int> fColumnNameIndex;

    QHash<int, QString> fColumnIndexName;

    QString fTableForUpdate;

    QList<int> fColumnsForUpdate;

    QList<QList<QVariant> > fRawData;

private:
    QStringList fDBParams;

    QTableView *fTableView;

    bool fSortAsc;

    int fLastSortedColumn;

    bool fCheckboxes;

    bool fSingleCheckBoxSelection;

    QList<int> fProxyData;

    QMap<int, QList<QColor> > fColorData;

    QHash<QString, QString> fTranslateColumn;

    QMap<int, Qt::ItemFlags> fEditableFlags;

    QSet<int> fRowToUpdate;

    QMap<int, QMap<QString, QVariant> > fAddDataToUpdate;

    QMap<int, QString> fFilters;

    QHash<QModelIndex, QFont> fCellFont;

    void clearModel();

    inline QVariant dataDisplay(int row, int column) const;

    void filterData();

};

#endif // C5TABLEMODEL_H
