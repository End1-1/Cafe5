#include "calendar.h"
#include "ui_calendar.h"

Calendar::Calendar() :
    C5Dialog(__c5config.dbParams()),
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
    bool result = false;
    if (c->exec() == QDialog::Accepted) {
        d = c->ui->calendarWidget->selectedDate();
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
