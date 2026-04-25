#ifndef MULTIFILTERPROXYMODEL_H
#define MULTIFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>

class MultiFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit MultiFilterProxyModel(QObject *parent = nullptr);

    // Метод фильтрации по списку колонок
    void filter(const QString &needle, const QList<int> &columns = {});

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString m_filterNeedle;
    QList<int> m_targetColumns;
};

#endif // MULTIFILTERPROXYMODEL_H
