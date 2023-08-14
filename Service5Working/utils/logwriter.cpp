#include "logwriter.h"
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <QDir>

QMutex LogWriter::fMutex;
int LogWriter::fCurrentLevel = 0;

LogWriter::LogWriter()
{

}

void LogWriter::write(const QString &file, const QString &session, const QString &message)
{
#ifdef QT_DEBUG
    qDebug() << QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss ") << session << message.left(1000);
#else
    if (file == LogWriterLevel::special) {
        return;
    }
#endif;
    QMutexLocker ml(&fMutex);
    writeToFile(file, session, message);
    if (file != LogWriterLevel::verbose) {
        writeToFile(LogWriterLevel::verbose, session, message);
    }
}

void LogWriter::writeToFile(const QString &fileName, const QString &session, const QString &message)
{
    QString fnpath = QString("%1/%2/%3/Logs").arg(QDir::tempPath(), _APPLICATION_,_MODULE_);
    QString fn = fnpath + QString("/%4_%5.log").arg(QDate::currentDate().toString("dd_MM_yyyy"), fileName);
    QDir().mkpath(QDir().absoluteFilePath(fnpath));
    QFile file(fn);
    if (file.open(QIODevice::Append)) {
        file.write(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss ").toUtf8());
        if (!session.isEmpty()) {
            file.write(session.toUtf8());
            file.write(" ");
        }
        file.write(message.toUtf8());
        file.write("\r\n");
        file.close();
    }
}
