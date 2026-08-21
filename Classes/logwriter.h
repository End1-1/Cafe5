#ifndef LOGWRITER_H
#define LOGWRITER_H

#include <QMap>
#include <QString>

#define LogWriterVerbose(title, message) LogWriter::write(LogWriterLevel::verbose, title, message)
#define LogWriterError(y) LogWriter::write(LogWriterLevel::errors, "", y)

namespace LogWriterLevel
{
const QString verbose = "verbose";
const QString warning = "warning";
const QString errors = "errors";
const QString special = "special";
const QString websocket = "websocket";
}

class LogWriter
{

public:
    LogWriter();
    static int fCurrentLevel;
    static void write(const QString &file, const QString &title, const QString &message);
    /** Directory that was successfully used for the last write (empty if never written). */
    static QString lastLogDirectory();
    /** Candidate directories in priority order (for diagnostics). */
    static QStringList candidateLogDirectories();

private:
    static void writeToFile(const QString &fileName, const QString &session, const QString &message);
    static void rememberLogDir(const QString &dir);
    static void writePointerFile(const QString &dir);

    static QString sLastLogDir;
};

#endif // LOGWRITER_H
