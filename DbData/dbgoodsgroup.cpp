#include "dbgoodsgroup.h"
#include "datadriver.h"

DbGoodsGroup::DbGoodsGroup() :
    DbData("c_groups")
{

}

DbGoodsGroup::DbGoodsGroup(int id) :
    DbData(id)
{
}

QString DbGoodsGroup::adgt()
{
    return dbgoodsgroup->get(fId, "f_adgcode").toString();
}

QString DbGoodsGroup::taxDept()
{
    return dbgoodsgroup->get(fId, "f_taxdept").toString();
}
