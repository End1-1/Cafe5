#include "dduser.h"

DDUser::DDUser()
{

}

bool DDUser::authUserPass(Database &db, const QString &email, const QString &phone, const QString &pass)
{
    db[":fphone"] = phone;
    db[":femail"] = email;
    db[":fpassword"] = pass;
    db.exec("select uuid() as session, u.* from users_list u where (u.femail=:femail or u.fphone=:fphone) and u.fpassword=:fpassword");
    if (db.next()) {
        fSession = db.stringValue("session");
        fId = db.integerValue("fid");
        return true;
    }
    return false;
}
