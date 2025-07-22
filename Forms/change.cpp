#include "change.h"
#include "ui_change.h"
#include "c5utils.h"
#include <QDoubleValidator>

Change::Change() :
    C5Dialog(),
    ui(new Ui::Change)
{
    ui->setupUi(this);
    ui->leReceived->setValidator(new QDoubleValidator(0, 999999999, 2));
    fButtons.append(ui->btn1);
    fButtons.append(ui->btn2);
    fButtons.append(ui->btn3);
    fButtons.append(ui->btn4);
    fButtons.append(ui->btn5);
    fButtons.append(ui->btn6);
    fButtons.append(ui->btn7);
    fButtons.append(ui->btn8);
    fButtons.append(ui->btn9);
    fButtons.append(ui->btn0);
    fButtons2.append(ui->btn1000);
    fButtons2.append(ui->btn2000);
    fButtons2.append(ui->btn5000);
    fButtons2.append(ui->btn10000);
    fButtons2.append(ui->btn15000);
    fButtons2.append(ui->btn20000);
}

Change::~Change()
{
    delete ui;
}

bool Change::getReceived(double& v)
{
    Change c;
    c.ui->leAmount->setDouble(v);

    for(int i = 0; i < c.fButtons.count(); i++) {
        QPushButton *b = c.fButtons.at(i);
        connect(b, &QPushButton::clicked, &c, &Change::buttonClicked);
    }

    for(int i = 0; i, i < c.fButtons2.count(); i++) {
        QPushButton *b = c.fButtons2.at(i);
        connect(b, &QPushButton::clicked, &c, &Change::buttonClicked2);
    }

    c.adjustSize();

    if(c.exec() == QDialog::Accepted) {
        v = c.ui->leReceived->getDouble();
        return true;
    }

    return false;
}

void Change::buttonClicked()
{
    QPushButton *b = static_cast<QPushButton*>(sender());
    ui->leReceived->setText(ui->leReceived->text() + b->text());
}

void Change::buttonClicked2()
{
    ui->leReceived->setText(static_cast<QPushButton*>(sender())->text());
}

void Change::on_leReceived_textChanged(const QString &arg1)
{
    ui->leChange->setDouble(str_float(arg1) - ui->leAmount->getDouble());
}

void Change::on_btnCancel_clicked()
{
    reject();
}

void Change::on_btnOK_clicked()
{
    accept();
}

void Change::on_leReceived_returnPressed()
{
    on_btnOK_clicked();
}
