#include "c5progressdialog.h"
#include "ui_c5progressdialog.h"

C5ProgressDialog::C5ProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::C5ProgressDialog)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
}

C5ProgressDialog::~C5ProgressDialog()
{
    delete ui;
}

void C5ProgressDialog::setMax(int max)
{
    fMax = max;
    ui->progressBar->setMaximum(max);
}

int C5ProgressDialog::exec()
{
    return QDialog::exec();
}

void C5ProgressDialog::updateProgressValue(int value)
{
    ui->lbHint->setText(QString("%1/%2").arg(value).arg(fMax));
    ui->progressBar->setValue(value);
    qApp->processEvents();
}

void C5ProgressDialog::updateProgressValueWithMessage(int value, const QString &msg)
{
    ui->lbHint->setText(QString("%1/%2 %3").arg(value).arg(fMax).arg(msg));
    ui->progressBar->setValue(value);
    qApp->processEvents();
}
