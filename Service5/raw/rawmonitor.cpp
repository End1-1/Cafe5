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

void RawMonitor::run(const QByteArray &d)
{
    QMutexLocker ml(fMutexInformMonitors);
    fMonitors.append(fSocket);
}
