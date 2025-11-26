#include "ntreenode.h"

NTreeNode::NTreeNode(NTreeNode *p)
{
    parent = p;
}

NTreeNode::NTreeNode(const QList<QVariant>& rowValues, NTreeNode *p)
{
    parent = p;
    values = rowValues;
}

NTreeNode::~NTreeNode()
{
    qDeleteAll(children);
}
