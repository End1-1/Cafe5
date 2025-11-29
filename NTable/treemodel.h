#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>

class TreeModel : public QAbstractItemModel {
  public:
    explicit TreeModel(QObject *parent = nullptr);
};

#endif // TREEMODEL_H
