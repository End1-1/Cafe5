#include "c5databasesync.h"
#include "c5filelogwriter.h"
#include "c5database.h"
#include "c5pointsync.h"
#include <QSettings>

static QSettings _s(_ORGANIZATION_, _APPLICATION_);

C5DatabaseSync::C5DatabaseSync(QObject *parent) :
    C5ThreadObject(parent)
{

}

void C5DatabaseSync::run()
{
    c5log("Syncronization of databases started.");
    C5Database db(_s.value("dbhost").toString(), _s.value("dbschema").toString(), _s.value("dbuser").toString(), _s.value("dbpassword").toString());
    if (!db.open()) {
        c5log("Connection to master databases failed.");
        c5log(db.fLastError);
        return;
    }
    c5log("Connected to master database.");
    c5log("Gettings list of databases.");
    db.exec("select * from s_db ");
    QList<QStringList> databases;
    QStringList mainDatabase;
    while (db.nextRow()) {
        QStringList d;
        d.append(db.getString("f_id"));
        d.append(db.getString("f_name"));
        d.append(db.getString("f_host"));
        d.append(db.getString("f_db"));
        d.append(db.getString("f_user"));
        d.append(db.getString("f_password"));
        if (db.getInt("f_main") == 1) {
            mainDatabase = d;
            continue;
        }
        databases.append(d);
    }
    if (databases.isEmpty()) {
        c5log("No databases to be syncronized.");
        return;
    } else {
        QString d;
        foreach (const QStringList &sl, databases) {
            d += sl.at(1) + "\r\n";
        }
        c5log("Databases to be syncronized: \r\n" + d);
    }
    foreach (const QStringList &sl, databases) {
        C5PointSync *ps = new C5PointSync(mainDatabase, sl, this);
        ps->start();
    }
}
