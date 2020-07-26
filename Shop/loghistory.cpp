#include "loghistory.h"
#include "ui_loghistory.h"
#include "c5logsystem.h"

LogHistory::LogHistory(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogHistory)
{
    ui->setupUi(this);
    mDate = QDate::currentDate();
    loadHistory();
    showMaximized();
}

LogHistory::~LogHistory()
{
    delete ui;
}

void LogHistory::loadHistory()
{
    ui->lbDate->setText(mDate.toString("dd/MM/yyyy"));
    QFile f(C5LogSystem::fileName(mDate));
    f.open(QIODevice::ReadOnly);
    ui->ptLog->setPlainText(f.readAll());
    f.close();
}

void LogHistory::on_btnNext_clicked()
{
    mDate = mDate.addDays(1);
    loadHistory();
}

void LogHistory::on_btnPreviouse_clicked()
{
    mDate = mDate.addDays(-1);
    loadHistory();
}
