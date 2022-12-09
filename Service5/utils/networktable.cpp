#include "networktable.h"
#include <QSqlRecord>
#include <QSqlField>

const quint8 datatype_int = 1;
const quint8 datatype_double = 2;
const quint8 datatype_string = 3;

NetworkTable::NetworkTable(RawMessage &rm, Database &db) :
    fRM(rm),
    fDb(db)
{
    fRowCount = 0;
}

void NetworkTable::execSQL(const QString &sql)
{
    if (!fDb.exec(sql)) {
        fRM.putUByte(0);
        fRM.putString(fDb.lastDbError());
        return;
    }
    //0: Positive answer
    fRM.putUByte(1);
    //1: columns count
    fRM.putUShort(fDb.columnCount());
    //2: rows count
    //fRowCount = fDb.rowCount();
    int rowcountpos = fRM.fReply.length();
    fRM.putUInt(fRowCount);
    //3: data types
    QSqlRecord r = fDb.fQuery->record();
    QList<int> datatype;
    for (int i = 0; i < r.count(); i++) {
        switch(r.field(i).type()) {
        case QVariant::Int:
            fRM.putUByte(datatype_int);
            datatype.append(datatype_int);
            break;
        case QVariant::Double:
            fRM.putUByte(datatype_double);
            datatype.append(datatype_double);
            break;
        case QVariant::String:
            fRM.putUByte(datatype_string);
            datatype.append(datatype_string);
            break;
        default:
            fRM.putUByte(datatype_string);
            datatype.append(datatype_string);
            break;
        }
    }
    //4: add to strings names of columns
    for (int i = 0; i < fDb.columnCount(); i++) {
        fStrings.append(fDb.columnName(i));
    }
    //5: add data to buffer. int, double added as is, strings added to stringlist and position of string list added to buffer as int
    fRowCount = 0;
    while (fDb.next()) {
        fRowCount++;
        for (int i = 0; i < datatype.count(); i++) {
        switch (datatype.at(i)) {
        case datatype_int:
            fRM.putUInt(fDb.value(i).toInt());
            break;
        case datatype_double:
            fRM.putDouble(fDb.value(i).toDouble());
            break;
        case datatype_string:
            fRM.putUInt(fStrings.count());
            fStrings.append(fDb.value(i).toString());
            break;
        }
        }
    }
    qDebug() << "ROW COUNT" << fRowCount;
    fRM.fReply.replace(rowcountpos, sizeof(fRowCount), reinterpret_cast<const char*>(&fRowCount), sizeof(fRowCount));
    //6: add strings
    qDebug() << "strings count" << fStrings.count();
    fRM.putUInt(fStrings.count());
    for (int i = 0; i < fStrings.count(); i++) {
        fRM.putString(fStrings.at(i));
    }
}
