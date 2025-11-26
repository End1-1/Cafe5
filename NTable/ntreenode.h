#pragma once

#include <QVariant>
#include <QVector>

class NTreeNode
{
public:
    QList<QVariant> values;
    QList<NTreeNode*> children;
    NTreeNode* parent;
    NTreeNode(NTreeNode *p = nullptr);
    NTreeNode(const QList<QVariant>& rowValues, NTreeNode *p = nullptr);
    ~NTreeNode();
};
