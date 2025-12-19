#include "c5user.h"
#include "ninterface.h"
#include "ndataprovider.h"
#include <QJsonArray>

C5User::C5User(const QMap<QString, QVariant>& m) :
    C5User()
{
    fUserData = m;
}

C5User::C5User(C5User *other)
{
    fUserData = other->fUserData;
    fPermissions = other->fPermissions;
    fConfig = other->fConfig;
    fSettings = other->fSettings;
}

void C5User::copySettings(C5User *other)
{
    fConfig = other->fConfig;
    fSettings = other->fSettings;
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
    return QString("%1 %2").arg(data("f_last").toString(), data("f_first").toString());
}

QVariant C5User::value(const QString & key)
{
    return fUserData[key];
}

int C5User::group()
{
    return data("f_group").toInt();
}

void C5User::authByUsernamePass(const QString & username, const QString & pass, NInterface * n,  std::function<void(const QJsonObject&)> callback)
{
    n->createHttpQueryLambda("/engine/login.php",
    QJsonObject{{"method", 1},
        {"username", username},
        {"password", pass}}, [this, callback](const QJsonObject & jo) {
        const QJsonObject &jdata = jo["data"].toObject();
        NDataProvider::sessionKey = jdata["sessionkey"].toString();
        QMap<int, QString> settings;
        QJsonArray jsettings = jdata["settings"].toArray();

        for(int i = 0; i < jsettings.count(); i++) {
            const QJsonObject &js = jsettings.at(i).toObject();
            settings[js["f_key"].toInt()] = js["f_value"].toString();
        }

        fPermissions.clear();
        QJsonArray jpermissions = jdata["permissions"].toArray();

        for(int i = 0; i < jpermissions.count(); i++) {
            fPermissions.append(jpermissions.at(i).toInt());
        }

        fSettings = settings;
        fConfig = jdata["config"].toObject();
        fUserData = jdata["user"].toObject().toVariantMap();
        callback(jo);
    }, [ = ](const QJsonObject & jerr) {
    });
}

void C5User::authByPinPass(const QString & pin, const QString & pass, NInterface * n, std::function<void(const QJsonObject&)> callback)
{
    n->createHttpQueryLambda("/engine/login.php",
    QJsonObject{{"method", 6},
        {"pin", pin},
        {"pass", pass}}, [this, callback](const QJsonObject & jo) {
        const QJsonObject &jdata = jo["data"].toObject();
        NDataProvider::sessionKey = jdata["sessionkey"].toString();
        QMap<int, QString> settings;
        QJsonArray jsettings = jdata["settings"].toArray();

        for(int i = 0; i < jsettings.count(); i++) {
            const QJsonObject &js = jsettings.at(i).toObject();
            settings[js["f_key"].toInt()] = js["f_value"].toString();
        }

        fPermissions.clear();
        QJsonArray jpermissions = jdata["permissions"].toArray();

        for(int i = 0; i < jpermissions.count(); i++) {
            fPermissions.append(jpermissions.at(i).toInt());
        }

        fSettings = settings;
        fConfig = jdata["config"].toObject();
        fUserData = jdata["user"].toObject().toVariantMap();
        callback(jo);
    }, [ = ](const QJsonObject & jerr) {
    });
}

bool C5User::check(int permission)
{
    if(fUserData["f_group"].toInt() == 1) {
        return true;
    }

    if(fPermissions.contains(permission)) {
        return true;
    }

    fError = tr("You have not permission");
    return false;
}
bool C5User::enterWork()
{
    if(fState == usAtWork) {
        fError = tr("Cannot input without output");
        return false;
    } else {
        //TODO
        // C5Database db;
        // db[":f_id"] = db.uuid();
        // db[":f_user"] = id();
        // db[":f_datein"] = QDate::currentDate();
        // db[":f_timein"] = QTime::currentTime();
        // if(!db.insert("s_salary_inout", false)) {
        //     fError = db.fLastError;
        //     return false;
        // }
    }

    fState = usAtWork;
    return true;
}
bool C5User::leaveWork()
{
    //TODO
    // C5Database db;
    // if(fState == usAtWork) {
    //     db[":f_dateout"] = QDate::currentDate();
    //     db[":f_timeout"] = QTime::currentTime();
    //     if(!db.update("s_salary_inout", "f_user", id())) {
    //         fError = db.fLastError;
    //         return false;
    //     }
    // } else {
    //     fError = tr("Cannot output without input");
    //     return false;
    // }
    fState = usNotAtWork;
    return true;
}
C5User::UserState C5User::state()
{
    return fState;
}

void C5User::addPermission(int permission)
{
    fPermissions.append(permission);
}

void C5User::removePermission(int permission)
{
    if(fPermissions.contains(permission)) {
        fPermissions.removeAll(permission);
    }
}

C5User::C5User() :
    QObject()
{
    fState = usNotAtWork;
}

QVariant C5User::data(const QString & name)
{
    Q_ASSERT(fUserData.contains(name));
    return fUserData[name];
}

void C5User::getState(NInterface * n)
{
    //TODO
    // C5Database db;
    // db[":f_user"] = id();
    // db.exec("select * from s_salary_inout where f_user=:f_user and f_dateout is null");
    // fState = db.nextRow() ? usAtWork : usNotAtWork;
}

void C5User::authorize(const QString & pin, NInterface * n, std::function<void (const QJsonObject&)> callback)
{
    n->createHttpQueryLambda("/engine/login.php",
    QJsonObject{{"method", 2},
        {"pin", pin}}, [this, callback](const QJsonObject & jo) {
        const QJsonObject &jdata = jo["data"].toObject();
        NDataProvider::sessionKey = jdata["sessionkey"].toString();
        QMap<int, QString> settings;
        QJsonArray jsettings = jdata["settings"].toArray();

        for(int i = 0; i < jsettings.count(); i++) {
            const QJsonObject &js = jsettings.at(i).toObject();
            settings[js["f_key"].toInt()] = js["f_value"].toString();
        }

        fPermissions.clear();
        QJsonArray jpermissions = jdata["permissions"].toArray();

        for(int i = 0; i < jpermissions.count(); i++) {
            fPermissions.append(jpermissions.at(i).toInt());
        }

        fSettings = settings;
        fConfig = jdata["config"].toObject()["f_config"].toObject();
        fUserData = jdata["user"].toObject().toVariantMap();
        callback(jo);
    }, [ = ](const QJsonObject & jerr) {
    });
}
