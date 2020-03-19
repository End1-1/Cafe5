#include "socketrw.h"
#include "logwriter.h"
#include "sslsocket.h"
#include "sockethandlerlogin.h"
#include "sockethandlerunknown.h"
#include "sockethandlerservicelogin.h"
#include "sockethandlerserviceconfig.h"
#include "sockethandlerloginwithsession.h"
#include "servicecommands.h"

SocketRW::SocketRW(SslSocket *socket)
{
    fSocket = socket;
}

SocketRW::~SocketRW()
{
    delete fSocket;
}

void SocketRW::go()
{
    qint32 datasize = 0;
    qint32 dataread = 0;
    qint32 datatype = 0;
    QByteArray data;
    int buffersize = 8192;
    char *buffer = new char[buffersize];
    while (fSocket->waitForReadyRead(-1)) {
        if (fSocket->bytesAvailable()) {
            if (datasize == 0) {
                fSocket->read(reinterpret_cast<char *>(&datasize), sizeof(datasize));
                fSocket->read(reinterpret_cast<char *>(&datatype), sizeof(datatype));
                LogWriter::write(10, 1, fSocket->fUuid, QString("Ready data, type: %2, size: %3 bytes").arg(datatype).arg(datasize));
            }
            int r = fSocket->read(buffer, datasize - dataread > buffersize ? buffersize : datasize - dataread);
            dataread += r;
            data.append(buffer, r);
            if (dataread == datasize) {
                LogWriter::write(10, 1, fSocket->fUuid, data);
                SocketHandler *sh;
                switch (datatype) {
                case dt_login:
                    sh = SocketHandler::create<SocketHandlerLogin>(data);
                    break;
                case dt_service_login:
                    sh = SocketHandler::create<SocketHandlerServiceLogin>(data);
                    break;
                case dt_service_config:
                    sh = SocketHandler::create<SocketHandlerServiceConfig>(data);
                    break;
                case dt_login_with_session:
                    sh = SocketHandler::create<SocketHandlerLoginWithSession>(data);
                    break;
                default:
                    sh = SocketHandler::create<SocketHandlerUnknown>(data);
                    break;
                }
                sh->setSocket(fSocket);
                sh->processData();
                LogWriter::write(10, 1, fSocket->fUuid, QObject::tr("Response: ") + data);
                datasize = data.size();
                fSocket->write(reinterpret_cast<const char*>(&datasize), sizeof(datasize));
                fSocket->write(reinterpret_cast<const char*>(&sh->fResponseCode), sizeof(sh->fResponseCode));
                fSocket->write(data);
                fSocket->flush();
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
    LogWriter::write(10, 1, fSocket->fUuid, "Close socket");
    fSocket->close();
}
