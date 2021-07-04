#ifndef DBSERVICEVALUES_H
#define DBSERVICEVALUES_H

#include "dbdata.h"

class DbServiceValues : public DbData
{
public:
    DbServiceValues();

    double value(int id);
};

#endif // DBSERVICEVALUES_H
