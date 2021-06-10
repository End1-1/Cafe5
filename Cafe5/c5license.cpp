#include "c5license.h"
#include "c5crypt.h"
#include <memory>
#include <cstdio>
#include <array>
#include <QDate>
#include <QDebug>
#include <QSysInfo>
#include <QFile>
#include <QDir>

//static const QString settingsName(QString(_ORGANIZATION_), QString("%1\\%2").arg(_APPLICATION_).arg(_MODULE_));

C5License::C5License()
{

}

bool C5License::isOK()
{
//    if (QDate::currentDate() > QDate::fromString("08/06/2021", "dd/MM/yyyy")) {
//        return false;
//    }
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(_popen("wmic path win32_physicalmedia get SerialNumber", "r"), _pclose);
    if (!pipe)  {
        return false;
    }
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL) {
            result += buffer.data();
        }
    }
    QString key = QString::fromStdString(result).replace("\r\n", "").replace(" ", "");

    QDir dir;
    QString fileName = dir.absolutePath() + "/lic.dat";
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly)) {
        if (!f.open(QIODevice::WriteOnly)) {
            return false;
        }
        f.write(key.toUtf8());
        f.close();
        return false;
    }

    C5Crypt c;
    QByteArray in = f.readAll();
    f.close();
    QByteArray out;
    c.decryptData(key.toUtf8(), in, out);
    QString outStr = out;
    outStr = outStr.replace('\006', "");
    QDate validTo = QDate::fromString(outStr, "dd/MM/yyyy");
    if (!validTo.isValid()) {
        f.open(QIODevice::WriteOnly);
        f.write(in);
        f.write("\r\n\r\n\r\n\r\n");
        f.write(outStr.toUtf8());
        f.write("\r\n\r\n\r\n\r\n");
        outStr = "Invalid date";
        f.write(outStr.toUtf8());
        f.close();
        return false;
    }
    qDebug() << key << in << out;
    if (QDate::currentDate() > validTo) {
        f.open(QIODevice::WriteOnly);
        f.write(outStr.toUtf8());
        outStr = "Expired";
        f.write(outStr.toUtf8());
        f.close();
        return false;
    }
    return true;
}
