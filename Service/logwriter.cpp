#include "logwriter.h"
#include <QMutex>
#include <QFile>
#include <QDateTime>
#include <QDebug>

static QMutex fMutex;
QMap<int, QString> LogWriter::fFilesMap;
int LogWriter::fCurrentLevel = 0;

LogWriter::LogWriter()
{

}

void LogWriter::setFile(int num, const QString &fileName)
{
    QMutexLocker ml(&fMutex);
    fFilesMap[num] = fileName;
}

void LogWriter::write(int level, int file, const QString &session, const QString &message)
{
#ifdef QT_DEBUG
    qDebug() << message;
#endif;
    if (fCurrentLevel < level) {
        return;
    }
    QMutexLocker ml(&fMutex);
    QString fileName = fFilesMap[file];
    ml.unlock();
    writeToFile(fileName, session, message);
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
