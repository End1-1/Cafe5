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

bool C5License::isOK(QString &err)
{
    return true;
//    if (QDate::currentDate() > QDate::fromString("08/06/2021", "dd/MM/yyyy")) {
//        return false;
//    }
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(_popen("wmic path win32_physicalmedia get SerialNumber", "r"), _pclose);
    if (!pipe)  {
        err = "Cannot run OS command";
        return false;
    }
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL) {
            result += buffer.data();
        }
    }
    QString key = QString::fromStdString(result).replace("\r\n", "").replace(" ", "");
    if (key.isEmpty()) {
        err = "Cannot determine device";
        return false;
    }

    QDir dir;
    QString fileName = dir.homePath() + "\\" + _APPLICATION_ + "\\" + _MODULE_ + "/lic.dat";
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly)) {
        if (!f.open(QIODevice::WriteOnly)) {
            err = f.errorString();
            return false;
        }
        f.write(key.toUtf8());
        f.close();
        err = "Please, register application.";
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
        err = "Invalid date. Please, register application.";
        return false;
    }
    qDebug() << key << in << out;
    if (QDate::currentDate() > validTo) {
        err = "License expired";
        return false;
    }
    return true;
}
