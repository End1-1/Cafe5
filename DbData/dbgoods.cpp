#include "dbgoods.h"
#include "datadriver.h"

DbGoods::DbGoods() :
    DbData("c_goods")
{
    fGroup = nullptr;
    fUnit = nullptr;
}

DbGoods::DbGoods(int id) :
    DbData(id)
{
    fGroup = new DbGoodsGroup(groupCode());
    fUnit = new DbUnit(unitCode());
}

DbGoods::~DbGoods()
{
    if (fGroup) {
        delete fGroup;
    }
    if (fUnit) {
        delete fUnit;
    }
}

int DbGoods::groupCode()
{
    return dbgoods->get(fId, "f_group").toInt();
}

int DbGoods::unitCode()
{
    return dbgoods->get(fId, "f_unit").toInt();
}

bool DbGoods::isService()
{
    return dbgoods->get(fId, "f_service").toInt() == 1;
}

QString DbGoods::goodsName()
{
    return dbgoods->name(fId);
}

QString DbGoods::scancode(int id)
{
    return get(id, "f_scancode").toString();
}

QString DbGoods::scancode()
{
    return dbgoods->get(fId, "f_scancode").toString();
}

bool DbGoods::acceptIntegerQty()
{
    return dbgoods->get(fId, "f_wholenumber").toInt() == 1;
}

double DbGoods::retailPrice()
{
    return dbgoods->get(fId, "f_saleprice").toDouble();
}

double DbGoods::whosalePrice()
{
    return dbgoods->get(fId, "f_saleprice2").toDouble();
}

double DbGoods::lastInputPrice()
{
    return dbgoods->get(fId, "f_lastinputprice").toDouble();
}

bool DbGoods::writeStoreInputBeforeSale()
{
    return dbgoods->get(fId, "f_storeinputbeforesale").toInt() == 1;
}

int DbGoods::idOfScancode(const QString &code)
{
    for (int id: list()) {
        if (scancode(id) == code) {
            return id;
        }
    }
    return 0;
}

DbGoodsGroup *DbGoods::group()
{
    return fGroup;
}

DbUnit *DbGoods::unit()
{
    return fUnit;
}
