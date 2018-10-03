#include "c5staticdb.h"

C5Database *C5StaticDB::fStaticDb = 0;

C5StaticDB::C5StaticDB()
{
    if (!fStaticDb) {
        fStaticDb = new C5Database();
    }
}
