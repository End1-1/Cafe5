#include "c5document.h"
#include "c5database.h"

C5Document::C5Document(QWidget *parent) :
    C5Widget(parent)
{
}

int C5Document::genNumber(int docType) const
{
    C5Database db;
    db[":f_id"] = docType;
    db.exec("select f_counter + 1 from a_type where f_id=:f_id for update");
    if (!db.nextRow()) {
        db[":f_id"] = docType;
        db[":f_counter"] = 1;
        db[":f_name"] = tr("Cash doc");
        db.insert("a_type", false);
        db[":f_id"] = docType;
        db.exec("select f_counter from a_type where f_id=:f_id for update");
        db.nextRow();
    }
    return db.getInt(0);
}

QString C5Document::genNumberText(int docType) const
{
    return QString::number(genNumber(docType));
}

void C5Document::updateGenNumber(int i, int docType)
{
    C5Database db;
    db[":f_id"] = docType;
    db[":f_counter"] = i;
    db.exec("update a_type set f_counter=:f_counter where f_id=:f_id and f_counter<:f_counter");
}
