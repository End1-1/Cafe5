#include "rawmonitor.h"
#include "rawdataexchange.h"

RawMonitor::RawMonitor(SslSocket *s, const QByteArray &d) :
    Raw(s, d)
{
    connect(this, &RawMonitor::registerMonitor, RawDataExchange::instance(), &RawDataExchange::registerMonitor);
}

RawMonitor::~RawMonitor()
{
    disconnect(this, &RawMonitor::registerMonitor, RawDataExchange::instance(), &RawDataExchange::registerMonitor);
}

void RawMonitor::run()
{
    emit registerMonitor(fSocket);
}
