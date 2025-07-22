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
    C5TableModel(QObject *parent = 0);

    void translate(const QHash<QString, QString>& t);

    void clearModel();

    void execQuery(const QString &query, QObject *echoError);

    void setCheckboxes(bool v);

    void setSingleCheckBoxSelection(bool v);

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    void setExternalData(const QHash<QString, int>& columnNameIndex, const QHash<QString, QString>& translation);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    int indexForColumnName(const QString &column);

    QString nameForColumnIndex(int index);

    virtual QVariant data(const QModelIndex &index, int role) const override;

    inline QVariant data(int row, int column, int role) const
    {
        return data(index(row, column), role);
    }

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    virtual bool setData(int row, int column, const QVariant &value, int role = Qt::EditRole);

    void setRowToUpdate(int row, const QString &field = "", const QVariant &value = QVariant(),
                        const QColor &rowColor = QColor::fromRgb(210, 150, 60));

    void setRowColor(int row, const QColor &color);

    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setEditableFlag(int column, Qt::ItemFlags flag);

    void insertRow(int row);

    void insertColumn(int column);

    void insertColumn(int column, const QString &header);

    void removeRow(int row, const QModelIndex &parent = QModelIndex());

    QJsonArray getRowValues(int row);

    void saveDataChanges();

    inline bool hasUpdates()
    {
        return fRowToUpdate.count() > 0;
    }

    void setFilter(int column, const QString &filter);

    void removeFilter(int column);

    void clearFilter();

    void uniqueValuesForColumn(int column, QSet<QString>& values);

    void sumForColumns(const QStringList &columns, QMap<QString, double>& values);

    void sumForColumnsIndexes(const QList<int>& columns, QMap<int, double>& values);

    void insertSubTotals(QList<int> cols, const QList<int>& totalCols, bool title, bool insertempty);

    void resetProxyData();

    QHash<QString, int> fColumnNameIndex;

    QHash<int, QString> fColumnIndexName;

    QString fTableForUpdate;

    QList<int> fColumnsForUpdate;

    QMap<int, QMetaType::Type> fColumnType;

    std::vector<QJsonArray > fRawData;

private:
    QTableView* fTableView;

    bool fSortAsc;

    int fLastSortedColumn;

    bool fCheckboxes;

    bool fSingleCheckBoxSelection;

    QVector<int> fProxyData;

    QMap<int, QList<QColor> > fColorData;

    QHash<QString, QString> fTranslateColumn;

    QMap<int, Qt::ItemFlags> fEditableFlags;

    QSet<int> fRowToUpdate;

    QMap<int, QMap<QString, QVariant> > fAddDataToUpdate;

    QHash<int, QString> fFilters;

    QHash<QModelIndex, QFont> fCellFont;

    inline QString dataDisplay(int row, int column) const;

    void filterData();

    bool isRowDataEmpty(int row, const QList<int>& cols);

    QString subtotalRowData(int row, const QList<int>& cols);

};

#endif // C5TABLEMODEL_H
