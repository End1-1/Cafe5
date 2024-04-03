#ifndef NTABLEMODEL_H
#define NTABLEMODEL_H

#include <QAbstractTableModel>
#include <QJsonArray>

class NTableModel : public QAbstractTableModel
{
public:
    NTableModel(QObject *parent = 0);

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    virtual int rowCount(const QModelIndex &index = QModelIndex()) const;

    virtual int columnCount(const QModelIndex &index = QModelIndex()) const;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QVariant data(int row, int column, int role = Qt::DisplayRole) const;

    void setDatasource(const QJsonArray &jcols, const QJsonArray &ja);

private:
    QJsonArray mColumnsNames;

    QJsonArray mRawData;
};

#endif // NTABLEMODEL_H
