#include "payment.h"
#include "ui_payment.h"
#include "printtaxn.h"
#include "c5printing.h"
#include "c5database.h"
#include "dish.h"
#include <QFile>
#include <QPrinter>

payment::payment(const QString order, const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::payment)
{
    ui->setupUi(this);
    fOrderUUID = order;
    ui->btnTax->setChecked(__c5config.getRegValue("taxprint").toBool());
    QFont f(qApp->font());
    f.setPointSize(f.pointSize() + 3);
    setFont(f);
    C5Database db(dbParams);
    db[":f_id"] = fOrderUUID;
    db.exec("select f_amounttotal, f_amountcash, f_amountcard from o_header where f_id=:f_id");
    if (db.nextRow()) {
        ui->leAmount->setDouble(db.getDouble("f_amounttotal"));
        ui->leCash->setDouble(db.getDouble("f_amountcash"));
        ui->leCard->setDouble(db.getDouble("f_amountcard"));
        ui->leChange->setDouble(0);
    }
    QStringList cash;
    cash << "500"
         << "1000"
         << "2000"
         << "5000"
         << "10000"
         << "15000"
         << "20000";
    int r = 0, c = 0;
    for (int i = 0; i < cash.count(); i++) {
        ui->tblChange->setItem(r, c, new QTableWidgetItem(cash.at(i)));
        c++;
        if (c == ui->tblChange->columnCount()) {
            c = 0;
            r++;
        }
        if (r > 1) {
            return;
        }
    }
    ui->tblChange->setVisible(false);
    adjustSize();
}

payment::~payment()
{
    delete ui;
}

void payment::on_btnTax_clicked(bool checked)
{
    __c5config.setRegValue("taxprint", checked);
}

void payment::on_btnCancel_clicked()
{
    reject();
}

void payment::on_leCash_textChanged(const QString &arg1)
{
    ui->leChange->setDouble(ui->leAmount->getDouble() - arg1.toDouble());
}

void payment::on_btnCheckoutCash_clicked()
{
    checkout(true);
}

void payment::on_btnCheckoutCard_clicked()
{
    checkout(false);
}

void payment::checkout(bool cash)
{
    if (ui->btnTax->isChecked()) {
        if (!printTax(cash ? 0 : ui->leAmount->getDouble())) {
            return;
        }
    }
    if (printReceipt()) {
        accept();
    }
}

bool payment::printReceipt()
{
    C5Database db(fDBParams);
    QFont font(qApp->font());
    font.setPointSize(20);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);

    if (QFile::exists("./logo_receipt.png")) {
        p.image("./logo_receipt.png", Qt::AlignHCenter);
        p.br();
    }
    db[":f_id"] = fOrderUUID;
    db.exec("select o.f_prefix, o.f_hallid, t.f_firmname, t.f_address, t.f_dept, t.f_hvhh, t.f_devnum, "
            "t.f_serial, t.f_fiscal, t.f_receiptnumber, t.f_time as f_taxtime, concat(left(u.f_first, 1), '. ', u.f_last) as f_staff, "
            "o.f_amountcash, o.f_amountcard, o.f_amounttotal "
            "from o_header o "
            "left join o_tax t on t.f_id=o.f_id "
            "left join s_user u on u.f_id=o.f_staff "
            "where o.f_id=:f_id");
    if (!db.nextRow()) {
        C5Message::error(tr("Invalid order number"));
        return false;
    }
    p.setFontBold(true);
    p.ctext(tr("Receipt #") + QString("%1%2").arg(db.getString("f_prefix")).arg(db.getString("f_hallid")));
    p.br();    
    p.setFontBold(false);
    if (db.getString("f_receiptnumber").length() > 0) {
        p.ltext(db.getString("f_firmname"), 0);
        p.br();
        p.ltext(db.getString("f_address"), 0);
        p.br();
        p.ltext(tr("Department"), 0);
        p.rtext(db.getString("f_dept"));
        p.br();
        p.ltext(tr("Tax number"), 0);
        p.rtext(db.getString("f_hvhh"));
        p.br();
        p.ltext(tr("Device number"), 0);
        p.rtext(db.getString("f_devnum"));
        p.br();
        p.ltext(tr("Serial"), 0);
        p.rtext(db.getString("f_serial"));
        p.br();
        p.ltext(tr("Fiscal"), 0);
        p.rtext(db.getString("f_fiscal"));
        p.br();
        p.ltext(tr("Receipt number"), 0);
        p.rtext(db.getString("f_receiptnumber"));
        p.br();
        p.ltext(tr("Date"), 0);
        p.rtext(db.getString("f_taxtime"));
        p.br();
        p.ltext(tr("(F)"), 0);
        p.br();
    }
    p.ltext(tr("Staff"), 0);
    p.rtext(db.getString("f_staff"));
    p.br();
    p.line(3);
    p.br(2);
    p.setFontBold(true);
    p.ctext(tr("Class | Name | Qty | Price | Total"));
    p.setFontBold(false);
    p.br();
    p.line(3);
    p.br(3);

    C5Database dd(fDBParams);
    dd[":f_header"] = fOrderUUID;
    dd[":f_state"] = DISH_STATE_OK;
    dd.exec("select b.f_adgcode, b.f_dish, d.f_name, b.f_price, b.f_qty1 "
            "from o_body b "
            "left join d_dish d on d.f_id=b.f_dish "
            "where b.f_header=:f_header and b.f_state=:f_state");
    while (dd.nextRow()) {
        p.ltext(QString("%1 %2, %3").arg(tr("Class:")).arg(dd.getString("f_adgcode")).arg(dd.getString("f_name")), 0);
        p.br();
        p.ltext(QString("%1 X %2 = %3").arg(float_str(dd.getDouble("f_qty1"), 2)).arg(dd.getDouble("f_price"), 2).arg(float_str(dd.getDouble("f_qty1") * dd.getDouble("f_price"), 2)), 0);
        p.br();
        p.line();
        p.br(2);
    }
    p.line(4);
    p.br(3);
    p.setFontBold(true);
    p.ltext(tr("Need to pay"), 0);
    p.rtext(float_str(db.getDouble("f_amounttotal"), 2));
    p.br();
    p.br();

    p.line();
    p.br();

    if (db.getDouble("f_amountcash") > 0.001) {
        p.ltext(tr("Payment, cash"), 0);
        p.rtext(float_str(db.getDouble("f_amountcash"), 2));
    }
    if (db.getDouble("f_amountcard") > 0.001) {
        p.ltext(tr("Payment, card"), 0);
        p.rtext(float_str(db.getDouble("f_amountcard"), 2));
    }

    p.setFontSize(20);
    p.setFontBold(true);
    p.br(p.fLineHeight * 3);
    p.ltext(tr("Thank you for visit!"), 0);
    p.br();
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.ltext(QString("%1: %2").arg(tr("Sample")).arg(db.getInt("f_print")), 0);
    p.br();
    p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);
    return true;
}

bool payment::printTax(double cardAmount)
{
    C5Database db(fDBParams);
    db[":f_header"] = fOrderUUID;
    db[":f_state"] = DISH_STATE_OK;
    db.exec("select b.f_adgcode, b.f_dish, d.f_name, b.f_price, b.f_qty1 "
            "from o_body b "
            "left join d_dish d on d.f_id=b.f_dish "
            "where b.f_header=:f_header and b.f_state=:f_state");
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(), this);
    while (db.nextRow()) {
        pt.addGoods(C5Config::taxDept(), db.getString("f_adgcode"), db.getString("f_dish"), db.getString("f_name"), db.getDouble("f_price"), db.getDouble("f_qty1"), 0.0);
    }
    QString jsonIn, jsonOut, err;
    int result = 0;
    result = pt.makeJsonAndPrint(cardAmount, 0, jsonIn, jsonOut, err);
    db[":f_order"] = fOrderUUID;
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_in"] = jsonIn;
    db[":f_out"] = jsonOut;
    db[":f_err"] = err;
    db[":f_result"] = result;
    db.insert("o_tax_log", false);
    if (result == pt_err_ok) {
        QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
        PrintTaxN::parseResponse(jsonOut, firm, hvhh, fiscal, rseq, sn, address, devnum, time);
        db[":f_id"] = fOrderUUID;
        db.exec("delete from o_tax where f_id=:f_id");
        db[":f_id"] = fOrderUUID;
        db[":f_dept"] = C5Config::taxDept();
        db[":f_firmname"] = firm;
        db[":f_address"] = address;
        db[":f_devnum"] = devnum;
        db[":f_serial"] = sn;
        db[":f_fiscal"] = fiscal;
        db[":f_receiptnumber"] = rseq;
        db[":f_hvhh"] = hvhh;
        db[":f_fiscalmode"] = tr("(F)");
        db[":f_time"] = time;
        db.insert("o_tax", false);
        ui->leTaxNumber->setText(rseq);
        return true;
    }
    C5Message::error(err);
    return false;
}
