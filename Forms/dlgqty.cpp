#include "dlgqty.h"
#include "ui_dlgqty.h"

namespace {

double parseQtyText(const QString &raw)
{
    QString t = raw.trimmed();
    t.replace(QLatin1Char(','), QLatin1Char('.'));
    t.remove(QLatin1Char(' '));
    bool ok = false;
    const double v = t.toDouble(&ok);
    return ok ? v : 0.0;
}

}

DlgQty::DlgQty(C5User *user) :
    C5Dialog(user),
    ui(new Ui::DlgQty)
{
    ui->setupUi(this);
}

DlgQty::~DlgQty()
{
    delete ui;
}

bool DlgQty::getQty(double& qty, const QString &name, C5User *user)
{
    DlgQty d(user);
    d.ui->label->setText(name);

    if(d.exec() == QDialog::Accepted) {
        qty = parseQtyText(d.ui->leQty->text());
        return qty > 0.000001;
    }

    return false;
}

void DlgQty::on_btnClear_clicked()
{
    ui->leQty->clear();
}

void DlgQty::click(const QString &c)
{
    QString text = ui->leQty->text();

    if(c == QLatin1String(".")) {
        if(text.contains(QLatin1Char('.'))) {
            return;
        }

        if(text.isEmpty()) {
            ui->leQty->setText(QStringLiteral("0."));
        } else {
            ui->leQty->setText(text + QLatin1Char('.'));
        }

        return;
    }

    if(c == QLatin1String("0")) {
        if(text.isEmpty()) {
            ui->leQty->setText(QStringLiteral("0"));
            return;
        }

        /* Leading zeros without a decimal are useless ("00", "01"...). */
        if(!text.contains(QLatin1Char('.')) && text == QLatin1String("0")) {
            return;
        }
    } else if(text == QLatin1String("0")) {
        /* Replace lone integer zero: 0 + 8 → 8, not 08. */
        ui->leQty->setText(c);
        return;
    }

    ui->leQty->setText(text + c);
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

void DlgQty::on_leQty_returnPressed()
{
    on_btnOk_clicked();
}

void DlgQty::on_btnCancel_clicked()
{
    reject();
}

void DlgQty::on_btnOk_clicked()
{
    if(parseQtyText(ui->leQty->text()) > 0.000001) {
        accept();
    }
}
