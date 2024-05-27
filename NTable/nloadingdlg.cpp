#include "nloadingdlg.h"
#include "ui_nloadingdlg.h"
#include <QTimer>

NLoadingDlg::NLoadingDlg(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::NLoadingDlg),
    mSecond(0)
{
    ui->setupUi(this);
    auto *t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &NLoadingDlg::timeout);
    t->start(1000);
}

NLoadingDlg::~NLoadingDlg()
{
    delete ui;
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
