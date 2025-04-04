#include "cr5saleflags.h"
#include "ui_cr5saleflags.h"
#include "c5database.h"

CR5SaleFlags::CR5SaleFlags(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::CR5SaleFlags)
{
    ui->setupUi(this);
    C5Database db(fDBParams);
    db.exec("select * from o_flags");
    while (db.nextRow()) {
        QLabel *l;
        QWidget *w;
        switch (db.getInt("f_id")) {
            case 1:
                l = ui->lbName1;
                w = ui->w1;
                break;
            case 2:
                l = ui->lbName2;
                w = ui->w2;
                break;
            case 3:
                l = ui->lbName3;
                w = ui->w3;
                break;
            case 4:
                l = ui->lbName4;
                w = ui->w4;
                break;
            case 5:
                l = ui->lbName5;
                w = ui->w5;
                break;
        }
        l->setText(db.getString("f_name"));
        w->setEnabled(db.getInt("f_enabled") > 0);
    }
}

CR5SaleFlags::~CR5SaleFlags()
{
    delete ui;
}

QString CR5SaleFlags::flagsCond() const
{
    QString result;
    if (ui->w1->isEnabled()) {
        if (ui->rbYes1->isChecked()) {
            result += (result.isEmpty() ? " and " : " and ") + QString("ohf.f_1=1 ");
        }
        if (ui->rbNo1->isChecked()) {
            result += (result.isEmpty() ? " and " : " and ") + QString("ohf.f_1=0 ");
        }
    }
    if (ui->w2->isEnabled()) {
        if (ui->rbYes2->isChecked()) {
            result += (result.isEmpty() ? " and " : " and ") + QString("ohf.f_2=1 ");
        }
        if (ui->rbNo2->isChecked()) {
            result += (result.isEmpty() ? " and " : " and ") + QString("ohf.f_2=0 ");
        }
    }
    if (ui->w3->isEnabled()) {
        if (ui->rbYes3->isChecked()) {
            result += (result.isEmpty() ? " and " : " and ") + QString("ohf.f_3=1 ");
        }
        if (ui->rbNo3->isChecked()) {
            result += (result.isEmpty() ? " and " : " and ") + QString("ohf.f_3=0 ");
        }
    }
    if (ui->w4->isEnabled()) {
        if (ui->rbYes4->isChecked()) {
            result += (result.isEmpty() ? " and " : " and ") + QString("ohf.f_4=1 ");
        }
        if (ui->rbNo4->isChecked()) {
            result += (result.isEmpty() ? " and " : " and ") + QString("ohf.f_4=0 ");
        }
    }
    if (ui->w5->isEnabled()) {
        if (ui->rbYes5->isChecked()) {
            result += (result.isEmpty() ? " and " : " and ") + QString("ohf.f_5=1 ");
        }
        if (ui->rbNo5->isChecked()) {
            result += (result.isEmpty() ? " and " : " and ") + QString("ohf.f_5=0 ");
        }
    }
    return result;
}

void CR5SaleFlags::on_btnCancel_clicked()
{
    reject();
}

void CR5SaleFlags::on_btnOK_clicked()
{
    accept();
}
