#include "nloadingdlg.h"
#include "ui_nloadingdlg.h"
#include <QTimer>

NLoadingDlg::NLoadingDlg(const QString &title, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::NLoadingDlg),
    mSecond(0)
{
    ui->setupUi(this);
    ui->lbTitle->setText(title);
    auto *t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &NLoadingDlg::timeout);
    t->start(1000);
    ui->wp->startAnimation();
}

NLoadingDlg::~NLoadingDlg()
{
    delete ui;
}

void NLoadingDlg::open()
{
    mSecond = 0;
}

void NLoadingDlg::reject()
{
}

void NLoadingDlg::accept()
{
}

void NLoadingDlg::resetSeconds()
{
    mSecond = 0;
    ui->label->setText(QString("%1 sec").arg(mSecond));
    open();
}

void NLoadingDlg::hide()
{
    QWidget::hide();
    mSecond = -999999999;
}

void NLoadingDlg::timeout()
{
    mSecond++;
    ui->label->setText(QString("%1 sec").arg(mSecond));

    if(mSecond == 1) {
        //   QDialog::open();
    }
}
