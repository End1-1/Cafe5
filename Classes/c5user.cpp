#include "c5user.h"
#include "c5database.h"
#include "c5config.h"
#include <QJsonArray>

C5User::C5User(const QMap<QString, QVariant> &m) :
    C5User()
{
    fUserData = m;
    fValid = true;
}

C5User::C5User(const QString &altPassword) :
    C5User()
{
    if (!authorize(altPassword)) {
        return;
    }
    fValid = true;
}

C5User::C5User(int id) :
    C5User()
{
    Q_UNUSED(id);
    //TODO: remove this constructor
}

bool C5User::isValid()
{
    return fValid;
}

QString C5User::error()
{
    return fError;
}

int C5User::id()
{
    return data("f_id").toInt();
}

QString C5User::fullName()
{
    return QString("%1 %2").arg(data("f_last").toString()).arg(data("f_first").toString());
}

int C5User::group()
{
    return data("f_group").toInt();
}

bool C5User::check(int permission)
{
    if (group() == 1) {
        return true;
    }
    if(fPermissions.contains(QString::number(permission))) {
        return true;
    }
    fError = tr("You have not permission");
    return false;
}

C5User::C5User() :
    QObject()
{
    fValid = false;
}

QVariant C5User::data(const QString &name)
{
    Q_ASSERT(fUserData.contains(name));
    return fUserData[name];
}

bool C5User::authorize(const QString &altPassword)
{
    C5Database db(__c5config.dbParams());
    db[":f_altpassword"] = altPassword;
    db.exec("select * from s_user where f_altpassword=md5(:f_altpassword)");
    if (!db.nextRow()) {
        fError = tr("Access denied");
        return false;
    }
    db.rowToMap(fUserData);
    return true;
}
