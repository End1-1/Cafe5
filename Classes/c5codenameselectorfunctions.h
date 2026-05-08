#pragma once
#include "c5codenameselector.h"
#include "c5structtableview.h"
#include "store_doc_status.h"
#include "store_doc_type.h"
#include "struct_cashbox.h"
#include "struct_currency.h"
#include "struct_goods_group.h"
#include "struct_goods_item.h"
#include "struct_partner.h"
#include "struct_payment_type.h"
#include "struct_storage_item.h"
#include "struct_employee.h"
#include "struct_employee_group.h"
#include "struct_goods_type.h"

template<typename T>
[[nodiscard]] inline QVector<T> selectItem(bool loadFirst = false, bool multiSelect = false, const QPoint &position = {-1, -1})
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

inline auto goodsGroupSelector = [](C5CodeNameSelector *s) {
    const auto r = selectItem<GoodsGroupItem>(false, false, s->getPosition());

    if (r.isEmpty())
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

inline auto currencyItemSelector = [](C5CodeNameSelector *s) {
    const auto r = selectItem<StructCurrency>(true, false, s->getPosition());
    if (r.isEmpty()) {
        return;
    }
    const auto &g = r.first();
    s->setCodeAndName(g.id, g.name);
};

inline auto cashboxItemSelector = [](C5CodeNameSelector *s) {
    const auto r = selectItem<StructCashbox>(true, false, s->getPosition());
    if (r.isEmpty()) {
        return;
    }
    const auto &g = r.first();
    s->setCodeAndName(g.id, g.name);
};

inline auto paymentTypeItemSelector = [](C5CodeNameSelector *s) {
    const auto r = selectItem<StructPaymentType>(true, false, s->getPosition());
    if (r.isEmpty()) {
        return;
    }
    const auto &g = r.first();
    s->setCodeAndName(g.id, g.name);
};

inline auto goodsTypeItemSelector = [](C5CodeNameSelector *s) {
    const auto r = selectItem<StructGoodsType>(true, false, s->getPosition());
    if (r.isEmpty()) {
        return;
    }
    const auto &g = r.first();
    s->setCodeAndName(g.id, g.name);
};

inline auto employeeItemSelector = [](C5CodeNameSelector *s) {
    const auto r = selectItem<StructEmployee>(true, false, s->getPosition());
    if (r.isEmpty()) {
        return;
    }
    const auto &g = r.first();
    QStringList name;
    if (!g.firstName.isEmpty() || !g.lastName.isEmpty()) {
        name.append(QString("%1 %2").arg(g.firstName, g.lastName).trimmed());
    }
    if (!g.login.isEmpty()) {
        name.append(g.login);
    }
    s->setCodeAndName(g.id, name.join(", "));
};

inline auto employeeGroupItemSelector = [](C5CodeNameSelector *s) {
    const auto r = selectItem<StructEmployeeGroup>(true, false, s->getPosition());
    if (r.isEmpty()) {
        return;
    }
    const auto &g = r.first();
    s->setCodeAndName(g.id, g.name);
};

inline auto mapPointToGlobal(QWidget *w)
{
    return w->mapToGlobal(QPoint(0, w->height()));
}
