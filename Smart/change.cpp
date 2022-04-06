#include "change.h"
#include "ui_change.h"
#include <QDoubleValidator>

Change::Change() :
    C5Dialog(__c5config.dbParams()),
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
    for (auto *b: fButtons) {
        b->setText(float_str(str_float(b->text()), 2));
        connect(b, &QPushButton::clicked, this, &Change::buttonClicked);
    }
}

Change::~Change()
{
    delete ui;
}

bool Change::getReceived(double &v)
{
    Change c;
    c.ui->leAmount->setDouble(v);
    if (c.exec() == QDialog::Accepted) {
        v = c.ui->leReceived->getDouble();
        return true;
    }
    return false;
}

void Change::buttonClicked()
{
    QPushButton *b = static_cast<QPushButton*>(sender());
    ui->leReceived->setText(b->text());
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
