#include "c5permissions.h"
#include "c5config.h"

QList<int> C5Permissions::fTemplate;
QMap<QString, QList<int> > C5Permissions::fPermissions;

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
              << cp_t1_databases
              << cp_t1_settigns
              << cp_t2_action
              << cp_t3_reports
              << cp_t3_sales_common
              << cp_t4_menu
              << cp_t4_menu_names
              << cp_t4_part1
              << cp_t4_part2
              << cp_t4_dishes
              << cp_t5_waiter
              << cp_t5_multiple_receipt
              << cp_t5_remove_printed_service
                 ;
    if (__usergroup == 1) {
        fPermissions[db.database()] = fTemplate;
        return;
    }
    db[":f_group"] = __usergroup;
    db.exec("select f_key from s_user_access where f_group=:f_group");
    while (db.nextRow()) {
        fPermissions[db.database()] << db.getInt(0);
    }
}

void C5Permissions::clear()
{
    fPermissions.clear();
    fTemplate.clear();
}

bool pr(const QString &db, int permission)
{
    return C5Permissions::fPermissions[db].contains(permission);
}
