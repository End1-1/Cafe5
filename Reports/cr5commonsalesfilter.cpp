#include "cr5commonsalesfilter.h"
#include "ui_cr5commonsalesfilter.h"
#include "c5cache.h"
#include "cr5saleflags.h"

CR5CommonSalesFilter::CR5CommonSalesFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5CommonSalesFilter)
{
    ui->setupUi(this);
    ui->leState->setSelector(ui->leStateName, cache_order_state).setMultiselection(true);
    ui->leHall->setSelector(ui->leHallName, cache_halls).setMultiselection(true);
    ui->leTable->setSelector(ui->leTableName, cache_tables).setMultiselection(true);
    ui->leStaff->setSelector(ui->leStaffName, cache_users).setMultiselection(true);
    ui->leBuyer->setSelector(ui->leBuyerName, cache_goods_partners).setMultiselection(true);
    ui->dt1->setDateTime(QDateTime::fromString(QDate::currentDate().toString("dd.MM.yyyy") + " 00:00", "dd.MM.yyyy HH:mm"));
    ui->dt2->setDateTime(QDateTime::fromString(QDate::currentDate().addDays(1).toString("dd.MM.yyyy") + " 00:00", "dd.MM.yyyy HH:mm"));
}

CR5CommonSalesFilter::~CR5CommonSalesFilter()
{
    delete ui;
}

QString CR5CommonSalesFilter::condition()
{
    QString result;

    if(ui->chUseClosingDateTime->isChecked()) {
        result = QString(" cast(concat(oh.f_dateclose, ' ', oh.f_timeclose) as datetime) between '%1' and '%2'")
                 .arg(ui->dt1->dateTime().toString(FORMAT_DATETIME_TO_STR_MYSQL),
                      ui->dt2->dateTime().toString(FORMAT_DATETIME_TO_STR_MYSQL));
    } else {
        if(ui->chDoNotUseCashDate->isChecked()) {
            result = " oh.f_datecash is null ";
        } else {
            result = " oh.f_datecash between " + ui->deStart->toMySQLDate() + " and " + ui->deEnd->toMySQLDate() + " ";
        }
    }

    if(!ui->leState->isEmpty()) {
        result += " and oh.f_state in (" + ui->leState->text() + ") ";
    }

    if(!ui->leHall->isEmpty()) {
        result += " and oh.f_hall in (" + ui->leHall->text() + ") ";
    }

    if(!ui->leTable->isEmpty()) {
        result += " and oh.f_table in (" + ui->leTable->text() + ") ";
    }

    if(!ui->leStaff->isEmpty()) {
        result += " and oh.f_staff in (" + ui->leStaff->text() + ") ";
    }

    if(ui->rbRIYes->isChecked()) {
        result += " and oh.f_amounttotal<0 ";
    }

    if(ui->rbRINo->isChecked()) {
        result += " and oh.f_amounttotal>=0 ";
    }

    if(!ui->leBuyer->isEmpty()) {
        result += " and oh.f_partner in (" + ui->leBuyer->text() + ") ";
    }

    if(ui->rbTaxYes->isChecked()) {
        result += " and ot.f_receiptnumber>0 ";
    }

    if(ui->rbTaxNo->isChecked()) {
        result += " and (ot.f_receiptnumber=0 or ot.f_receiptnumber is null) ";
    }

    if(ui->rbByuyerYes->isChecked()) {
        result += " and oh.f_partner>0 ";
    }

    if(ui->rbBuyerNo->isChecked()) {
        result += " and oh.f_partner=0 ";
    }

    result += fFlags;

    if(ui->chptBank->isChecked() || ui->chptCard->isChecked()
            || ui->chptCash->isChecked()
            || ui->chptOther->isChecked()
            || ui->chptHotel->isChecked()) {
        result += ui->chptCash->isChecked() ? " and oh.f_amountcash>0 " : " and oh.f_amountcash=0 ";
        result += ui->chptCard->isChecked() ? " and oh.f_amountcard>0 " : " and oh.f_amountcard=0 ";
        result += ui->chptBank->isChecked() ? " and oh.f_amountbank>0 " : " and oh.f_amountbank=0 ";
        result += ui->chptOther->isChecked() ? " and oh.f_amountother>0 " : " and oh.f_amountother=0 ";
        result += ui->chptHotel->isChecked() ? " and oh.f_hotel>0 " : " and oh.f_hotel=0 ";
    }

    if(ui->chptComplimentary->isChecked()) {
        result += " and oh.f_amounttotal=0 ";
    }

    if(ui->checkBoxNotComplimentary->isChecked()) {
        result += " and oh.f_amounttotal>0 ";
    }

    return result;
}

QDate CR5CommonSalesFilter::date1() const
{
    return ui->deStart->date();
}

QDate CR5CommonSalesFilter::date2() const
{
    return ui->deEnd->date();
}

bool CR5CommonSalesFilter::complimentary()
{
    return ui->chptComplimentary->isChecked();
}

bool CR5CommonSalesFilter::notComplimentary()
{
    return ui->checkBoxNotComplimentary->isChecked();
}

void CR5CommonSalesFilter::on_btnFlags_clicked()
{
    CR5SaleFlags f(mUser);

    if(f.exec() == QDialog::Accepted) {
        fFlags = f.flagsCond();
    }
}
void CR5CommonSalesFilter::on_chUseClosingDateTime_clicked(bool checked)
{
    ui->dt1->setEnabled(checked);
    ui->dt2->setEnabled(checked);
}

void CR5CommonSalesFilter::on_checkBoxNotComplimentary_clicked(bool checked)
{
    if(checked) {
        if(ui->chptComplimentary->isChecked()) {
            ui->chptComplimentary->setChecked(false);
        }
    }
}

void CR5CommonSalesFilter::on_chptComplimentary_clicked(bool checked)
{
    if(checked) {
        if(ui->checkBoxNotComplimentary->isChecked()) {
            ui->checkBoxNotComplimentary->setChecked(false);
        }
    }
}
