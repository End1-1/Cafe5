#include "rawdllop.h"
#include "configini.h"
#include "logwriter.h"
#include "sqlqueries.h"
#include <QLibrary>

typedef bool (*dllfunc)(const QByteArray &, RawMessage &);

RawDllOp::RawDllOp(SslSocket *s) :
    Raw(s)
{

}

RawDllOp::~RawDllOp()
{
    qDebug() << "~RawDllOp";
}

int RawDllOp::run(const QByteArray &d)
{
    QString dll;
    readString(dll, d);
    QLibrary l(ConfigIni::fAppPath + "/rawhandlers/" + dll + ".dll");
    if (l.load()) {
        dllfunc f = reinterpret_cast<dllfunc>(l.resolve("dllfunc"));
        if (f) {
            f(d, *this);
        } else {
            putUByte(1);
        }
    } else {
        putUByte(0);
    }
    l.unload();
    return 0;
}
