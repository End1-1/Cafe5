#include "RFilterProxyModel.h"
#include "c5utils.h"

static QString normalizeDecimal(QString s)
{
    s = s.trimmed();
    s.remove(' ');
    s.remove(QChar(0xA0)); // NBSP
    // US style only
    s.remove(',');  // thousand separator
    return s;
}

static bool numericMatch(QString cell, QString filter)
{
    QString cellN   = normalizeDecimal(cell);
    QString filterN = normalizeDecimal(filter).trimmed();

    if(filterN.isEmpty())
        return true;

    return cellN.contains(filterN, Qt::CaseInsensitive);
}

RFilterProxyModel::RFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void RFilterProxyModel::setGlobalFilter(const QString &f)
{
    globalFilter = f.toLower();
    invalidateFilter();
    recalcSums();
    emit sumsChanged(columnSums);
}

void RFilterProxyModel::setColumnFilter(int col, const QString &f)
{
    columnFilters[col] = f.toLower();
    invalidateFilter();
    recalcSums();
    emit sumsChanged(columnSums);
}

double RFilterProxyModel::sumForColumn(int col) const
{
    return columnSums.value(col, 0.0);
}

bool RFilterProxyModel::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    const QAbstractItemModel *m = sourceModel();
    QModelIndex idx;

    // ---- Column filters ----
    for(auto it = columnFilters.begin(); it != columnFilters.end(); ++it) {
        int col = it.key();
        const QString &filter = it.value();
        idx = m->index(row, col, parent);
        QString cell = m->data(idx).toString();

        if(numericCols.contains(col)) {
            if(!numericMatch(cell, filter))
                return false;
        } else {
            if(!cell.contains(filter, Qt::CaseInsensitive))
                return false;
        }
    }

    // ---- Global filter ----
    if(!globalFilter.isEmpty()) {
        int cols = m->columnCount(parent);

        for(int c = 0; c < cols; c++) {
            idx = m->index(row, c, parent);
            QString cell = m->data(idx).toString();

            if(numericCols.contains(c)) {
                if(numericMatch(cell, globalFilter))
                    return true;
            } else {
                if(cell.contains(globalFilter, Qt::CaseInsensitive))
                    return true;
            }
        }

        return false;
    }

    return true;
}

void RFilterProxyModel::recalcSums()
{
    int rows = rowCount();

    for(QHash<int, double>::Iterator it = columnSums.begin(); it != columnSums.end(); it++) {
        it.value() = 0;

        for(int r = 0; r < rows; r++) {
            QModelIndex idx = index(r, it.key());
            double v = str_float(data(idx).toString());
            it.value() += v;
        }
    }
}
