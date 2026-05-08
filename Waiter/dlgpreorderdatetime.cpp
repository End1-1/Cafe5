#include "dlgpreorderdatetime.h"

#include <QCalendarWidget>
#include <QDate>
#include <QDateTime>

#include "c5message.h"
#include "ui_dlgpreorderdatetime.h"

DlgPreorderDateTime::DlgPreorderDateTime(C5User *user, QWidget *parent)
    : C5Dialog(user, parent),
      ui(new Ui::DlgPreorderDateTime)
{
    ui->setupUi(this);
    setWindowTitle(tr("Cafe5"));
    ui->lbTitle->setText(tr("Preorder date and time"));
    ui->label_hour->setText(tr("Hour"));
    ui->label_minute->setText(tr("Minute"));
    ui->btnOk->setText(tr("OK"));
    ui->btnClear->setText(tr("Clear date"));
    ui->btnCancel->setText(tr("Cancel"));
    ui->calendarWidget->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    ui->calendarWidget->setFocusPolicy(Qt::NoFocus);
    ui->calendarWidget->setMinimumDate(QDate::currentDate());
}

DlgPreorderDateTime::~DlgPreorderDateTime()
{
    delete ui;
}

void DlgPreorderDateTime::setInitial(const QDateTime &dt)
{
    ui->calendarWidget->setMinimumDate(QDate::currentDate());
    const QDateTime now = QDateTime::currentDateTime();
    QDateTime use = dt.isValid() ? dt : now;

    if(use < now) {
        use = now;
    }

    ui->calendarWidget->setSelectedDate(use.date());
    ui->sbHour->setValue(use.time().hour());
    ui->sbMinute->setValue(use.time().minute());
    mClearRequested = false;
}

QDateTime DlgPreorderDateTime::selectedDateTime() const
{
    QDateTime dt(ui->calendarWidget->selectedDate(), QTime(ui->sbHour->value(), ui->sbMinute->value()));
    return dt;
}

void DlgPreorderDateTime::on_btnOk_clicked()
{
    const QDateTime sel = selectedDateTime();
    const QDateTime now = QDateTime::currentDateTime();

    if(sel < now) {
        C5Message::error(tr("Preorder time cannot be in the past"));
        return;
    }

    mClearRequested = false;
    accept();
}

void DlgPreorderDateTime::on_btnClear_clicked()
{
    mClearRequested = true;
    accept();
}

void DlgPreorderDateTime::on_btnCancel_clicked()
{
    reject();
}
