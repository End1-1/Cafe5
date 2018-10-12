#include "c5permissions.h"
#include "c5config.h"

QList<int> C5Permissions::fPermissions;
QList<int> C5Permissions::fTemplate;

C5Permissions::C5Permissions()
{

}

void C5Permissions::init(C5Database &db)
{
    clear();
    fTemplate << cp_append_database
              << cp_t1_preference
              << cp_t1_usergroups
              << cp_t1_users
              << cp_t2_action
                 ;
    if (__usergroup == 1) {
        fPermissions = fTemplate;
        return;
    }
    db[":f_group"] = __usergroup;
    db.exec("select f_key from s_user_access where f_group=:f_group");
    while (db.nextRow()) {
        fPermissions << db.getInt(0);
    }
}

void C5Permissions::clear()
{
    fPermissions.clear();
    fTemplate.clear();
}

bool pr(int permission)
{
    return C5Permissions::fPermissions.contains(permission);
}
