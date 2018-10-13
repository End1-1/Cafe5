#ifndef C5TABLEMODEL_H
#define C5TABLEMODEL_H

#include "c5database.h"
#include <QAbstractTableModel>
#include <QSet>

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

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    void setRowToUpdate(int row, const QString &field = "", const QVariant &value = QVariant(), const QColor &rowColor = QColor::fromRgb(210, 150, 60));

    void setRowColor(int row, const QColor &color);

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    void setEditableFlag(int column, Qt::ItemFlags flag);

    void insertRow(int row);

    void saveDataChanges();

    inline bool hasUpdates() {return fRowToUpdate.count() > 0;}

    QMap<QString, int> fColumnNameIndex;

    QMap<int, QString> fColumnIndexName;

    QString fTableForUpdate;

    QList<int> fColumnsForUpdate;

private:
    C5Database &fDb;

    QList<QList<QVariant> > fRawData;

    QList<int> fProxyData;

    QMap<int, QList<QColor> > fColorData;

    QMap<QString, QString> fTranslateColumn;

    QMap<int, Qt::ItemFlags> fEditableFlags;

    QSet<int> fRowToUpdate;

    QMap<int, QMap<QString, QVariant> > fAddDataToUpdate;

    void clearModel();

    inline QVariant dataDisplay(int row, int column) const {return fRawData.at(fProxyData.at(row)).at(column); }

};

#endif // C5TABLEMODEL_H
