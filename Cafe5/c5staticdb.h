#ifndef C5STATICDB_H
#define C5STATICDB_H

#include "c5database.h"

class C5StaticDB
{
public:
    C5StaticDB();
    static C5Database *fStaticDb;
};

#endif // C5STATICDB_H
