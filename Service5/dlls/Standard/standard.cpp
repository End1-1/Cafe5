#include "standard.h"
#include "requesthandler.h"
#include <QJsonObject>
#include <QSettings>
#include <QJsonArray>
#include <QRandomGenerator>
#include <QUuid>
#include <QDir>

enum RegistrationState {rsNew = 1, rsConfirmed};

void routes(QStringList &r)
{
    r.append("notfound");
}

bool notfound(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap,
              const ContentType &contentType)
{
    Q_UNUSED(dataMap);
    RequestHandler rh(outdata);
    rh.fContentType = contentType;
    rh.setResponse(HTTP_NOT_FOUND, QString("%1 not found on this server").arg(indata.data()));
    return true;
}

