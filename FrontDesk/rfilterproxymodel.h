#pragma once

#include <QSet>
#include <QSortFilterProxyModel>
#include <QHash>
#include <QString>

class RFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT   // <-- ОБЯЗАТЕЛЬНО

public:
    explicit RFilterProxyModel(QObject *parent = nullptr);

    void setGlobalFilter(const QString &f);
    void setColumnFilter(int col, const QString &f);
    /// Restrict column to these display string values. Empty set keeps no visible rows. Use \a clearColumnValueFilter to drop filter.
    void setColumnValueAllowList(int col, const QSet<QString> &allowed);
    void clearColumnValueFilter(int col);
    void clearAllColumnValueFilters();
    bool hasColumnValueFilter(int col) const;
    QSet<QString> columnAllowedValues(int col) const;
    void recalcSums();
    double sumForColumn(int col) const;
    QHash<int, double> columnSums;
    QList<int> numericCols;

signals:
    void sumsChanged(const QHash<int, double>&);

protected:
    bool filterAcceptsRow(int row, const QModelIndex &parent) const override;

    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:

    QString globalFilter;
    QHash<int, QString> columnFilters;
    /// When key present: row passes iff cell text is in the set. Empty set → no row passes.
    QHash<int, QSet<QString>> columnValueAllow;
};
