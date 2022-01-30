#include "dbstoredoctype.h"
#include "datadriver.h"

DbStoreDocType::DbStoreDocType() :
    DbData("a_type")
{

}

DbStoreDocType::DbStoreDocType(int id) :
    DbData(id)
{

}

QString DbStoreDocType::typeName() const
{
    return dbstoredoctype->name(fId);
}
