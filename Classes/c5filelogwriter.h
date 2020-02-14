#ifndef C5FILELOGWRITER_H
#define C5FILELOGWRITER_H

#include "c5threadobject.h"

#define c5log(x) C5FileLogWriter::write(x, "")
#define c5logf(x, y) C5FileLogWriter::write(x, y)

#ifdef QT_DEBUG
#define c5logd(x, y) C5FileLogWriter::write(x, y)
#else
//#define c5logd(x, y) ;
#define c5logd(x, y) C5FileLogWriter::write(x, y)
#endif

class C5FileLogWriter : public C5ThreadObject
{
    Q_OBJECT
public:
    C5FileLogWriter(const QString &message, QObject *parent = nullptr);
    static void write(const QString &message, const QString &fileName = "");
    static void setFileName(const QString &fileName);

protected slots:
    virtual void run() override;

private:
    static QString fFileName;
    QString fAltFileName;
    const QString fMessage;
    static QString createFile(const QString &fileName);
};

#endif // C5FILELOGWRITER_H
