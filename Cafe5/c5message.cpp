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
    return showMessage(errorStr, 2, yes, no);
}

int C5Message::info(const QString &infoStr, const QString &yes, const QString &no)
{
    return showMessage(infoStr, 1, yes, no);
}

int C5Message::question(const QString &questionStr, const QString &yes, const QString &no)
{
    return showMessage(questionStr, 3, yes, no);
}

int C5Message::showMessage(const QString &text, int tp, const QString &yes, const QString &no)
{
    C5Message *c5 = new C5Message(__c5config.fParentWidget);
    c5->ui->btnYes->setText(yes);
    c5->ui->btnCancel->setText(no);
    if (no.isEmpty()) {
        c5->ui->btnCancel->setVisible(false);
    }
    QString img;
    switch (tp) {
    case 1:
        img = "info";
        break;
    case 2:
        img = "error";
        break;
    case 3:
        img = "help";
        break;
    }
#ifdef WAITER
    c5->ui->btnYes->setMinimumHeight(50);
    c5->ui->btnCancel->setMinimumHeight(50);
    c5->ui->btnYes->setMinimumWidth(100);
    c5->ui->btnCancel->setMinimumWidth(100);
#endif
    c5->ui->img->setPixmap(QPixmap(QString(":/%1.png").arg(img)));
    c5->ui->label->setText(text);
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
