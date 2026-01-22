#include "cr5goodsreservationsfilter.h"
#include "ui_cr5goodsreservationsfilter.h"
#include "goodsreserve.h"
#include "format_date.h"

CR5GoodsReservationsFilter::CR5GoodsReservationsFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5GoodsReservationsFilter)
{
    ui->setupUi(this);
}

CR5GoodsReservationsFilter::~CR5GoodsReservationsFilter()
{
    delete ui;
}

QString CR5GoodsReservationsFilter::condition()
{
    QString result = "where ";

    if(ui->cbDateOption->currentIndex() == 0) {
        result += QString("rs.f_state=%1").arg(GR_RESERVED);
    } else {
        QString datestr;

        switch(ui->cbDateOption->currentIndex()) {
        case 1:
            datestr = "f_date";
            break;

        case 2:
            datestr = "f_enddate";
            break;

        case 3:
            datestr = "f_completeddate";
            break;

        case 4:
            datestr = "f_canceleddate";
            break;
        }

        result += QString("rs.%1 between '%2' and '%3' ")
                  .arg(datestr, ui->leStart->date().toString(FORMAT_DATE_TO_STR_MYSQL), ui->leEnd->date().toString(FORMAT_DATE_TO_STR_MYSQL));
    }

    return result;
}
