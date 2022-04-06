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
    QMutexLocker ml(&fMutex);
#ifdef QT_DEBUG
    qDebug() << QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss ") << session << message;
#endif;
    writeToFile(file, session, message);
    if (file != LogWriterLevel::verbose) {
        writeToFile(LogWriterLevel::verbose, session, message);
    }
}

void LogWriter::writeToFile(const QString &fileName, const QString &session, const QString &message)
{
    QFile file(QString("%1/%2/%3/Logs/%4_%5.log").arg(QDir::tempPath(), _APPLICATION_,_MODULE_, QDate::currentDate().toString("dd_MM_yyyy"), fileName));
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
