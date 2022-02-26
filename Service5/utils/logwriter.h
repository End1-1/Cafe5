#ifndef LOGWRITER_H
#define LOGWRITER_H

#include <QMutex>
#include <QMap>


namespace LogWriterLevel {
    const int verbose = 1;
    const int warning = 2;
    const int errors = 3;
}

class LogWriter
{
private:
    static QMap<int, QString> fFilesMap;

public:
    LogWriter();
    static int fCurrentLevel;
    static void setFile(int num, const QString &fileName);
    static void write(int file, const QString &session, const QString &message);

private:
    static void writeToFile(const QString &fileName, const QString &session, const QString &message);
    static QMutex fMutex;

};

#endif // LOGWRITER_H
