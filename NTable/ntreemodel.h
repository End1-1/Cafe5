#pragma once

#include "ntreenode.h"
#include <QAbstractItemModel>
#include <QJsonArray>
#include <QJsonObject>

class NTreeModel : public QAbstractItemModel
{

public:
    explicit NTreeModel(QObject* parent = nullptr);

    ~NTreeModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex& idx) const override;

    void setRoot(NTreeNode* node);

    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /* FROM NTableModel */

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    void setDatasource(const QJsonArray &jcols, const QJsonArray &jchildcols, const QJsonArray &ja);

    NTreeNode* parseNode(const QJsonObject &obj);

    void setSumColumns(const QJsonArray &jcolsum);

    void setFilter(const QString &filter);

    void setColumnFilter(const QString &filter, int column);

    QMap<int, double> fColSum;

    QJsonArray fSumColumnsSpecial;

    const QSet<QString>& uniqueValuesForColumn(int column);

    QList<int> mRowColors;

    NTreeNode* m_root;

private:
    int mMaxColumns = 0;

    QList<NTreeNode*> mFilteredRootChildren;

    QMap<int, QSet<QString> > mUniqueValuesForColumn;

    QJsonArray mColumnsNames;

    QJsonArray mChildColumnsName;

    QList<int> fColumnsOfDouble;

    Qt::SortOrder mSortOrder;

    int mLastColumnSorted;

    void countSum();

    void buildChildren(NTreeNode *parent, const QJsonArray &children) ;
};
