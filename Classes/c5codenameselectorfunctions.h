#pragma once
#include "struct_goods_item.h"
#include "struct_storage_item.h"
#include "c5codenameselector.h"
#include "c5structtableview.h"

template <typename T>
[[nodiscard]] inline QVector<T> selectItem(bool loadFirst = false, bool multiSelect = false, const QPoint &position = {})
{
    return C5StructTableView::get<T>(SelectorName<T>::value, loadFirst, multiSelect, position);
}

template<typename T>
auto makeSelector(std::function<void(T)> callback = nullptr)
{
    return [callback](C5CodeNameSelector * s) {
        const auto r = selectItem<T>(false, false, s->getPosition());

        if(r.isEmpty())
            return;

        const auto &g = r.first();
        s->setCodeAndName(g.id, g.name);

        if(callback)
            callback(g);
    };
}

inline auto goodsItemSelector = [](C5CodeNameSelector *s)
{
    const auto r = selectItem<GoodsItem>(false, false, s->getPosition());

    if(r.isEmpty())
        return;

    const auto &g = r.first();
    s->setCodeAndName(g.id, g.name);
};

inline auto storeItemSelector = [](C5CodeNameSelector *s)
{
    const auto r = selectItem<StorageItem>(true, false, s->getPosition());

    if(r.isEmpty())
        return;

    const auto &g = r.first();
    s->setCodeAndName(g.id, g.name);
};

inline auto mapPointToGlobal(QWidget *w) {
    return w->mapToGlobal(QPoint(0, w->height()));
}
