#include "c5logsystem.h"

#include <QDir>
#include <QDate>

static QString mInfoLogFileName;

C5LogSystem::C5LogSystem()
{

}

QString C5LogSystem::fileName(const QDate &date)
{
    QDir d;
    if (!d.exists(d.homePath() + "/" + _APPLICATION_ + "/logs/")) {
        d.mkpath(d.homePath() + "/" + _APPLICATION_ + "/logs/");
    }
    return d.homePath() + "/" + _APPLICATION_ + "/logs/info." + date.toString("dd.MM.yyyy.log");
}

void C5LogSystem::writeEvent(const QString message)
{
    if (mInfoLogFileName.isEmpty()) {
        QDir d;
        mInfoLogFileName = fileName(QDate::currentDate());
    }
    QFile f(mInfoLogFileName);
    f.open(QIODevice::Append);
    f.write(QDateTime::currentDateTime().toString("[dd.MM.yyyy HH:mm:ss]").toUtf8());
    f.write(message.toUtf8());
    f.write("\r\n");
    f.close();
}
