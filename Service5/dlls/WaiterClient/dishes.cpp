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
        break;
    case version2:
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
        break;
    case version2:
        break;
    case version3:
        nt.execSQL("select f_id, f_part, f_textcolor, f_bgcolor, f_queue, f_en from r_dish_type");
        break;
    }

    if (nt.fRowCount > 0) {
        return;
    }

    rm.putUByte(0);
    rm.putString("Empty dish part 2");
}
