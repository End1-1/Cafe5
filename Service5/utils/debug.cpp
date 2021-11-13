#include "debug.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

void __debug_log_to_file(const QVariant &msg) {
    QDir dir;
    if (!dir.exists(dir.homePath() + "/Service5")) {
        Q_ASSERT(dir.mkpath(dir.homePath() + "/Service5"));
    }
    QFile f(dir.homePath() + "/Service5" + "/log." + QDate::currentDate().toString("ddMMyyyy") + ".txt");
    f.open(QIODevice::Append);
    QTextStream ts(&f);
    ts << QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm") << ": " << msg.toString() << "\r\n" ;
    f.close();
}
