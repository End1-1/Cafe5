#include "dishes.h"
#include "translator.h"
#include "networktable.h"
#include "ops.h"

void loadPart1(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    NetworkTable nt(rm, db);
    rm.readUByte(version, in);
    switch (version) {
    case version1:
        nt.execSQL("select id, name from me_group_types");
        break;
    case version2:
        nt.execSQL("select f_id, f_name from d_part1");
        break;
    case version3:
        nt.execSQL("select f_id, f_en from r_dish_part");
        break;
    }

    if (nt.fRowCount > 0) {
        return;
    }

    rm.putUByte(0);
    rm.putString("Empty dish part 1");
}

void loadPart2(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    NetworkTable nt(rm, db);
    rm.readUByte(version, in);
    switch (version) {
    case version1:
        nt.execSQL("select distinct(dt.id), 0 as f_parentid, dt.part_id, "
                   "-16777216 as f_textcolor, -1 as f_bgcolor, dt.name, 1 "
                   "from me_types dt "
                   "inner join me_dishes d on d.type_id=dt.id "
                   "inner join me_dishes_menu m on m.dish_id=d.id "
                   "where m.state_id=1 "
                   "order by dt.name "  );
        break;
    case version2:
        nt.execSQL("select distinct(p2.f_id), p2.f_parent, p2.f_part, "
                   "-16777216 as f_textcolor, -1 as f_bgcolor, p2.f_name, 1 "
                    "from d_part2 p2 "
                    "inner join d_dish d on d.f_part=p2.f_id "
                    "inner join d_menu m on m.f_dish=d.f_id "
                    "where m.f_state=1 "
                    "order by p2.f_name ");
        break;
    case version3:
        nt.execSQL("select distinct(dt.f_id), 0 as f_parentid, dt.f_part, dt.f_textcolor, dt.f_bgcolor, dt.f_en, dt.f_queue "
                   "from r_dish_type dt "
                   "inner join r_dish d on d.f_type=dt.f_id "
                   "inner join r_menu m on m.f_dish=d.f_id "
                   "where m.f_state=1 " );
        break;
    }

    if (nt.fRowCount > 0) {
        return;
    }

    rm.putUByte(0);
    rm.putString("Empty dish part 2");
}

void loadDishes(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    NetworkTable nt(rm, db);
    rm.readUByte(version, in);
    switch (version) {
    case version1:
        nt.execSQL("select id, type_id, -1 as f_bgcolor, -16777216 as f_textcolor, name, queue, quick_list from me_dishes "
                    "where id in (select distinct(dish_id) from me_dishes_menu where state_id=1) ");
        break;
    case version2:
        nt.execSQL("select distinct(d.f_id), d.f_part, d.f_color as f_bgcolor, -16777216 as f_textcolor, "
                    "d.f_name, d.f_queue, m.f_recent "
                    "from d_dish d "
                    "inner join d_menu m on m.f_dish=d.f_id and m.f_state=1 ");
        break;
    case version3:
        nt.execSQL("select f_id, f_type, f_bgcolor, f_textcolor, f_en, f_queue, 0 as quick_list from r_dish "
                    "where f_id in (select distinct(f_dish) from r_menu where f_state=1) ");
        break;
    }

    if (nt.fRowCount > 0) {
        return;
    }

    rm.putUByte(0);
    rm.putString("Empty dishes");
}

void loadDishMenu(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    NetworkTable nt(rm, db);
    rm.readUByte(version, in);
    switch (version) {
    case version1:
        nt.execSQL("select m.menu_Id, d.type_id, m.dish_id, m.price, m.store_id, m.print1, "
                    "m.print2 "
                    "from me_dishes_menu m "
                    "inner join me_dishes d on d.id=m.dish_id "
                    "where m.state_id=1 "
                    "order by d.queue ");
        break;
    case version2:
        nt.execSQL("select m.f_menu, d.f_part, m.f_dish, m.f_price, m.f_store, m.f_print1, "
                    "m.f_print2 "
                    "from d_menu m "
                    "inner join d_dish d on d.f_id=m.f_dish "
                    "where m.f_state=1 "
                    "order by d.f_queue");
        break;
    case version3:
        nt.execSQL("select m.f_menu, d.f_type, m.f_dish, m.f_price, m.f_store, m.f_print1, "
                    "m.f_print2 from r_menu m "
                    "inner join r_dish d on d.f_id=m.f_dish "
                    "where f_state=1 "
                    "order by d.f_queue ");
        break;
    }

    if (nt.fRowCount > 0) {
        return;
    }

    rm.putUByte(0);
    rm.putString("Empty dishes list in menu");
}

void getDishComment(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    NetworkTable nt(rm, db);
    rm.readUByte(version, in);
    switch (version) {
    case version1:
        nt.execSQL("select id, forid, name from me_dishes_comment ");
        break;
    case version2:
        nt.execSQL("select f_id, f_dish, f_comment "
                    "from d_special "
                    "union "
                    "select f_id, 0, f_name "
                    "from d_dish_comment ");
        break;
    case version3:
        nt.execSQL("select 1, 0, '-' ");
        break;
    }
}

void loadMenuNames(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    NetworkTable nt(rm, db);
    rm.readUByte(version, in);
    switch (version) {
    case version1:
        nt.execSQL("select id, name from me_menus");
        break;
    case version2:
        nt.execSQL("select f_id, f_name from d_menu_names");
        break;
    case version3:
        break;
    }
}
