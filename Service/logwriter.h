#ifndef LOGWRITER_H
#define LOGWRITER_H

#include <QMap>

class LogWriter
{
private:
    static QMap<int, QString> fFilesMap;

public:
    LogWriter();
    static int fCurrentLevel;
    static void setFile(int num, const QString &fileName);
    static void write(int level, int file, const QString &session, const QString &message);

private:
    static void writeToFile(const QString &fileName, const QString &session, const QString &message);

};

#endif // LOGWRITER_H
