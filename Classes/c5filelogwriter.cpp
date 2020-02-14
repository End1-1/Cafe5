#include "c5filelogwriter.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>

QString C5FileLogWriter::fFileName;

C5FileLogWriter::C5FileLogWriter(const QString &message, QObject *parent) :
    C5ThreadObject(parent),
    fMessage(message)
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void C5FileLogWriter::write(const QString &message, const QString &fileName)
{
    C5FileLogWriter *l = new C5FileLogWriter(message);
    if (!fileName.isEmpty()) {
        l->fAltFileName = fileName;
    }
    l->start();
}

void C5FileLogWriter::setFileName(const QString &fileName)
{
    fFileName = createFile(fileName);
}

void C5FileLogWriter::run()
{
    QString filename = fAltFileName.isEmpty() ? fFileName : createFile(fAltFileName);
    QFile file(filename);
    if (file.open(QIODevice::Append)) {
        file.write(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss ").toUtf8());
        file.write(fMessage.toUtf8());
        file.write("\r\n");
        file.close();
    }
#ifdef QT_DEBUG
    qDebug() << fMessage;
#endif
    emit finished();
}

QString C5FileLogWriter::createFile(const QString &fileName)
{
    QDir d;
    QString appHomePath = d.homePath() + "\\" + _APPLICATION_ + "\\" + _MODULE_;
    if (!d.exists(appHomePath)) {
        d.mkpath(appHomePath);
    }
    QString appLogFile = appHomePath + "\\" + fileName;
    return appLogFile;
}
