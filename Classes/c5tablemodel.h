#ifndef C5TABLEMODEL_H
#define C5TABLEMODEL_H

#include "c5database.h"
#include <QAbstractTableModel>

class C5TableModel : public QAbstractTableModel
{
public:
    C5TableModel(C5Database &db, QObject *parent = 0);

    void translate(const QMap<QString, QString> &t);

    void execQuery(const QString &query);

    virtual int rowCount(const QModelIndex &parent) const;

    virtual int columnCount(const QModelIndex &parent) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    virtual QVariant data(const QModelIndex &index, int role) const;

    QMap<QString, int> fColumnNameIndex;

    QMap<int, QString> fColumnIndexName;

private:
    C5Database &fDb;

    QList<QList<QVariant> > fRawData;

    QList<int> fProxyData;

    QMap<QString, QString> fTranslateColumn;

    void clearModel();

    inline QVariant dataDisplay(int row, int column) const {return fRawData.at(fProxyData.at(row)).at(column); }

};

#endif // C5TABLEMODEL_H
