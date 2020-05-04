#include "socketrw.h"
#include "logwriter.h"
#include "sslsocket.h"
#include "sockethandlerlogin.h"
#include "sockethandlerunknown.h"
#include "sockethandlerservicelogin.h"
#include "sockethandlerserviceconfig.h"
#include "sockethandlerlocationchanged.h"
#include "sockethandleruploadsale.h"
#include "sockethandleruuid.h"
#include "servicecommands.h"

SocketRW::SocketRW(SslSocket *socket)
{
    fSocketData.fSocket = socket;
}

SocketRW::~SocketRW()
{
    delete fSocketData.fSocket;
}

void SocketRW::go()
{
    qint32 datasize = 0;
    qint32 dataread = 0;
    qint32 datatype = 0;
    QByteArray data;
    int buffersize = 8192;
    char *buffer = new char[buffersize];
    int waitForReadyReadTimeout = 30000;
    while (fSocketData.fSocket->waitForReadyRead(waitForReadyReadTimeout)) {
        if (fSocketData.fSocket->bytesAvailable()) {
            if (datasize == 0) {
                fSocketData.fSocket->read(reinterpret_cast<char *>(&datasize), sizeof(datasize));
                fSocketData.fSocket->read(reinterpret_cast<char *>(&datatype), sizeof(datatype));
                LogWriter::write(10, 1, fSocketData.fSocket->fUuid, QString("Ready data, type: %2, size: %3 bytes").arg(datatype).arg(datasize));
            }
            int r = fSocketData.fSocket->read(buffer, datasize - dataread > buffersize ? buffersize : datasize - dataread);
            dataread += r;
            data.append(buffer, r);
            if (dataread == datasize) {
                LogWriter::write(10, 1, fSocketData.fSocket->fUuid, data);
                SocketHandler *sh = nullptr;
                SocketHandlerLogin *hl = nullptr;
                switch (datatype) {
                case dt_login:
                    hl = SocketHandler::create<SocketHandlerLogin>(&fSocketData, data);
                    sh = hl;
                    if (hl->login()) {
                        waitForReadyReadTimeout = -1;
                    }
                    break;
                case dt_service_login:
                    sh = SocketHandler::create<SocketHandlerServiceLogin>(&fSocketData, data);
                    break;
                case dt_service_config:
                    sh = SocketHandler::create<SocketHandlerServiceConfig>(&fSocketData, data);
                    break;
                case dt_location_changed:
                    sh = SocketHandler::create<SocketHandlerLocationChanged>(&fSocketData, data);
                    break;
                case dt_upload_sale:
                    sh = SocketHandler::create<SocketHandlerUploadSale>(&fSocketData, data);
                    break;
                case dt_wonna_uuid:
                    sh = SocketHandler::create<SocketHandlerUUID>(&fSocketData, data);
                    break;
                default:
                    sh = SocketHandler::create<SocketHandlerUnknown>(&fSocketData, data);
                    break;
                }
                if (fSocketData.fSessionId.length() > 0) {
                    sh->processData();
                } else {
                    sh->fResponseCode = dr_login_required;
                    data.clear();
                }

                LogWriter::write(10, 1, fSocketData.fSocket->fUuid, QObject::tr("Response: ") + data);
                datasize = data.size();
                fSocketData.fSocket->write(reinterpret_cast<const char*>(&datasize), sizeof(datasize));
                fSocketData.fSocket->write(reinterpret_cast<const char*>(&sh->fResponseCode), sizeof(sh->fResponseCode));
                fSocketData.fSocket->write(data);
                fSocketData.fSocket->flush();
                bool closeConnection = sh->closeConnection();
                delete sh;
                if (closeConnection) {
                    break;
                }
                data.clear();
                datasize = 0;
                dataread = 0;
                datatype = 0;
            }
        }
    }
    delete [] buffer;
    LogWriter::write(10, 1, fSocketData.fSocket->fUuid, "Close socket");
    fSocketData.fSocket->close();
}
