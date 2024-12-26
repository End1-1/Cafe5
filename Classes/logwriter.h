#ifndef LOGWRITER_H
#define LOGWRITER_H

#include <QMutex>
#include <QMap>

#define LogWriterVerbose(y) LogWriter::write(LogWriterLevel::verbose, y)
#define LogWriterError(y) LogWriter::write(LogWriterLevel::errors, "", y)

namespace LogWriterLevel
{
const QString verbose = "verbose";
const QString warning = "warning";
const QString errors = "errors";
const QString special = "special";
}

class LogWriter
{

public:
    LogWriter();
    static int fCurrentLevel;
    static void write(const QString &file, const QString &session, const QString &message);

private:
    static void writeToFile(const QString &fileName, const QString &session, const QString &message);
    static QMutex fMutex;

};

#endif // LOGWRITER_H
