#pragma once

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

};
