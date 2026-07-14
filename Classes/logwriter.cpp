#include "logwriter.h"
#include <QCoreApplication>
#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>
#include <QStandardPaths>

int LogWriter::fCurrentLevel = 0;
QString LogWriter::sLastLogDir;

namespace {

QMutex &logMutex()
{
    static QMutex m;
    return m;
}

bool &pointerWritten()
{
    static bool done = false;
    return done;
}

}

LogWriter::LogWriter()
{
}

QString LogWriter::lastLogDirectory()
{
    QMutexLocker lock(&logMutex());
    return sLastLogDir;
}

QStringList LogWriter::candidateLogDirectories()
{
    QStringList dirs;
    const QString module = QString::fromUtf8(_MODULE_);

#ifdef Q_OS_WIN
    /* Windows service: never write next to install dir (often not writable / hard to find). */
    if(module == QLatin1String("Service5") || module.startsWith(QLatin1String("Service5_"))) {
        dirs << QString("C:/Windows/Temp/%1/%2/Logs").arg(_APPLICATION_, _MODULE_);
        dirs.removeDuplicates();
        return dirs;
    }
#endif

    /* 1) Fixed ProgramData — Local System service + easy to find */
    dirs << QString("C:/ProgramData/%1/%2/Logs").arg(_APPLICATION_, _MODULE_);

    /* 2) Next to the executable */
    if(qApp) {
        const QString appDir = QCoreApplication::applicationDirPath();
        if(!appDir.isEmpty()) {
            dirs << (appDir + QString("/%1_logs").arg(_MODULE_));
        }
    }

    /* 3) Process TEMP (user Temp OR C:/Windows/Temp for SYSTEM) */
#ifdef Q_OS_WIN
    dirs << QString("%1/%2/%3/Logs").arg(QDir::tempPath(), _APPLICATION_, _MODULE_);
#else
    dirs << QString("%1/%2/%3/Logs")
                .arg(QStandardPaths::writableLocation(QStandardPaths::TempLocation),
                     _APPLICATION_,
                     _MODULE_);
#endif

    /* 4) Ultra-obvious root folder */
    dirs << QString("C:/Cafe5Logs/%1").arg(_MODULE_);

    dirs.removeDuplicates();
    return dirs;
}

void LogWriter::rememberLogDir(const QString &dir)
{
    sLastLogDir = dir;
}

void LogWriter::writePointerFile(const QString &dir)
{
    if(pointerWritten()) {
        return;
    }
    pointerWritten() = true;

    const QByteArray text = (dir + "\r\n").toUtf8();
    const QStringList pointers = {
        QString("C:/ProgramData/%1/LOG_PATH.txt").arg(_APPLICATION_),
        QString("C:/Cafe5Logs/LOG_PATH.txt"),
        QDir::tempPath() + QString("/%1_LOG_PATH.txt").arg(_MODULE_),
    };

    for(const QString &pointer : pointers) {
        QDir().mkpath(QFileInfo(pointer).absolutePath());
        QFile f(pointer);
        if(f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            f.write("Cafe5 log directory:\r\n");
            f.write(text);
            f.write("Candidates:\r\n");
            for(const QString &c : candidateLogDirectories()) {
                f.write(c.toUtf8());
                f.write("\r\n");
            }
            f.close();
        }
    }
}

void LogWriter::write(const QString &file, const QString &title, const QString &message)
{
#ifdef QT_DEBUG
    qDebug() << QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss ") << title << message.left(1000);
#else

    if(file == LogWriterLevel::special) {
        return;
    }

#endif
    writeToFile(file, title, message);

    if(file != LogWriterLevel::verbose) {
        writeToFile(LogWriterLevel::verbose, title, message);
    }
}

void LogWriter::writeToFile(const QString &fileName, const QString &title, const QString &message)
{
    QMutexLocker lock(&logMutex());

    const QByteArray stamp = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss ").toUtf8();
    QByteArray line = stamp;
    if(!title.isEmpty()) {
        line += title.toUtf8();
        line += " ";
    }
    line += message.toUtf8();
    line += "\r\n";

    QString lastAttempt;
    bool written = false;
    QString firstDir;

    /* Mirror into every writable location so console / service / SYSTEM are all covered */
    for(const QString &fnpath : candidateLogDirectories()) {
        lastAttempt = fnpath + QString("/%1_%2.log").arg(QDate::currentDate().toString("dd_MM_yyyy"), fileName);
        if(!QDir().mkpath(fnpath)) {
            continue;
        }
        QFile file(lastAttempt);
        if(!file.open(QIODevice::Append)) {
            continue;
        }
        file.write(line);
        file.close();
        if(!written) {
            firstDir = fnpath;
        }
        written = true;
    }

    if(!written) {
        QFile fallback(QStringLiteral("C:/Windows/Temp/cafe5_log_error.txt"));
        if(fallback.open(QIODevice::Append)) {
            fallback.write("LOG OPEN FAILED lastAttempt=");
            fallback.write(lastAttempt.toUtf8());
            fallback.write("\r\n");
            fallback.close();
        }
        return;
    }

    rememberLogDir(firstDir);
    writePointerFile(firstDir);
}
