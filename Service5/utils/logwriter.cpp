#include "logwriter.h"
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <QDir>

QMutex LogWriter::fMutex;
QMap<int, QString> LogWriter::fFilesMap;
int LogWriter::fCurrentLevel = 0;

LogWriter::LogWriter()
{

}

void LogWriter::setFile(int num, const QString &fileName)
{
    QMutexLocker ml(&fMutex);
    QDir().mkpath(QFileInfo(fileName).path());
    fFilesMap[num] = fileName;
}

void LogWriter::write(int file, const QString &session, const QString &message)
{
    QMutexLocker ml(&fMutex);
#ifdef QT_DEBUG
    qDebug() << QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss ") << message;
#endif;
    if (fFilesMap.empty()) {
        fFilesMap[file] = QDir::tempPath() + "/" + _APPLICATION_ + "." + _MODULE_ + ".log";
    }

    QString fileName = fFilesMap[file];
    writeToFile(fileName, session, message);
    if (file != LogWriterLevel::verbose) {
        writeToFile(fFilesMap[LogWriterLevel::verbose], session, message);
    }
}

void LogWriter::writeToFile(const QString &fileName, const QString &session, const QString &message)
{
    QFile file(fileName);
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
