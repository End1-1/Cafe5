#pragma once
#include "c5codenameselector.h"
#include "c5structtableview.h"
#include "store_doc_status.h"
#include "store_doc_type.h"
#include "struct_goods_item.h"
#include "struct_partner.h"
#include "struct_storage_item.h"

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

inline auto storeDocStatusItemSelector = [](C5CodeNameSelector *s) {
    const auto r = selectItem<StoreDocStatusItem>(true, false, s->getPosition());

    if (r.isEmpty())
        return;

    const auto &g = r.first();
    s->setCodeAndName(g.id, g.name);
};

inline auto storeDocTypeItemSelector = [](C5CodeNameSelector *s) {
    const auto r = selectItem<StoreDocTypeItem>(true, false, s->getPosition());

    if (r.isEmpty())
        return;

    const auto &g = r.first();
    s->setCodeAndName(g.id, g.name);
};

inline auto partnerItemSelector = [](C5CodeNameSelector *s) {
    const auto r = selectItem<PartnerItem>(true, false, s->getPosition());
    if (r.isEmpty()) {
        return;
    }
    const auto &g = r.first();
    QStringList name;
    if (!g.taxName.isEmpty()) {
        name.append(g.taxName);
    }
    if (!g.contactName.isEmpty()) {
        name.append(g.contactName);
    }
    s->setCodeAndName(g.id, name.join(","));
};

inline auto mapPointToGlobal(QWidget *w)
{
    return w->mapToGlobal(QPoint(0, w->height()));
}
