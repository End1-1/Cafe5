#include "rawdllop.h"
#include "configini.h"
#include <QLibrary>

typedef void (*dllfunc)(const QByteArray &, RawMessage &);

RawDllOp::RawDllOp(SslSocket *s) :
    Raw(s)
{

}

void RawDllOp::run(const QByteArray &d)
{
    QString dll;
    readString(dll, d);
    QLibrary l(ConfigIni::fAppPath + "/rawhandlers/" + dll + ".dll");
    if (l.load()) {
        dllfunc f = (dllfunc) l.resolve("dllfunc");
        if (f) {
            f(d, *this);
        } else {
            putUByte(1);
        }
    } else {
        putUByte(0);
    }
}
