#include "c5datasynchronize.h"
#include "ui_c5datasynchronize.h"
#include "c5filelogwriter.h"
#include <QScrollBar>
#include <QMenu>

C5DataSynchronize::C5DataSynchronize(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5DataSynchronize)
{
    ui->setupUi(this);
}

C5DataSynchronize::~C5DataSynchronize()
{
    delete ui;
}

void C5DataSynchronize::on_btnSaveSyncTables_clicked()
{
    C5Database ds(fDBParams);
    ds.exec("show tables");
    QStringList tables;
    while (ds.nextRow()) {
        tables.append(ds.getString(0));
    }
    if (tables.isEmpty()) {
        c5log("No tables to be syncronized.");
        return;
    } else {
        c5log("Tables to be syncronized: \r\n" + tables.join("\r\n"));
    }
    QString logFileName = "syncdb_" + fDBParams.at(4) + ".log";
    //C5Database dm(fMainDbParams.at(2), fMainDbParams.at(3), fMainDbParams.at(4), fMainDbParams.at(5));
    if (!ds.open()) {
        c5logf(QString("Database %1 (%2:%3) inaccesible").arg(fDBParams.at(4)).arg(fDBParams.at(0)).arg(fDBParams.at(1)), logFileName);
        return;;
    }
    ds.exec("show tables");
    c5logf(QString("Start dropping triggers for tables on %1").arg(fDBParams.at(4)), logFileName);
    while (ds.nextRow()) {
        ds.execDirect(QString("drop trigger if exists %1_ai").arg(ds.getString(0)));
        ds.execDirect(QString("drop trigger if exists %1_bu").arg(ds.getString(0)));
        ds.execDirect(QString("drop trigger if exists %1_bd").arg(ds.getString(0)));
    }
    if (ui->leAddSyncTable->text().toLower() == "clear") {
        c5logf(QString("Clear synchronization on  %1").arg(fDBParams.at(4)), logFileName);
        C5Message::info(tr("Done"));
        return;
    }
    c5logf(QString("Start creating triggers for tables on %1").arg(fDBParams.at(4)), logFileName);
    foreach (const QString &t, tables) {
        //SKIP MAIN TABLES, OTHERWISE NOTHING WILL TO WORK
        if (t.compare("s_syncronize", Qt::CaseInsensitive) == 0) {
            continue;
        }
        if (!ds.execDirect(QString("CREATE  TRIGGER `%1_ai` \
            AFTER INSERT ON `%1`  \
            FOR EACH ROW \
            BEGIN \
            insert into s_syncronize (f_db, f_table, f_recid, f_op, f_date) values (%2, '%3', new.f_id, 1, current_timestamp); \
                                   END").arg(t).arg(ui->leAddSyncTable->text()).arg(t))) {
            c5logf(ds.fLastError + " " + t, logFileName);
        }
        if (!ds.execDirect(QString("CREATE  TRIGGER `%1_bu` \
            BEFORE update ON `%1` \
            FOR EACH ROW  \
            BEGIN \
            insert into s_syncronize (f_db, f_table, f_recid, f_op, f_date) values (%2, '%3', new.f_id, 2, current_timestamp); \
            END ").arg(t).arg(ui->leAddSyncTable->text()).arg(t))) {
            c5logf(ds.fLastError + " " + t, logFileName);
        }
        if (!ds.execDirect(QString("CREATE  TRIGGER `%1_bd` \
            BEFORE delete ON `%1` \
            FOR EACH ROW  \
            BEGIN \
            insert into s_syncronize (f_db, f_table, f_recid, f_op, f_date) values (%2, '%3', old.f_id, 3, current_timestamp); \
            END ").arg(t).arg(ui->leAddSyncTable->text()).arg(t))) {
            c5logf(ds.fLastError + " " + t, logFileName);
        }
    }
    C5Message::info(tr("Done"));
}

void C5DataSynchronize::dropTriggers(C5Database &ds)
{
    ds.exec("show tables");
    QStringList tables;
    while (ds.nextRow()) {
        tables.append(ds.getString(0));
    }
    if (tables.isEmpty()) {
        c5log("No tables to be syncronized.");
        return;
    } else {
        c5log("Tables to be syncronized: \r\n" + tables.join("\r\n"));
    }
    QString logFileName = "syncdb_" + fDBParams.at(4) + ".log";
    //C5Database dm(fMainDbParams.at(2), fMainDbParams.at(3), fMainDbParams.at(4), fMainDbParams.at(5));
    if (!ds.open()) {
        c5logf(QString("Database %1 (%2:%3) inaccesible").arg(fDBParams.at(4)).arg(fDBParams.at(0)).arg(fDBParams.at(1)), logFileName);
        return;
    }
    ds.exec("show tables");
    c5logf(QString("Start dropping triggers for tables on %1").arg(fDBParams.at(4)), logFileName);
    while (ds.nextRow()) {
        ds.execDirect(QString("drop trigger if exists %1_ai").arg(ds.getString(0)));
        ds.execDirect(QString("drop trigger if exists %1_bu").arg(ds.getString(0)));
        ds.execDirect(QString("drop trigger if exists %1_bd").arg(ds.getString(0)));
    }
}


void C5DataSynchronize::on_btnRevert_clicked()
{
    C5Database ds(fDBParams);
    dropTriggers(ds);
    C5Message::error(tr("Synchronization removed"));
}
