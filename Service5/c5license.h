#ifndef C5LICENSE_H
#define C5LICENSE_H

#include "c5crypt.h"
#include <QApplication>
#include <QDate>
#include <QFile>

class C5License
{
public:
    explicit C5License();
    bool write(const QByteArray &b);
    __forceinline bool read(QDate &d, QString &ltype) {
        QByteArray src;
        QFile f(qApp->applicationDirPath() + "/lic.dat");
        if (f.open(QIODevice::ReadOnly)) {
            src = f.readAll();
            f.close();
        } else {
            return false;
        }
        QByteArray outData;
        C5Crypt c;
        c.ede3_cbc(src, outData, c.driveKey(), false);
        QStringList licenseData = QString(outData).split(";", QString::SkipEmptyParts);
        if (licenseData.count() != 4) {
            return false;
        }
        d = QDate::fromString(licenseData.at(2), "dd/MM/yyyy");
        if (d.isValid() == false) {
            return false;
        }
        switch (licenseData.at(3).toInt()) {
        case 1:
            ltype = "Demo";
            break;
        case 2:
            ltype = "Server";
            break;
        default:
            return false;
        }
        return true;
    }

};

#endif // C5LICENSE_H
