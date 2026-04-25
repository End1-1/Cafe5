#include "MultiFilterProxyModel.h"

MultiFilterProxyModel::MultiFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setSortRole(Qt::UserRole); // Сортируем по числам, а не по тексту
}

void MultiFilterProxyModel::filter(const QString &needle, const QList<int> &columns)
{
    m_filterNeedle = needle.trimmed();
    m_targetColumns = columns;
    invalidateFilter();
}

bool MultiFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (m_filterNeedle.isEmpty())
        return true;

    // Если список колонок пуст, проверяем все доступные
    QList<int> cols = m_targetColumns;
    if (cols.isEmpty()) {
        for (int i = 0; i < sourceModel()->columnCount(source_parent); ++i) {
            cols << i;
        }
    }

    for (int col : cols) {
        QModelIndex idx = sourceModel()->index(source_row, col, source_parent);
        // Ищем в отформатированном тексте (DisplayRole)
        QString data = sourceModel()->data(idx, Qt::DisplayRole).toString();
        if (data.contains(m_filterNeedle, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}
