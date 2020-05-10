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

QString C5UserAuth::error() const
{
    return fError;
}
