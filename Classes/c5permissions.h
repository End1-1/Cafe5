#ifndef C5PERMISSIONS_H
#define C5PERMISSIONS_H

#include "c5database.h"
#include <QList>

#define cp_append_database 1

#define cp_t1_preference 100
#define cp_t1_usergroups 101
#define cp_t1_users 102
#define cp_t1_databases 103
#define cp_t1_settigns 104

#define cp_t2_action 200

#define cp_t3_reports 300
#define cp_t3_sales_common 301

#define cp_t4_menu 400
#define cp_t4_menu_names 401
#define cp_t4_part1 402
#define cp_t4_part2 403
#define cp_t4_dishes 404

#define cp_t5_waiter 500
#define cp_t5_multiple_receipt 501
#define cp_t5_remove_printed_service 502


class C5Permissions
{
public:
    C5Permissions();
    static QList<int> fTemplate;
    static void init(C5Database &db);
    static void clear();
    static QMap<QString, QList<int> > fPermissions;
};

bool pr(const QString &db, int permission);

#endif // C5PERMISSIONS_H
