#ifndef DBPARTNER_H
#define DBPARTNER_H

#include "dbdata.h"

class DbPartner : public DbData
{
public:
    DbPartner();

    QString contact(int id);

    QString phone(int id);

    virtual QString name(int id);
};

#endif // DBPARTNER_H
