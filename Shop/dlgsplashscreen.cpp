#include "dlgsplashscreen.h"
#include "ui_dlgsplashscreen.h"
#include "goodsreserve.h"
#include <QTimer>

DlgSplashScreen::DlgSplashScreen() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgSplashScreen)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    ui->lbText->setText(tr("Prepare to work"));
    connect(this, &DlgSplashScreen::messageSignal, this, &DlgSplashScreen::message);
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DlgSplashScreen::timeout);
    timer->start(1000);
}

DlgSplashScreen::~DlgSplashScreen()
{
    delete ui;
}

void DlgSplashScreen::timeout()
{
    sender()->deleteLater();
    emit messageSignal(tr("Building store"));
    C5Database db1(__c5config.dbParams());
    C5Database db2(__c5config.dbParams());
    db1[":f_store"] = __c5config.defaultStore();
    db1.exec("delete from a_store_sale where f_store=:f_store");
    db1[":f_store"] = __c5config.defaultStore();
    db1.exec("select f_goods, sum(f_qty*f_type) as f_qty from a_store where f_store=:f_store group by 1 having sum(f_qty*f_type)>0");
    int i = 0;
    QString sql;
    while (db1.nextRow()) {
        if (i % 1000 == 0) {
            if (!sql.isEmpty()) {
                db2.exec(sql);
            }
            sql.clear();
        }
        if (sql.isEmpty()) {
            sql = QString("insert into a_store_sale (f_store, f_goods, f_qty, f_qtyreserve, f_qtyprogram) values (%1, %2, %3, 0, 0) ")
                    .arg(__c5config.defaultStore())
                    .arg(db1.getInt("f_goods"))
                    .arg(db1.getDouble("f_qty"));
        } else {
            sql += QString(",(%1, %2, %3, 0, 0) ")
                    .arg(__c5config.defaultStore())
                    .arg(db1.getInt("f_goods"))
                    .arg(db1.getDouble("f_qty"));
        }
        i++;
    }
    if (!sql.isEmpty()) {
        db2.exec(sql);
    }
    C5Database dr(__c5config.replicaDbParams());
    emit messageSignal(tr("Clear expired reservations"));
    dr.exec(QString("update a_store_reserve set f_state=%2, f_canceleddate=current_date, f_canceledtime=current_time, "
                    "f_message=concat(f_message, ' (%1)') where f_enddate<current_date or f_enddate is null and f_state=%3 ")
            .arg(tr("Expired"))
            .arg(GR_EXPIRED)
            .arg(GR_RESERVED));

    dr[":f_store"] = __c5config.defaultStore();
    dr[":f_state"] = GR_RESERVED;
    dr.exec("select f_goods, sum(f_qty) as f_qty from a_store_reserve where f_state=:f_state and f_store=:f_store group by 1");
    while (dr.nextRow()) {
        db2[":f_store"] = __c5config.defaultStore();
        db2[":f_goods"] = dr.getInt("f_goods");
        db2[":f_qtyreserve"] = dr.getDouble("f_qty");
        db2.exec("update a_store_sale set f_qtyreserve=:f_qtyreserve where f_store=:f_store and f_goods=:f_goods");
    }

    emit messageSignal(tr("All done!"));
    accept();
}

void DlgSplashScreen::message(const QString &text)
{
    ui->lbText->setText(text);
}
