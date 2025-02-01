#include "c5user.h"
#include "c5database.h"
#include "c5config.h"
#include "ndataprovider.h"
#include "c5permissions.h"
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
    if (id > 0) {
        loadFromDB(id);
    }
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

QVariant C5User::value(const QString &key)
{
    return fUserData[key];
}

int C5User::group()
{
    return data("f_group").toInt();
}

bool C5User::isActive()
{
    return data("f_state").toInt() == 1;
}

bool C5User::authByUsernamePass(const QString &username, const QString &pass)
{
    fError.clear();
    fValid = false;
    C5Database db(__c5config.dbParams());
    db[":f_login"] = username;
    db[":f_password"] = pass;
    db.exec("select * from s_user where f_login=:f_login and f_password=md5(:f_password)");
    if (db.nextRow()) {
        if (db.getInt("f_state") == 2) {
            fError = tr("User is inactive");
        } else {
            db.rowToMap(fUserData);
        }
    } else {
        fError = tr("Login failed");
    }
    if (fError.isEmpty()) {
        getPermissions();
        getState();
        fValid = true;
    }
    return fError.isEmpty();
}

bool C5User::authByPinPass(const QString &pin, const QString &pass)
{
    fError.clear();
    fValid = false;
    C5Database db(__c5config.dbParams());
    db[":f_login"] = pin;
    db[":f_password"] = pass;
    db.exec("select * from s_user where f_login=:f_login and f_altpassword=md5(:f_password)");
    if (db.nextRow()) {
        if (db.getInt("f_state") == 2) {
            fError = tr("User is inactive");
        } else {
            db.rowToMap(fUserData);
        }
    } else {
        fError = tr("Login failed");
    }
    if (fError.isEmpty()) {
        getPermissions();
        getState();
        fValid = true;
        NDataProvider::mHost = __c5config.dbParams().at(1);
    }
    return fError.isEmpty();
}

bool C5User::check(int permission)
{
    if(fPermissions.contains(permission)) {
        return true;
    }
    fError = tr("You have not permission");
    return false;
}

bool C5User::enterWork()
{
    if (fState == usAtWork) {
        fError = tr("Cannot input without output");
        return false;
    } else {
        C5Database db(__c5config.dbParams());
        db[":f_id"] = db.uuid();
        db[":f_user"] = id();
        db[":f_datein"] = QDate::currentDate();
        db[":f_timein"] = QTime::currentTime();
        if (!db.insert("s_salary_inout", false)) {
            fError = db.fLastError;
            return false;
        }
    }
    fState = usAtWork;
    return true;
}

bool C5User::leaveWork()
{
    C5Database db(__c5config.dbParams());
    if (fState == usAtWork) {
        db[":f_dateout"] = QDate::currentDate();
        db[":f_timeout"] = QTime::currentTime();
        if (!db.update("s_salary_inout", "f_user", id())) {
            fError = db.fLastError;
            return false;
        }
    } else {
        fError = tr("Cannot output without input");
        return false;
    }
    fState = usNotAtWork;
    return true;
}

C5User::UserState C5User::state()
{
    return fState;
}

bool C5User::loadFromDB(int id)
{
    C5Database db(__c5config.dbParams());
    db[":f_id"] = id;
    db.exec("select u.*, s.f_name as f_settingsname "
            "from s_user u "
            "left join s_settings_names s on s.f_id=u.f_config "
            "where u.f_id=:f_id ");
    if (!db.nextRow()) {
        fError = tr("Access denied");
        fValid = false;
        return false;
    }
    db.rowToMap(fUserData);
    getPermissions();
    getState();
    fValid = true;
    return true;
}

C5User::C5User() :
    QObject()
{
    fValid = false;
    fState = usNotAtWork;
}

QVariant C5User::data(const QString &name)
{
    Q_ASSERT(fUserData.contains(name));
    return fUserData[name];
}

void C5User::getState()
{
    C5Database db(__c5config.dbParams());
    db[":f_user"] = id();
    db.exec("select * from s_salary_inout where f_user=:f_user and f_dateout is null");
    fState = db.nextRow() ? usAtWork : usNotAtWork;
}

void C5User::getPermissions()
{
    fPermissions.clear();
    C5Database db(__c5config.dbParams());
    C5Permissions::init(db, group());
    db[":f_group"] = group();
    db.exec("select f_key, f_value from s_user_access where f_group=:f_group and f_value=1");
    while (db.nextRow()) {
        fPermissions.append(db.getInt("f_key"));
    }
}

bool C5User::authorize(const QString &altPassword)
{
    C5Database db(__c5config.dbParams());
    db[":f_altpassword"] = altPassword;
    db.exec("select * from s_user where f_altpassword=md5(:f_altpassword) and f_state=1 ");
    if (!db.nextRow()) {
        fError = tr("Access denied");
        return false;
    }
    db.rowToMap(fUserData);
    getPermissions();
    getState();
    return true;
}
