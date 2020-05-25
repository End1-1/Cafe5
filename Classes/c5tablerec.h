#ifndef C5TABLEREC_H
#define C5TABLEREC_H


#include "c5database.h"

class C5TableRec
{
public:
    C5TableRec(C5Database &db);

private:
    C5Database &fDb;
};

#endif // C5TABLEREC_H
