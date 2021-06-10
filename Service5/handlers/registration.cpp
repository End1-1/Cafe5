#include "registration.h"
#include "databaseconnectionmanager.h"
#include "registrationstate.h"
#include "jsonhandler.h"
#include <QRandomGenerator>
#include <QUuid>

Registration::Registration() :
    RequestHandler()
{

}

bool Registration::handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    JsonHandler jh;
    Database db;
    if (!DatabaseConnectionManager::openDatabase(db, jh)) {
        return setInternalServerError(jh.toString());
    }
    db[":fphone"] = getData(data, dataMap["phone"]);
    db[":femail"] = getData(data, dataMap["email"]);
    db.exec("select fid from users_list where (length(femail)>0 and femail=:femail) || (length(fphone)>0 and fphone=:fphone)");
    if (db.next()) {
        jh["message"] = "An account exists with given data. You should use authentication or password recover";
        fHttpHeader.setResponseCode(HTTP_SEE_OTHER);
        fHttpHeader.setLocation("/authentication");
        fHttpHeader.setContentLength(jh.length());
        fResponse.append(fHttpHeader.toString());
        fResponse.append(jh.toString());
        return true;
    }
    const quint32 low = 100000;
    const quint32 high = 999999;
    QRandomGenerator rg = QRandomGenerator::securelySeeded();
    QString token = QUuid::createUuid().toString() + "-" + QUuid::createUuid().toString();
    token.replace("{", "").replace("}", "");
    db[":fstate"] = RegistrationState::rsNew;
    db[":fphone"] = getData(data, dataMap["phone"]);
    db[":femail"] = getData(data, dataMap["email"]);
    db[":fpassword"] = getData(data, dataMap["password"]);
    db[":fconfirmation_code"] = rg.bounded(low, high);
    db[":ftoken"] = token;
    db[":fcreatedate"] = QDate::currentDate();
    db[":fcreatetime"] = QTime::currentTime();
    db.insert("users_registration");
    if (!getData(data, dataMap["phone"]).isEmpty()) {
        jh["message"] = "A confirmation code was sent to your phone.";
    }
    if (!getData(data, dataMap["email"]).isEmpty()) {
        jh["message"] = "A confirmation code was sent to your email.";
    }
    jh["token"] = token;
    return setResponse(HTTP_OK, jh.toString());
}

bool Registration::validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    Q_UNUSED(data);
    QString message;
    if ((!dataMap.contains("phone") && !dataMap.contains("email"))) {
        message += "Phone or email required.";
    }
    if (!dataMap.contains("password")) {
        message += "Password required.";
    }
    if (!message.isEmpty()) {
        JsonHandler jh;
        jh["message"] = message;
        fHttpHeader.setResponseCode(HTTP_DATA_VALIDATION_ERROR);
        fHttpHeader.setContentLength(jh.length());
        fResponse.append(fHttpHeader.toString());
        fResponse.append(jh.toString());
    }
    return message.isEmpty();
}
