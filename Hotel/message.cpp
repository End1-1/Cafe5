#include "message.h"
#include "ui_message.h"

Message::Message() :
    Dialog(),
    ui(new Ui::Message)
{
    ui->setupUi(this);
    fResult = rCancel;
    ui->btnCancel->setVisible(false);
    ui->btnClose->setVisible(false);
    ui->btnYes->setVisible(false);
    ui->btnNo->setVisible(false);
}

Message::~Message()
{
    delete ui;
}

void Message::showMessage(const QString &msg)
{
    Message *m = new Message();
    m->ui->lbMessage->setText(msg);
    m->ui->btnClose->setVisible(true);
    m->adjustSize();
    m->exec();
    delete m;
}

Message::Result Message::yesNoCancel(const QString &msg)
{
    Message *m = new Message();
    m->ui->lbMessage->setText(msg);
    m->ui->btnYes->setVisible(true);
    m->ui->btnNo->setVisible(true);
    m->ui->btnCancel->setVisible(true);
    m->adjustSize();
    m->exec();
    Result result = m->fResult;
    delete m;
    return result;
}

Message::Result Message::yesNo(const QString &msg)
{
    Message *m = new Message();
    m->ui->lbMessage->setText(msg);
    m->ui->btnYes->setVisible(true);
    m->ui->btnNo->setVisible(true);
    m->adjustSize();
    m->exec();
    Result result = m->fResult;
    delete m;
    return result;
}

void Message::setWidgetContainer()
{

}

void Message::on_btnYes_clicked()
{
    fResult = rYes;
    accept();
}

void Message::on_btnClose_clicked()
{
    fResult = rClose;
    accept();
}

void Message::on_btnNo_clicked()
{
    fResult = rNo;
    accept();
}

void Message::on_btnCancel_clicked()
{
    fResult = rCancel;
    accept();
}
