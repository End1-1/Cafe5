#include "nloadingdlg.h"
#include "ui_nloadingdlg.h"
#include <QTimer>

NLoadingDlg::NLoadingDlg(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    mSecond(0),
    ui(new Ui::NLoadingDlg)
{
    ui->setupUi(this);
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
    QDialog::open();
}

void NLoadingDlg::reject()
{
}

void NLoadingDlg::accept()
{
}

void NLoadingDlg::timeout()
{
    mSecond++;
    ui->label->setText(QString("%1 sec").arg(mSecond));
}
