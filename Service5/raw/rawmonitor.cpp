#include "rawmonitor.h"
#include <QMutex>

RawMonitor::RawMonitor(SslSocket *s) :
    Raw(s)
{
}

RawMonitor::~RawMonitor()
{
    qDebug() << "~RawMonitor";
}

int RawMonitor::run(const QByteArray &d)
{
    QMutexLocker ml(fMutexInformMonitors);
    fMonitors.append(fSocket);
    return 0;
}
