#include "dict_goods_types.h"
#include <QCoreApplication>

const QVector<int> goods_types = {GOODS_TYPE_GOODS,
                                  GOODS_TYPE_DISH,
                                  GOODS_TYPE_SERVICE,
                                  GOODS_TYPE_MODIFICATOR,
                                  GOODS_TYPE_PACKAGE,
                                  GOODS_TYPE_GUEST};

const QMap<int, const char *> goods_type_names = {
    {GOODS_TYPE_GOODS, QT_TRANSLATE_NOOP("GoodsType", "Goods")},
    {GOODS_TYPE_DISH, QT_TRANSLATE_NOOP("GoodsType", "Dish")},
    {GOODS_TYPE_SERVICE, QT_TRANSLATE_NOOP("GoodsType", "Service")},
    {GOODS_TYPE_MODIFICATOR, QT_TRANSLATE_NOOP("GoodsType", "Modificator")},
    {GOODS_TYPE_PACKAGE, QT_TRANSLATE_NOOP("GoodsType", "Package")},
    {GOODS_TYPE_GUEST, QT_TRANSLATE_NOOP("GoodsType", "Package member")},
};

QString goodsTypeDisplayName(int typeId)
{
    const char *const nm = goods_type_names.value(typeId);
    if (!nm) {
        return {};
    }
    return QCoreApplication::translate("GoodsType", nm);
}
