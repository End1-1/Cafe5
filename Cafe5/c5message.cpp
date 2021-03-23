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
}

C5Message::~C5Message()
{
    delete ui;
}

int C5Message::error(const QString &errorStr)
{
    return showMessage(errorStr, "red");
}

int C5Message::info(const QString &infoStr)
{
    return showMessage(infoStr, "green");
}

int C5Message::question(const QString &questionStr)
{
    return showMessage(questionStr, "blue");
}

int C5Message::showMessage(const QString &text, const QString &color)
{
    C5Message *c5 = new C5Message(__c5config.fParentWidget);
#ifdef FRONTDESK
    c5->ui->te->setText(QString("<html><body><center><h4><font color=\"%1\">%2</font></h4></center></body></html>").arg(color).arg(text));
#else
    c5->ui->btnYes->setMinimumHeight(50);
    c5->ui->btnCancel->setMinimumHeight(50);
    c5->ui->te->setText(QString("<html><body><center><h4><font color=\"%1\">%2</font></h1></center></body></html>").arg(color).arg(text));
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
