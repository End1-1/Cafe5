#include "cr5reportsfilter.h"
#include "ui_cr5reportsfilter.h"
#include "c5cache.h"
#include "c5lineeditwithselector.h"

CR5ReportsFilter::CR5ReportsFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5ReportsFilter)
{
    ui->setupUi(this);
}


CR5ReportsFilter::~CR5ReportsFilter()
{
    delete ui;
}

void CR5ReportsFilter::setFields(const QStringList &cache)
{
    int r = 2;
    for (const QString &c: cache) {
        QStringList kv = c.split("-", Qt::SkipEmptyParts);
        switch (kv.at(0).toInt()) {
        case cache_goods_partners:
            auto *l = new QLabel(tr("Partner"), this);
            auto *e1 = new C5LineEditWithSelector(this);
            e1->setProperty("fieldname", kv.at(1));
            auto *e2 = new C5LineEditWithSelector(this);
            e1->setSelector(fDBParams, e2, kv.at(0).toInt());
            e2->setReadOnly(true);
            ui->gr->addWidget(l, r, 0);
            ui->gr->addWidget(e1, r, 1);
            ui->gr->addWidget(e2, r, 2);
            fCache[kv.at(0).toInt()] = e1;
            break;
        }
    }
}

QString CR5ReportsFilter::condition()
{
    return "";
}

QString CR5ReportsFilter::replacement()
{
    QString cond = " ";
    for (QMap<int, C5LineEditWithSelector*>::const_iterator it = fCache.constBegin(); it != fCache.constEnd(); it++) {
        if (!it.value()->isEmpty()) {
            switch (it.key()) {
            case cache_goods_partners:
                in(cond, it.value()->property("fieldname").toString(), it.value());
                break;
            }
        }
    }
    return cond;
}

QString CR5ReportsFilter::d1()
{
    return ui->leDate1->toMySQLDate();
}

QString CR5ReportsFilter::d2()
{
    return ui->leDate2->toMySQLDate();
}
