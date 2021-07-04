#include "dlgqty.h"
#include "ui_dlgqty.h"

DlgQty::DlgQty() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgQty)
{
    ui->setupUi(this);
}

DlgQty::~DlgQty()
{
    delete ui;
}

bool DlgQty::getQty(double &qty, const QString &name)
{
    DlgQty d;
    d.ui->label->setText(name);
    if (d.exec() == QDialog::Accepted) {
        qty = d.ui->leQty->getDouble();
        return true;
    }
    return false;
}

void DlgQty::on_btnClear_clicked()
{
    ui->leQty->clear();
}

void DlgQty::click(const QString &c)
{
    if (c == "0" && ui->leQty->getDouble() < 0.001) {
        return;
    }
    if (c == ".") {
        if (ui->leQty->getDouble() < 0.001) {
            ui->leQty->setText("0.");
        } else {
            if (ui->leQty->text().contains(".")) {
                return;
            }
            ui->leQty->setText(ui->leQty->text() + ".");
        }
        return;
    }
    ui->leQty->setText(ui->leQty->text() + c);
}

void DlgQty::on_btn1_clicked()
{
    click("1");
}

void DlgQty::on_btn2_clicked()
{
    click("2");
}

void DlgQty::on_btn3_clicked()
{
    click("3");
}

void DlgQty::on_btn4_clicked()
{
    click("4");
}

void DlgQty::on_btn5_clicked()
{
    click("5");
}

void DlgQty::on_btn6_clicked()
{
    click("6");
}

void DlgQty::on_btn7_clicked()
{
    click("7");
}

void DlgQty::on_btn8_clicked()
{
    click("8");
}

void DlgQty::on_btn9_clicked()
{
    click("9");
}

void DlgQty::on_btnDec_clicked()
{
    click(".");
}

void DlgQty::on_btn0_clicked()
{
    click("0");
}

void DlgQty::on_btn05_clicked()
{
    ui->leQty->setText("0.5");
}

void DlgQty::on_btn15_clicked()
{
    ui->leQty->setText("1.5");
}

void DlgQty::on_btn033_clicked()
{
    ui->leQty->setText("0.33");
}

void DlgQty::on_btn025_clicked()
{
    ui->leQty->setText("0.25");
}

void DlgQty::on_pushButton_11_clicked()
{
    reject();
}

void DlgQty::on_pushButton_12_clicked()
{
    accept();
}
