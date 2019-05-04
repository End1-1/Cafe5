#include "payment.h"
#include "ui_payment.h"
#include "printtaxn.h"
#include "c5printing.h"
#include "c5database.h"
#include "dish.h"

payment::payment(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::payment)
{
    ui->setupUi(this);
    ui->btnTax->setChecked(__c5config.getRegValue("taxprint").toBool());
    QFont f(qApp->font());
    f.setPointSize(f.pointSize() + 3);
    setFont(f);
}

payment::~payment()
{
    delete ui;
}

void payment::setAmount(double amount)
{
    QStringList cash;
    cash << "500"
         << "1000"
         << "2000"
         << "5000"
         << "10000"
         << "20000";
    ui->leAmount->setDouble(amount);
    ui->leCash->setDouble(amount);
    ui->leChange->setDouble(0);
}

double payment::cardAmount()
{
    return fCardAmount;
}

double payment::cashAmount()
{
    return ui->leAmount->getDouble() - fCardAmount;
}

void payment::setOrderTable(QTableWidget *t)
{
    fOrderTable = t;
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
    fOrderUUID = C5Database::uuid();
    if (ui->btnTax->isChecked()) {
        if (!printTax(cash ? 0 : ui->leAmount->getDouble())) {
            return;
        }
    }
    if (cash) {
        fCardAmount = ui->leAmount->getDouble();
    }
    if (printReceipt()) {

    }
}

bool payment::printReceipt()
{
    C5Database db;
    QFont font(qApp->font());
    font.setPointSize(20);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);

    p.image("./logo_receipt.png", Qt::AlignHCenter);
    p.br();
    p.setFontBold(true);
    p.ctext(tr("Receipt #") + QString("%1%2").arg(fHeader["f_prefix"].toString()).arg(fHeader["f_hallid"].toString()));
    p.br();
    if (fHeader["f_otherid"].toString().toInt() == PAYOTHER_SELFCOST) {
        p.setFontSize(24);
        p.setFontBold(true);
        p.ctext(__translator.tt(tr("Breakfast")));
        p.br();
        p.br(4);
        p.setFontSize(20);
        p.setFontBold(false);
    }
    p.setFontBold(false);db[":f_id"] = fOrderUUID;
    db.exec("select * from o_tax where f_id=:f_id");
    if (db.nextRow()) {
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
        p.rtext(fHeader["f_fiscal"].toString());
        p.br();
        p.ltext(__translator.tt(tr("Receipt number")), 0);
        p.rtext(fHeader["f_receiptnumber"].toString());
        p.br();
        p.ltext(__translator.tt(tr("Date")), 0);
        p.rtext(fHeader["f_taxtime"].toString());
        p.br();
        p.ltext(__translator.tt(tr("(F)")), 0);
        p.br();
    }
    p.br(1);
    p.ltext(__translator.tt(tr("Table")), 0);
    p.rtext(QString("%1/%2").arg(fHeader["f_hallname"].toString()).arg(fHeader["f_tablename"].toString()));
    p.br();
    p.ltext(__translator.tt(tr("Printed")), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.ltext(__translator.tt(tr("Staff")), 0);
    p.rtext(fHeader["f_currentstaffname"].toString());
    p.br();
    p.br(2);
    p.line();
    p.br(2);
    p.ctext(__translator.tt(tr("Class | Name | Qty | Price | Total")));
    p.br();
    p.br(4);
    p.line();
    p.br(1);
    int nn = 1;
    double total = 0;
    for (int i = 0; i < fBody.count(); i++) {
        QJsonObject o = fBody.at(i).toObject();
        if (o["f_state"].toString().toInt() != DISH_STATE_OK) {
            continue;
        }
        total += o["f_qty2"].toString().toDouble() * o["f_price"].toString().toDouble();
        p.ltext(QString("%1. %2: %3, %4").arg(nn++).arg(__translator.tt(tr("Class"))).arg(o["f_adgcode"].toString()).arg(o["f_name"].toString()), 0);
//        p.br();
//        p.ltext(o["f_name"].toString(), 0);
        p.br();
        QString servPlus;
        QString servValue = float_str(fHeader["f_servicefactor"].toString().toDouble() * 100, 2) + "% ";
        if (fHeader["f_servicemode"].toString().toInt() == SERVICE_AMOUNT_MODE_INCREASE_PRICE) {
            if (fHeader["f_servicefactor"].toString().toDouble() > 0.001) {
                servPlus = "+";
            } else {
                servValue = "";
            }
            p.ltext(QString("%1 x %2 %3 %4 = %5")
                    .arg(float_str(o["f_qty2"].toString().toDouble(), 2))
                    .arg(float_str(o["f_price"].toString().toDouble(), 2))
                    .arg(servPlus).arg(servValue)
                    .arg(float_str(o["f_total"].toString().toDouble(), 2)), 0);
        } else {
            p.ltext(QString("%1 x %2 = %5")
                    .arg(float_str(o["f_qty2"].toString().toDouble(), 2))
                    .arg(float_str(o["f_price"].toString().toDouble(), 2))
                    .arg(float_str(o["f_total"].toString().toDouble(), 2)), 0);
        }
        p.br();
        p.br(2);
        p.line();
        p.br(1);
    }
    p.br();
    switch (fHeader["f_servicemode"].toString().toInt()) {
    case SERVICE_AMOUNT_MODE_INCREASE_PRICE:
        break;
    case SERVICE_AMOUNT_MODE_SEPARATE:
        if (fHeader["f_servicefactor"].toString().toDouble() > 0.0001) {
            p.br(2);
            p.line();
            p.ltext(QString("%1").arg(__translator.tt(tr("Total"))), 0);
            p.rtext(float_str(total, 2));
            p.br();
            p.ltext(QString("%1 %2%").arg(__translator.tt(tr("Service"))).arg(float_str(fHeader["f_servicefactor"].toString().toDouble() * 100, 2)), 0);
            p.rtext(float_str(fHeader["f_amountservice"].toString().toDouble(), 2));
            p.br();
            p.br(2);
            p.line();
            p.br(1);
        }
        break;
    }
    p.ltext(__translator.tt(tr("Grand total")), 0);
    p.rtext(float_str(fHeader["f_amounttotal"].toString().toDouble(), 2));
    p.br();
    if (fHeader["f_servicemode"].toString().toInt() == SERVICE_AMOUNT_MODE_INCREASE_PRICE && fHeader["f_servicefactor"].toString().toDouble() > 0.001) {
        p.ltext(QString("%1 %2%").arg(__translator.tt(tr("Service included"))).arg(float_str(fHeader["f_servicefactor"].toString().toDouble() * 100, 2)), 0);
        p.rtext(float_str(fHeader["f_amountservice"].toString().toDouble(), 2));
        p.br();
    }
    if (fHeader["f_discountfactor"].toString().toDouble() > 0.01) {
        p.ltext(QString("%1 %2%").arg(__translator.tt(tr("Discount included"))).arg(float_str(fHeader["f_discountfactor"].toString().toDouble() * 100, 2)), 0);
        p.rtext(float_str(fHeader["f_amountdiscount"].toString().toDouble(), 2));
        p.br();
    }
    p.setFontBold(true);
    p.ltext(__translator.tt(tr("Need to pay")), 0);
    p.rtext(float_str(fHeader["f_amounttotal"].toString().toDouble(), 2));
    p.br();
    p.br();

    p.line();
    p.br();
    if (fHeader["f_idramid"].toString().length() > 0) {
        p.setFontBold(false);
        p.ctext(__translator.tt(tr("Pay by IDRAM")));
        p.br();
        int levelIndex = 1;
        int versionIndex = 0;
        bool bExtent = true;
        int maskIndex = -1;
        QString encodeString = QString("%1;%2;%3;%4|%5;%6;")
                .arg("Jazzve")
                .arg(fHeader["f_idramid"].toString()) //IDram ID
                .arg(fHeader["f_amounttotal"].toString())
                .arg(QString("%1-%2").arg(fHeader["f_prefix"].toString()).arg(fHeader["f_id"].toString()))
                .arg(fHeader["f_idramphone"].toString())
                .arg("1");
        CQR_Encode qrEncode;
        bool successfulEncoding = qrEncode.EncodeData( levelIndex, versionIndex, bExtent, maskIndex, encodeString.toUtf8().data() );
        if (successfulEncoding) {
            int qrImageSize = qrEncode.m_nSymbleSize;
            int encodeImageSize = qrImageSize + ( QR_MARGIN * 2 );
            QImage encodeImage(encodeImageSize, encodeImageSize, QImage::Format_Mono);
            encodeImage.fill(1);

            for ( int i = 0; i < qrImageSize; i++ )
                for ( int j = 0; j < qrImageSize; j++ )
                    if ( qrEncode.m_byModuleData[i][j] )
                        encodeImage.setPixel( i + QR_MARGIN, j + QR_MARGIN, 0 );

            QPixmap pix = QPixmap::fromImage( encodeImage );
            pix = pix.scaled(300, 300);
            p.image(pix, Qt::AlignHCenter);
            p.br();
        }
    }

    if (!fBill) {
        p.br();
        if (fHeader["f_amountcash"].toString().toDouble() > 0.001) {
            p.ltext(__translator.tt(tr("Payment, cash")), 0);
            p.rtext(float_str(fHeader["f_amountcash"].toString().toDouble(), 2));
        }
        if (fHeader["f_amountcard"].toString().toDouble() > 0.001) {
            p.ltext(__translator.tt(tr("Payment, card")), 0);
            p.rtext(float_str(fHeader["f_amountcard"].toString().toDouble(), 2));
        }
        if (fHeader["f_amountbank"].toString().toDouble() > 0.001) {
            p.ltext(__translator.tt(tr("Bank transfer")), 0);
            p.rtext(float_str(fHeader["f_amountbank"].toString().toDouble(), 2));
        }
        p.br();
    }

    p.setFontSize(24);
    if (!fHeader["f_other_room"].toString().isEmpty()) {
        p.br();
        p.ctext(__translator.tt(tr("Transfer to room")));
        p.br();
        p.ctext(fHeader["f_other_room"].toString() + ", " + fHeader["f_other_guest"].toString());
        p.br(p.fLineHeight * 5);
        p.line(3);
        p.ctext(__translator.tt(tr("Signature")));
        p.br(p.fLineHeight * 2);
    }

    if (!fHeader["f_other_clcode"].toString().isEmpty()) {
        p.br();
        p.ctext(__translator.tt(tr("City ledger")));
        p.br();
        p.ctext(fHeader["f_other_clcode"].toString() + ", " + fHeader["f_other_clname"].toString());
        p.br(p.fLineHeight * 3);
    }

    if (fHeader["f_otherid"].toString().toInt() == PAYOTHER_COMPLIMENTARY) {
        p.br();
        p.ctext(__translator.tt(tr("Complimentary")) + " " + fHeader["f_comment"].toString());
        p.br(p.fLineHeight * 5);
        p.line(3);
        p.ctext(__translator.tt(tr("Signature")));
        p.br(p.fLineHeight * 2);
    }

    p.setFontSize(20);
    p.setFontBold(true);
    p.br(p.fLineHeight * 3);
    p.ltext(__translator.tt(tr("Thank you for visit!")), 0);
    p.br();
    p.ltext(QString("%1: %2").arg(tr("Sample")).arg(fHeader["f_print"].toString()), 0);
    p.br();
    p.print(fPrinter, QPrinter::Custom);
}

bool payment::printTax(double cardAmount)
{
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(), this);
    for (int i = 0; i < fOrderTable->rowCount(); i++) {
        Dish d = fOrderTable->item(i, 0)->data(Qt::UserRole).value<Dish>();
        pt.addGoods(C5Config::taxDept(), d.adgCode, QString::number(d.id), d.name, d.price, d.qty);
    }
    QString jsonIn, jsonOut, err;
    int result = 0;
    result = pt.makeJsonAndPrint(cardAmount, 0, jsonIn, jsonOut, err);
    C5Database db;
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
    } else {
        C5Message::error(err);
    }
    return result;
}
