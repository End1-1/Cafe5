#ifndef C5PERMISSIONS_H
#define C5PERMISSIONS_H

#include "c5database.h"
#include <QList>

#define cp_append_database 1
#define cp_t1_preference 100
#define cp_t1_usergroups 101
#define cp_t1_users 102
#define cp_t2_action 200
#define cp_t3_reports 300
#define cp_t3_sales_common 301

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
