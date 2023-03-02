#include "sqdriver.h"
#include <QFile>
#include <QDebug>

SQDriver *SQDriver::fInstance = nullptr;
QHash<QString, QString> SQDriver::fListOfQueries;

SQDriver::SQDriver()
{
    if (fListOfQueries.isEmpty()) {
        Q_INIT_RESOURCE(shopclientp);
        QFile f(":/sq.txt");
        if (f.open(QIODevice::ReadOnly)){
            QString src(f.readAll());
            qDebug() << src;
            QStringList srclist = src.split("==========");
            for(const QString &s: qAsConst(srclist)) {
                QStringList bl = s.split(">>");
                if (bl.count() >= 2) {
                    fListOfQueries[bl[0].replace("\n", "").replace("\r", "")] = bl.at(1);
                }
            }
        }
    }
}

QString SQDriver::get(const QString &query_name)
{
    if (!fInstance) {
        fInstance = new SQDriver();
    }
    qDebug() << fInstance->fListOfQueries.keys();
    Q_ASSERT(fInstance->fListOfQueries.contains(query_name));
    return fInstance->fListOfQueries[query_name];
}
