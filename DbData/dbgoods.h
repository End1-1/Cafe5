#ifndef DBGOODS_H
#define DBGOODS_H

#include "dbdata.h"

class DbGoodsGroup;
class DbUnit;

class DbGoods : public DbData
{
public:
    DbGoods();

    DbGoods(int id);

    ~DbGoods();

    int groupCode();

    int unitCode();

    bool isService();

    int storeGoods();

    QString goodsName();

    QString goodsFiscalName();

    QString scancode(int id);

    QString scancode();

    int canDiscount();

    bool acceptIntegerQty();

    double retailPrice();

    double whosalePrice();

    double qtyBox();

    double lastInputPrice();

    bool writeStoreInputBeforeSale();

    int idOfScancode(const QString &code);

    DbGoodsGroup *group();

    DbUnit *unit();

private:
   DbGoodsGroup *fGroup;

   DbUnit *fUnit;

};

#endif // DBGOODS_H
