#ifndef DBSTOREDOCTYPE_H
#define DBSTOREDOCTYPE_H

#include "dbdata.h"

class DbStoreDocType : public DbData
{
public:
    DbStoreDocType();

    DbStoreDocType(int id);

    QString typeName() const;
};

#endif // DBSTOREDOCTYPE_H
