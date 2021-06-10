#include "c5userauth.h"
#include "c5utils.h"

C5UserAuth::C5UserAuth(C5Database &db, QObject *parent) :
    QObject(parent),
    fDb(db)
{

}

bool C5UserAuth::authByPinPass(const QString &pin, const QString &pass, int &id, int &group, QString &name)
{
    fDb[":f_login"] = pin;
    fDb[":f_password"] = password(pass);
    fDb.exec("select f_id, f_group, f_state, concat(f_last, ' ', f_first) as f_name from s_user where f_login=:f_login and f_altpassword=:f_password");
    if (fDb.nextRow()) {
        if (fDb.getInt(2) == 2){
            fError = tr("User is inactive");
        } else {
            id = fDb.getInt("f_id");
            group = fDb.getInt("f_group");
            name = fDb.getString("f_name");
        }
    } else {
        fError = tr("Login failed");
    }
    return fError.isEmpty();
}

bool C5UserAuth::authByPass(const QString &pass, int &id, int &group, QString &name)
{
    fDb[":f_altpassword"] = password(pass);
    fDb.exec("select f_id, f_group, f_state, concat(f_last, ' ', f_first) as f_name from s_user where f_altpassword=:f_altpassword");
    if (fDb.nextRow()) {
        if (fDb.getInt(2) == 2){
            fError = tr("User is inactive");
        } else {
            id = fDb.getInt("f_id");
            group = fDb.getInt("f_group");
            name = fDb.getString("f_name");
        }
    } else {
        fError = tr("Login failed");
    }
    return fError.isEmpty();
}

bool C5UserAuth::enterWork(const QString &pass)
{
    int id, group;
    QString name;
    if (!authByPass(pass, id, group, name)) {
        return false;
    }
    fDb[":f_user"] = id;
    fDb.exec("select * from s_salary_inout where f_user=:f_user and f_dateout is null");
    if (fDb.nextRow()) {
        fError = tr("Cannot input without output");
        return false;
    } else {
        fDb[":f_user"] = id;
        fDb[":f_datein"] = QDate::currentDate();
        fDb[":f_timein"] = QTime::currentTime();
        fDb.insert("s_salary_inout", false);
    }
    return true;
}

bool C5UserAuth::leaveWork(const QString &pass)
{
    int id, group;
    QString name;
    if (!authByPass(pass, id, group, name)) {
        return false;
    }
    fDb[":f_user"] = id;
    fDb.exec("select * from s_salary_inout where f_user=:f_user and f_dateout is null");
    if (fDb.nextRow()) {
        fDb[":f_user"] = fDb.getInt("f_id");
        fDb[":f_dateout"] = QDate::currentDate();
        fDb[":f_timeout"] = QTime::currentTime();
        fDb.update("s_salary_inout", where_id(fDb.getInt("f_id")));
    } else {
        fError = tr("Cannot output without input");
        return false;
    }
    return true;
}

QString C5UserAuth::error() const
{
    return fError;
}
