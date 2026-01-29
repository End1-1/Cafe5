#ifndef LOGWRITER_H
#define LOGWRITER_H

#include <QMap>

#define LogWriterVerbose(title, message) LogWriter::write(LogWriterLevel::verbose, title, message)
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
    static void write(const QString &file, const QString &title, const QString &message);

private:
    static void writeToFile(const QString &fileName, const QString &session, const QString &message);

};

#endif // LOGWRITER_H
