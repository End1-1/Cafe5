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

    QStringList cll2;
    for (int i = 1; i < cll.count(); i++) {
        if (cll.at(i).mid(0, 2) == "--") {
            cll2.append(cll.at(i));
        } else {
            if (cll2.count() == 0) {
                continue;
            }
            cll2[cll2.count() - 1] += " " + cll.at(i);
        }
    }

    for (int i = 0; i < cll2.count(); i++) {
        QStringList opt = cll2.at(i).split("=");
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
