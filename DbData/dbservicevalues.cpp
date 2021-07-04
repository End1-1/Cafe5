#include "dbservicevalues.h"

DbServiceValues::DbServiceValues() :
    DbData("o_service_values")
{

}

double DbServiceValues::value(int id)
{
    return get(id, "f_value").toDouble();
}
