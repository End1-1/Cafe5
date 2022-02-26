#include "authentication.h"
#include "database.h"
#include "jsonhandler.h"
#include "databaseconnectionmanager.h"
#include "dduser.h"

Authentication::Authentication() :
    RequestHandler()
{

}

Authentication::~Authentication()
{
}

bool Authentication::handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    JsonHandler jh;
    Database db;
    if (!DatabaseConnectionManager::openDatabase(db, jh)) {
        return setInternalServerError(jh.toString());
    }
    jh["db"] = db.fDatabaseNumber;
    DDUser u;

    if (u.authUserPass(db, getData(data, dataMap["email"]), getData(data, dataMap["phone"]), getData(data, dataMap["password"]))) {
        jh["uuid"] = db("uuid");
        fHttpHeader.setContentLength(jh.length());
        fResponse.append(fHttpHeader.toString());
        fResponse.append(jh.toString());
    } else {
        return setForbiddenError("Authentication failed.");
    }
    db.close();
    return true;
}

bool Authentication::validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    Q_UNUSED(data);
    QString message;
    if ((!dataMap.contains("phone") && !dataMap.contains("email"))) {
        message.append("Phone or email required.");
    }
    if (!dataMap.contains("password")) {
        message.append("Password required.");
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
