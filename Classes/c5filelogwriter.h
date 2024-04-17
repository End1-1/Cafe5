#ifndef C5FILELOGWRITER_H
#define C5FILELOGWRITER_H

#include "c5threadobject.h"

#define c5log(x) C5FileLogWriter::write(x, "log")
#define c5loge(x) C5FileLogWriter::write(x, "error")
#define c5logf(x, y) C5FileLogWriter::write(x, y)

#ifdef QT_DEBUG
#define c5logd(x, y) C5FileLogWriter::write(x, y)
#else
#define c5logd(x, y) C5FileLogWriter::write(x, y)
#endif

class C5FileLogWriter : public C5ThreadObject
{
    Q_OBJECT
public:
    C5FileLogWriter(const QString &message, QObject *parent = nullptr);
    static void write(const QString &message, const QString &file);

protected slots:
    virtual void run() override;

private:
    QString fFileName;
    const QString fMessage;
    QString createFile();
};

#endif // C5FILELOGWRITER_H
