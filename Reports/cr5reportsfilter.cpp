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
            addFilterField(tr("Partner"), c, r);
            r++;
            break;
        case cache_currency:
            addFilterField(tr("Crossrate"), c, r);
            r++;
            break;
        case cache_goods_store:
            addFilterField(tr("Store"), c, r);
            r++;
            break;
        case cache_goods_group:
            addFilterField(tr("Goods group"), c, r);
            r++;
            break;
        case cache_draft_sale_state:
            addFilterField(tr("State"), c, r);
            r++;
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
            case cache_goods_group:
            case cache_goods_store:
            case cache_currency:
            case cache_draft_sale_state:
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

void CR5ReportsFilter::addFilterField(const QString &title, const QString &data, int row)
{
    QStringList kv = data.split("-", Qt::SkipEmptyParts);
    auto *l = new QLabel(title, this);
    auto *e1 = new C5LineEditWithSelector(this);
    e1->setProperty("fieldname", kv.at(1));
    e1->setMaximumWidth(150);
    auto *e2 = new C5LineEditWithSelector(this);
    e1->setSelector(fDBParams, e2, kv.at(0).toInt());
    e2->setReadOnly(true);
    e2->setMinimumWidth(250);
    ui->gr->addWidget(l, row, 0);
    ui->gr->addWidget(e1, row, 1);
    ui->gr->addWidget(e2, row, 2);
    fCache[kv.at(0).toInt()] = e1;
}
