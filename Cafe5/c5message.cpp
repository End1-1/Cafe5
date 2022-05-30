#include "c5message.h"
#include "ui_c5message.h"
#include "c5config.h"

C5Message::C5Message(QWidget *parent) :
#ifdef WAITER
    QDialog(parent, Qt::FramelessWindowHint),
#else
    QDialog(parent),
#endif
    ui(new Ui::C5Message)
{
    ui->setupUi(this);
#ifndef WAITER
    ui->frame->setFrameShape(QFrame::NoFrame);
#endif
}

C5Message::~C5Message()
{
    delete ui;
}

int C5Message::error(const QString &errorStr, const QString &yes, const QString &no)
{
    return showMessage(errorStr, "red", yes, no);
}

int C5Message::info(const QString &infoStr, const QString &yes, const QString &no)
{
    return showMessage(infoStr, "green", yes, no);
}

int C5Message::question(const QString &questionStr, const QString &yes, const QString &no)
{
    return showMessage(questionStr, "blue", yes, no);
}

int C5Message::showMessage(const QString &text, const QString &color, const QString &yes, const QString &no)
{
    C5Message *c5 = new C5Message(__c5config.fParentWidget);
    c5->ui->btnYes->setText(yes);
    c5->ui->btnCancel->setText(no);
    if (no.isEmpty()) {
        c5->ui->btnCancel->setVisible(false);
    }
#ifdef FRONTDESK
    c5->ui->label->setText(QString("<html><body><center><h4><font color=\"%1\">%2</font></h4></center></body></html>").arg(color, text));
#else
    c5->ui->btnYes->setMinimumHeight(50);
    c5->ui->btnCancel->setMinimumHeight(50);
    c5->ui->label->setText(QString("<html><body><center><h4><font color=\"%1\">%2</font></h1></center></body></html>").arg(color, text));
#endif
    c5->adjustSize();
    int result = c5->exec();
    delete c5;
    return result;
}

void C5Message::on_btnYes_clicked()
{
    accept();
}

void C5Message::on_btnCancel_clicked()
{
    reject();
}
