#include "commandline.h"
#include <QFileInfo>
#include <Windows.h>

CommandLine::CommandLine()
{
    QString cl = QString::fromWCharArray(GetCommandLine());
    QStringList cll = cl.split(" ");
    fArgv["0"] = cll.at(0);
    QFileInfo fi(cll.at(0));
    fArgv["path"] = fi.absolutePath();
    for (int i = 1; i < cll.count(); i++) {
        QStringList opt = cll.at(i).split("=");
        if (opt.count() == 1) {
            opt.append("");
        }
        fArgv[opt.at(0).toLower()] = opt.at(1);
    }
}

bool CommandLine::value(const QString &k, QString &v)
{
    if (fArgv.contains(k)) {
        v = fArgv[k];
        return true;
    }
    return false;
}
