#include "confirmregistration.h"
#include "jsonhandler.h"
#include "databaseconnectionmanager.h"
#include "registrationstate.h"

ConfirmRegistration::ConfirmRegistration() :
    RequestHandler()
{

}

void ConfirmRegistration::handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    Database db;
    JsonHandler jh;
    if (!DatabaseConnectionManager::openDatabase(db, jh)) {
        setResponse(HTTP_INTERNAL_SERVER_ERROR, jh.toString());
        return;
    }
    db[":ftoken"] = getData(data, dataMap["token"]);
    db[":fconfirmation_code"] = getData(data, dataMap["code"]);
    db[":fstate"] = RegistrationState::rsNew;
    db.exec("select * from users_registration where fconfirmation_code=:fconfirmation_code and ftoken=:ftoken and fstate=:fstate");
    if (!db.next()) {
        jh["message"] = "Invalid code";
        setResponse(HTTP_DATA_VALIDATION_ERROR, jh.toString());
        return;
    }
    int id = db("fid").toInt();
    int confirmid = 0;
    db[":fgroup"] = 2;
    db[":fphone"] = db("fphone");
    db[":femail"] = db("femail");
    db[":fpassword"] = db("fpassword");
    db[":ffirstname"] = "";
    db[":flastname"] = "";
    if (!db.insert("users_list", confirmid)) {
        jh["message"] = db.lastDbError();
        setResponse(HTTP_INTERNAL_SERVER_ERROR, jh.toString());
        return;
    }
    db[":fid"] = id;
    db[":fstate"] = RegistrationState::rsConfirmed;
    db[":fconfirmdate"] = QDate::currentDate();
    db[":fconfirmtime"] = QTime::currentTime();
    db[":fconfirmid"] = confirmid;
    db.update("users_registration");
    jh["message"] = "ok";
    setResponse(HTTP_OK, jh.toString());
}

bool ConfirmRegistration::validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    Q_UNUSED(data);
    QString message;
    JsonHandler jh;
    if (!dataMap.contains("token")) {
        message += "Missing token.";
    }
    if (!dataMap.contains("code")) {
        message += "Missing code.";
    }
    if (!message.isEmpty()) {
        jh["message"] = message;
        setResponse(HTTP_DATA_VALIDATION_ERROR, jh.toString());
        return false;
    }
    return true;
}
