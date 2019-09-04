#include "user.h"
#include <QJsonArray>

User *__user = new User();

User::User()
{
    
}

bool User::pr(int permission)
{
    return fPermissions.contains(permission) || fData["fgroup"].toString().toInt() == 1;
}

void User::reg(const QJsonObject &jo)
{
    fPermissions.clear();
    fData = jo["user"].toArray().at(0).toObject();
    for (int i = 0; i < jo["role"].toArray().count(); i++) {
        fPermissions.append(jo["role"].toArray().at(i)["frole"].toString().toInt());
    }
}

void User::unreg()
{
    fData = QJsonObject();
    fPermissions.clear();
}
