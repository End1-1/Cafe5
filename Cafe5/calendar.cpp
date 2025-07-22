#include "calendar.h"
#include "ui_calendar.h"

Calendar::Calendar() :
    C5Dialog(),
    ui(new Ui::Calendar)
{
    ui->setupUi(this);
}

Calendar::~Calendar()
{
    delete ui;
}

bool Calendar::getDate(QDate &d)
{
    Calendar *c = new Calendar();
    c->ui->calendarWidget_2->setVisible(false);
    c->adjustSize();
    bool result = false;

    if(c->exec() == QDialog::Accepted) {
        d = c->ui->calendarWidget->selectedDate();
        result = true;
    }

    delete c;
    return result;
}

bool Calendar::getDate2(QDate &d1, QDate &d2)
{
    Calendar *c = new Calendar();
    bool result = false;

    if(c->exec() == QDialog::Accepted) {
        d1 = c->ui->calendarWidget->selectedDate();
        d2 = c->ui->calendarWidget_2->selectedDate();
        result = true;
    }

    delete c;
    return result;
}

void Calendar::on_btnCancel_clicked()
{
    reject();
}

void Calendar::on_btnOk_clicked()
{
    accept();
}
