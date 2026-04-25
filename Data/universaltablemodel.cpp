#include "UniversalTableModel.h"

UniversalTableModel::UniversalTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_locale = QLocale::system();
}

void UniversalTableModel::updateData(const QVector<QVector<QVariant>> &newData, const QStringList &headers)
{
    beginResetModel();
    m_tableData = newData;
    m_headers = headers;
    endResetModel();
}

int UniversalTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_tableData.size();
}

int UniversalTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_headers.size();
}

QVariant UniversalTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal && section < m_headers.size()) {
        return m_headers.at(section);
    }
    return QVariant();
}

QVariant UniversalTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_tableData.size())
        return QVariant();

    const QVariant &value = m_tableData[index.row()][index.column()];

    // Красивое отображение (Локаль + разделители)
    if (role == Qt::DisplayRole) {
        bool ok;
        double d = value.toDouble(&ok);
        if (ok && value.userType() != QMetaType::QString) {
            if (d == static_cast<double>(static_cast<long long>(d)))
                return m_locale.toString(static_cast<long long>(d));
            return m_locale.toString(d, 'f', 2);
        }
        return value.toString();
    }

    // Роль для СОРТИРОВКИ (чистые данные)
    if (role == Qt::UserRole || role == Qt::EditRole) {
        return value;
    }

    // Выравнивание чисел
    if (role == Qt::TextAlignmentRole) {
        bool ok;
        value.toDouble(&ok);
        if (ok && value.userType() != QMetaType::QString) {
            // Просто добавь приведение к int, чтобы QVariant "проглотил" это
            return static_cast<int>(Qt::AlignRight | Qt::AlignVCenter);
        }
    }

    return QVariant();
}
