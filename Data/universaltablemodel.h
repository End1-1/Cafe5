#ifndef UNIVERSALTABLEMODEL_H
#define UNIVERSALTABLEMODEL_H

#include <QAbstractTableModel>
#include <QLocale>
#include <QVariant>
#include <QVector>

class UniversalTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit UniversalTableModel(QObject *parent = nullptr);

    // Управление данными
    void updateData(const QVector<QVector<QVariant>> &newData, const QStringList &headers);

    // Методы Qt Model API
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QVector<QVector<QVariant>> m_tableData;
    QStringList m_headers;
    QLocale m_locale;
};

#endif // UNIVERSALTABLEMODEL_H
