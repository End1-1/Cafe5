#include "dbpartner.h"

DbPartner::DbPartner() :
    DbData("c_partners")
{

}

QString DbPartner::contact(int id)
{
    return get(id, "f_contact").toString();
}

QString DbPartner::name(int id)
{
    return get(id, "f_taxname").toString();
}
