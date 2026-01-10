#include "calendar.h"
#include "ui_calendar.h"

Calendar::Calendar(C5User *user) :
    C5Dialog(user),
    ui(new Ui::Calendar)
{
    ui->setupUi(this);
}

Calendar::~Calendar()
{
    delete ui;
}

bool Calendar::getDate(QDate &d, C5User *user)
{
    Calendar *c = new Calendar(user);
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

bool Calendar::getDate2(QDate &d1, QDate &d2, C5User *user)
{
    Calendar *c = new Calendar(user);
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
