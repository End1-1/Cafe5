#include "authentication.h"
#include "database.h"
#include "jsonhandler.h"

Authentication::Authentication() :
    RequestHandler()
{

}

Authentication::~Authentication()
{
    __debug_log("~Authentication");
}

bool Authentication::handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    JsonHandler jh;
    Database db;
    if (!db.open("127.0.0.1", "server5", "root", "osyolia")) {
        return setInternalServerError("Database error: " + db.lastDbError() + " #" + db.fDatabaseNumber);
    }
    jh["db"] = db.fDatabaseNumber;
    db[":fphone"] = getData(data, dataMap["phone"]);
    db[":femail"] = getData(data, dataMap["email"]);
    db[":fpassword"] = getData(data, dataMap["password"]);
    db.exec("select uuid(), u.* from users_list u where (u.femail=:femail or u.fphone=:fphone) and u.fpassword=:fpassword");
    if (db.next()) {
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
