#include "cr5carvisitsfilter.h"
#include "ui_cr5carvisitsfilter.h"
#include "c5cache.h"

CR5CarVisitsFilter::CR5CarVisitsFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5CarVisitsFilter)
{
    ui->setupUi(this);
    ui->leCostumer->setSelector(ui->leCostumerName, cache_goods_partners);
}

CR5CarVisitsFilter::~CR5CarVisitsFilter()
{
    delete ui;
}

QString CR5CarVisitsFilter::condition()
{
    QString where = QString(" oh.f_state=2 and oh.f_datecash between %1 and %2").arg(ui->deStart->toMySQLDate()).arg(
                        ui->deEnd->toMySQLDate());
    if (!ui->leCostumer->isEmpty()) {
        where += " and car.f_costumer in (" + ui->leCostumer->text() + ") ";
    }
    if (!ui->leGovNumber->isEmpty()) {
        QString gov;
        QStringList l = ui->leGovNumber->text().split(",", Qt::SkipEmptyParts);
        for (int i = 0; i < l.count(); i++) {
            QString t = l[i].trimmed();
            if (!t.isEmpty()) {
                if (t.at(0) != '\'') {
                    t = "'" + t + "'";
                }
                if (!gov.isEmpty()) {
                    gov += ",";
                }
                gov += t;
            }
        }
        if (!gov.isEmpty()) {
            where += " and car.f_govnumber in (" + gov + ") ";
        }
    }
    return where;
}

QString CR5CarVisitsFilter::filterText()
{
    return QString("%1 %1-%2").arg(tr("Date range"), ui->deStart->text(), ui->deEnd->text());
}
