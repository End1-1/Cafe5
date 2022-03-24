#include "c5license.h"

C5License::C5License()
{

}

bool C5License::write(const QByteArray &b)
{
    QFile f(qApp->applicationDirPath() + "/lic.dat");
    if (f.open(QIODevice::WriteOnly)) {
        f.write(b);
        f.close();
    } else {
        return false;
    }
    return true;
}
