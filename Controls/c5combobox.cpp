#include "c5combobox.h"
#include "c5database.h"
#include "c5cache.h"

C5ComboBox::C5ComboBox(QWidget *parent) :
    QComboBox(parent)
{
}

int C5ComboBox::getTag()
{
    return fTag;
}

void C5ComboBox::setTag(int tag)
{
    fTag = tag;
}

void C5ComboBox::setIndexForValue(const QVariant &value)
{
    setCurrentIndex(findData(value));
}

void C5ComboBox::setDBValues(const QString &sql, const QVariant &defaultValue)
{
    C5Database db;
    db.exec(sql);

    while(db.nextRow()) {
        addItem(db.getString(1), db.getInt(0));
    }

    if(defaultValue.isValid()) {
        setCurrentIndex(findData(defaultValue));
    }
}

void C5ComboBox::setCache(int cacheid, int colId, int colName)
{
    C5Cache *c = C5Cache::cache(cacheid);

    for(int i = 0; i < c->rowCount(); i++) {
        addItem(c->getString(i, colName), c->getInt(i, colId));
    }
}
