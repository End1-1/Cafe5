#include "c5filelogwriter.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>

C5FileLogWriter::C5FileLogWriter(const QString &message, QObject *parent) :
    C5ThreadObject(parent),
    fMessage(message)
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void C5FileLogWriter::write(const QString &message, const QString &file)
{
    C5FileLogWriter *l = new C5FileLogWriter(message);
    l->fFileName = file;
    l->start();
}

void C5FileLogWriter::run()
{
#ifdef QT_DEBUG
    qDebug() << fMessage.left(5000);
    QFile file(createFile());
    if (file.open(QIODevice::Append)) {
        file.write(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss ").toUtf8());
        file.write(fMessage.toUtf8());
        file.write("\r\n");
        file.close();
    }
#else
    QFile file(createFile());
    if (file.open(QIODevice::Append)) {
        file.write(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss ").toUtf8());
        file.write(fMessage.toUtf8());
        file.write("\r\n");
        file.close();
    }
#endif
    emit finished();
}

QString C5FileLogWriter::createFile()
{
    QDir d;
    QString appHomePath = d.homePath() + "\\" + _APPLICATION_ + "\\" + _MODULE_;
    if (!d.exists(appHomePath)) {
        d.mkpath(appHomePath);
    }
    return appHomePath + "\\" + fFileName + "_" + QDate::currentDate().toString("ddMMyyyy") + ".log";
}
