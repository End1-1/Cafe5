#include "ntreemodel.h"
#include "c5utils.h"
#include <QColor>
#include <QFont>

NTreeModel::NTreeModel(QObject* parent)
    : QAbstractItemModel(parent),
      mSortOrder(Qt::AscendingOrder),
      mLastColumnSorted(-1)
{
    m_root = new NTreeNode();
}

NTreeModel::~NTreeModel()
{
    delete m_root;
}

void NTreeModel::setRoot(NTreeNode* node)
{
    beginResetModel();
    delete m_root;
    m_root = node;
    endResetModel();
}

bool NTreeModel::hasChildren(const QModelIndex &parent) const
{
    bool hc = false;

    if(!parent.isValid()) {
        hc = !m_root->children.isEmpty();
    } else {
        NTreeNode* node = static_cast<NTreeNode*>(parent.internalPointer());
        hc = !node->children.isEmpty();
    }

    qDebug() << "HasCHildreN" << hc;
    return hc;
}

Qt::ItemFlags NTreeModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    NTreeNode *n = static_cast<NTreeNode*>(index.internalPointer());
    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if(n->children.isEmpty()) {
        f |= Qt::ItemNeverHasChildren;
    }

    return f;
}

QModelIndex NTreeModel::index(int row, int column, const QModelIndex& parentIdx) const
{
    if(row < 0 || column < 0)
        return QModelIndex();

    NTreeNode *parentNode = parentIdx.isValid()
                            ? static_cast<NTreeNode*>(parentIdx.internalPointer())
                            : m_root;
    // TOP LEVEL → берём детей из mFilteredRootChildren
    const QList<NTreeNode*>& list =
        (parentNode == m_root ? mFilteredRootChildren : parentNode->children);

    if(row >= list.size())
        return QModelIndex();

    return createIndex(row, column, list[row]);
}

QModelIndex NTreeModel::parent(const QModelIndex& idx) const
{
    if(!idx.isValid())
        return QModelIndex();

    NTreeNode *node = static_cast<NTreeNode*>(idx.internalPointer());
    NTreeNode *parentNode = node->parent;

    if(!parentNode || parentNode == m_root)
        return QModelIndex();

    int row = parentNode->parent == m_root
              ? mFilteredRootChildren.indexOf(parentNode)
              : parentNode->parent->children.indexOf(parentNode);
    return createIndex(row, 0, parentNode);
}

int NTreeModel::rowCount(const QModelIndex& parent) const
{
    if(!parent.isValid()) {
        return mFilteredRootChildren.size();
    }

    NTreeNode *node = static_cast<NTreeNode*>(parent.internalPointer());
    return node->children.size();
}

int NTreeModel::columnCount(const QModelIndex &parent) const
{
    return mMaxColumns;
}

QVariant NTreeModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    NTreeNode* node = static_cast<NTreeNode*>(index.internalPointer());

    if(!node)
        return QVariant();

    if(role == Qt::BackgroundRole) {
        // перебираем все колонки которые являются цветовыми
        for(int colorCol : mRowColors) {
            if(colorCol < node->values.size()) {
                QVariant v = node->values[colorCol];
                bool ok = false;
                int rgb = v.toInt(&ok);

                // цвет задан и цвет не -1
                if(ok) {
                    return QColor::fromRgb(rgb);
                }
            }
        }

        return QVariant();
    }

    // --- ЖИРНЫЙ ШРИФТ ДЛЯ РОДИТЕЛЕЙ ---
    if(role == Qt::FontRole) {
        QFont f;

        if(node->parent == m_root)
            f.setBold(true);

        return f;
    }

    // --- ВСЕ ОСТАЛЬНЫЕ РОЛИ ---
    if(role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    int col = index.column();

    if(col >= node->values.size())
        return QVariant();   // пусто для отсутствующих полей

    return node->values[col];
}

QVariant NTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(role) {
    case Qt::DisplayRole:
        if(orientation == Qt::Vertical) {
            return section + 1;
        } else {
            return mColumnsNames[section].toString();
        }

    case Qt::DecorationRole:
        if(orientation == Qt::Horizontal) {
            //            if (fFilters.contains(section)) {
            //                return QIcon(":/filter_set.png");
            //            }
        }
    }

    return QVariant();
}

void NTreeModel::sort(int column, Qt::SortOrder order)
{
    if(!m_root)
        return;

    // клик по другой колонке — сортировка становится Asc
    if(mLastColumnSorted >= 0 && mLastColumnSorted != column) {
        order = Qt::AscendingOrder;
    } else {
        // повторный клик — меняем порядок
        order = (mSortOrder == Qt::AscendingOrder)
                ? Qt::DescendingOrder
                : Qt::AscendingOrder;
    }

    mLastColumnSorted = column;
    mSortOrder = order;
    beginResetModel();
    std::sort(
        m_root->children.begin(),
        m_root->children.end(),
    [column, order](NTreeNode * a, NTreeNode * b) {
        QVariant va = a->values.value(column);
        QVariant vb = b->values.value(column);

        // типы сравниваем культурно
        switch(va.typeId()) {
        case QMetaType::Int:
        case QMetaType::LongLong:
        case QMetaType::Long:
            return (order == Qt::AscendingOrder)
                   ? va.toLongLong() < vb.toLongLong()
                   : va.toLongLong() > vb.toLongLong();

        case QMetaType::Double:
            return (order == Qt::AscendingOrder)
                   ? va.toDouble() < vb.toDouble()
                   : va.toDouble() > vb.toDouble();

        default:
            return (order == Qt::AscendingOrder)
                   ? va.toString() < vb.toString()
                   : va.toString() > vb.toString();
        }
    }
    );
    endResetModel();
}

void NTreeModel::setDatasource(const QJsonArray &jcols, const QJsonArray &jchildcols, const QJsonArray &ja)
{
    beginResetModel();
    mColumnsNames = jcols;
    mChildColumnsName =  jchildcols;
    delete m_root;
    m_root = new NTreeNode(nullptr);
    mMaxColumns = jcols.size(); // ← минимум
    std::function<void(const QJsonArray&, NTreeNode*)> buildTree;
    buildTree = [&](const QJsonArray & arr, NTreeNode * parent) {
        for(const QJsonValue &v : arr) {
            QJsonObject o = v.toObject();
            QJsonArray data = o["data"].toArray();
            QJsonArray children = o["children"].toArray();
            // создаём узел
            NTreeNode *node = new NTreeNode(parent);

            // записываем данные
            for(const QJsonValue &dv : data)
                node->values.append(dv.toVariant());

            // обновляем maxColumns
            if(node->values.size() > mMaxColumns)
                mMaxColumns = node->values.size();

            parent->children.append(node);

            // рекурсивно
            if(!children.isEmpty())
                buildTree(children, node);
        }
    };
    buildTree(ja, m_root);
    // создаем проекцию (proxy rows) только 1-го уровня
    mFilteredRootChildren.clear();
    mFilteredRootChildren = m_root->children;
    endResetModel();
}

NTreeNode* NTreeModel::parseNode(const QJsonObject &obj)
{
    auto *node = new NTreeNode();
    // data: [...]
    QJsonArray data = obj["data"].toArray();

    for(int i = 0; i < data.size(); i++) {
        node->values.append(data[i].toVariant());
    }

    // children: [...]
    QJsonArray children = obj["children"].toArray();

    for(const QJsonValue &child : children) {
        node->children.append(parseNode(child.toObject()));
    }

    return node;
}

void NTreeModel::setSumColumns(const QJsonArray &jcolsum)
{
    fColSum.clear();
    fColumnsOfDouble.clear();

    for(int i = 0; i < jcolsum.size(); i++) {
        QJsonObject jo = jcolsum.at(i).toObject();

        if(jo.keys().isEmpty()) {
            continue;
        }

        fColSum[jo.keys().first().toInt()] = 0;
        fColumnsOfDouble.append(jcolsum.at(i).toInt());
    }

    countSum();
}

void NTreeModel::setFilter(const QString &filter)
{
    beginResetModel();
    // очищаем отображаемые элементы (только верхний уровень)
    mFilteredRootChildren.clear();

    if(filter.trimmed().isEmpty()) {
        // если фильтр пустой → показываем всё
        mFilteredRootChildren = m_root->children;
        endResetModel();
        return;
    }

    QString f = filter.trimmed();

    // фильтруем root->children
    for(NTreeNode *node : m_root->children) {
        bool match = false;

        // проверяем только values родителя (строка)
        for(const QVariant &v : node->values) {
            if(v.toString().contains(f, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }

        if(match)
            mFilteredRootChildren.append(node);
    }

    endResetModel();
}

void NTreeModel::setColumnFilter(const QString &filter, int column)
{
    beginResetModel();
    mFilteredRootChildren.clear();
    QStringList filters = filter.split("|", Qt::SkipEmptyParts);

    for(NTreeNode* node : m_root->children) {
        if(column >= 0 && column < node->values.size()) {
            if(filters.contains(node->values[column].toString())) {
                mFilteredRootChildren.append(node);
            }
        }
    }

    endResetModel();
}

const QSet<QString>& NTreeModel::uniqueValuesForColumn(int column)
{
    if(!mUniqueValuesForColumn.contains(column)) {
        mUniqueValuesForColumn[column] = QSet<QString>();
    }

    QSet<QString>& s = mUniqueValuesForColumn[column];

    // Уже рассчитано?
    if(!s.isEmpty())
        return s;

    // Берем данные только из верхнего уровня
    for(NTreeNode* node : mFilteredRootChildren) {
        if(column >= 0 && column < node->values.size()) {
            s.insert(node->values[column].toString());
        }
    }

    return s;
}

void NTreeModel::countSum()
{
    // Обнуляем старые суммы
    for(auto it = fColSum.begin(); it != fColSum.end(); ++it) {
        it.value() = 0;
    }

    // Считаем только верхний уровень (как и было в таблице)
    for(NTreeNode* node : mFilteredRootChildren) {
        for(auto it = fColSum.begin(); it != fColSum.end(); ++it) {
            int col = it.key();

            if(col >= 0 && col < node->values.size()) {
                it.value() += node->values[col].toDouble();
            }
        }
    }

    // Обработка спец-формул
    for(int i = 0; i < fSumColumnsSpecial.size(); i++) {
        QJsonObject jo = fSumColumnsSpecial.at(i).toObject();
        QString formula = jo["formula"].toString();
        int targetCol = jo["column"].toInt();

        if(formula == "totaldiff") {
            double d = 0;
            QJsonArray diffCols = jo["diffcols"].toArray();

            for(auto v : diffCols) {
                int c = v.toInt();
                d += fColSum[c];
            }

            fColSum[targetCol] = d;
        } else if(formula == "percent") {
            QJsonArray cols = jo["cols"].toArray();
            double a = fColSum[cols.at(0).toInt()];
            double b = fColSum[cols.at(1).toInt()];
            fColSum[targetCol] = (b == 0 ? 0 : (a / b * 100.0));
        }
    }
}

void NTreeModel::buildChildren(NTreeNode *parent, const QJsonArray &children)
{
    for(const QJsonValue &childValue : children) {
        const QJsonObject obj = childValue.toObject();
        const QJsonArray data = obj["data"].toArray();
        QList<QVariant> row;
        row.reserve(data.size());

        for(const QJsonValue &v : data)
            row.append(v.toVariant());

        auto *node = new NTreeNode(row, parent);
        parent->children.append(node);
    }
}
