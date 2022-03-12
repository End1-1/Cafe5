#include "payment.h"
#include "ui_payment.h"
#include "printtaxn.h"
#include "c5printing.h"
#include "c5storedraftwriter.h"
#include "c5database.h"
#include "c5user.h"
#include "dish.h"
#include <QFile>
#include <QPrinter>
#include <QShortcut>
#include <QNetworkReply>
#include <QNetworkAccessManager>

payment::payment(const QString order, const QStringList &dbParams, C5User *user) :
    C5Dialog(dbParams),
    ui(new Ui::payment)
{
    ui->setupUi(this);
    fUser = user;
    fCanAccept = false;
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

    adjustSize();
    new QShortcut(QKeySequence("F2"), this, SLOT(focusChangeLineEdit()));
    new QShortcut(QKeySequence("F9"), this, SLOT(keyF9()));
    new QShortcut(QKeySequence("F10"), this, SLOT(keyF10()));
    if (__c5config.getValue(param_tax_print_always_offer).toInt() != 0) {
        ui->btnTax->setEnabled(false);
        ui->btnTax->setChecked(true);
        ui->btnCancel->setVisible(false);
    }
    ui->btnCheckoutOther->setEnabled(fUser->check(cp_t5_pay_complimentary));
}

payment::~payment()
{
    delete ui;
}

void payment::justPrint()
{
    on_btnCheckoutCash_clicked();
}

void payment::accept()
{
    if (fCanAccept) {
        C5Dialog::accept();
    }
}

void payment::reject()
{
    accept();
}

void payment::keyF9()
{
    ui->btnTax->setChecked(false);
    on_btnCheckoutCash_clicked();
}

void payment::keyF10()
{
    ui->btnTax->setChecked(true);
    qApp->processEvents();
    on_btnCheckoutCash_clicked();
}

void payment::on_btnTax_clicked(bool checked)
{
    __c5config.setRegValue("taxprint", checked);
}

void payment::on_btnCancel_clicked()
{
    fCanAccept = true;
    reject();
}

void payment::on_leCash_textChanged(const QString &arg1)
{
    ui->leChange->setDouble(ui->leAmount->getDouble() - arg1.toDouble());
}

void payment::on_btnCheckoutCash_clicked()
{
    checkout(true, false);
}

void payment::on_btnCheckoutCard_clicked()
{
    checkout(false, false);
}

void payment::checkout(bool cash, bool idram)
{
    if (ui->btnTax->isChecked()) {
        if (!printTax(cash ? 0 : ui->leAmount->getDouble(), idram ? true : (C5Config::taxUseExtPos() == "true" ? true : false))) {
            return;
        }
    }
    C5Database db(fDBParams);
    db[":f_id"] = fOrderUUID;
    db.exec("select * from o_header where f_id=:f_id");
    if (!db.nextRow()) {
        C5Message::error(db.fLastError);
        return;
    }
    QString headerNum = db.getString("f_prefix") + db.getString("f_hallid");
    int cashid = cash ? __c5config.cashId() : __c5config.nocashId();
    QString cashprefix = cash ? __c5config.cashPrefix() : __c5config.nocashPrefix();
    if (cash) {

    } else if (idram) {
        db[":f_amountcash"] = 0;
        db[":f_amountidram"] = ui->leAmount->getDouble();
        db.update("o_header", "f_id", fOrderUUID);
    } else {
        db[":f_id"] = fOrderUUID;
        db.exec("update o_header set f_amountcash=0, f_amountcard=f_amounttotal where f_id=:f_id");
    }
    C5StoreDraftWriter dw(db);
    int counter = dw.counterAType(DOC_TYPE_CASH);
    if (counter == 0) {
        C5Message::error(dw.fErrorMsg);
        return;
    }
    QString cashdoc;
    if (!dw.writeAHeader(cashdoc, QString::number(counter), DOC_STATE_SAVED, DOC_TYPE_CASH,
                         fUser->id(), QDate::currentDate(), QDate::currentDate(), QTime::currentTime(),
                         0, ui->leAmount->getDouble(), cashprefix + " " + headerNum)) {
        C5Message::error(dw.fErrorMsg);
        return;
    }
    if (!dw.writeAHeaderCash(cashdoc, cashid, 0, 1, "", fOrderUUID)) {
        C5Message::error(dw.fErrorMsg);
        return;
    }
    QString cashUUID;
    if (!dw.writeECash(cashUUID, cashdoc, cashid, 1, cashprefix + " " + headerNum, ui->leAmount->getDouble(), cashUUID, 1)) {
        C5Message::error(dw.fErrorMsg);
        return;
    }
    if (printReceipt(true)) {
        fCanAccept = true;
        accept();
    }
}

bool payment::printReceipt(bool printSecond)
{
    C5Database db(fDBParams);
    QFont font(qApp->font());
    font.setPointSize(26);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);
    QMap<int, QMap<QString, QVariant> > packages;

    if (QFile::exists("./logo_receipt.png")) {
        p.image("./logo_receipt.png", Qt::AlignHCenter);
        p.br();
    }
    db[":f_header"] = fOrderUUID;
    db.exec("select p.f_id, d.f_name, p.f_qty, p.f_price "
            "from o_package p "
            "inner join d_package d on d.f_id=p.f_package "
            "where p.f_header=:f_header");
    while (db.nextRow()) {
        QMap<QString, QVariant> p;
        for (int i = 0; i < db.columnCount(); i++) {
            p[db.columnName(i)] = db.getValue(i);
        }
        packages[db.getInt("f_id")] = p;
    }
    db[":f_id"] = fOrderUUID;
    db.exec("select o.f_prefix, o.f_hallid, t.f_firmname, t.f_address, t.f_dept, t.f_hvhh, t.f_devnum, "
            "t.f_serial, t.f_fiscal, t.f_receiptnumber, t.f_time as f_taxtime, concat(left(u.f_first, 1), '. ', u.f_last) as f_staff, "
            "o.f_amountcash, o.f_amountcard, o.f_amounttotal, o.f_print, o.f_comment, ht.f_name as f_tablename, "
            "p.f_contact, p.f_phone, p.f_address "
            "from o_header o "
            "left join o_tax t on t.f_id=o.f_id "
            "left join s_user u on u.f_id=o.f_staff "
            "left join h_tables ht on ht.f_id=o.f_table "
            "left join c_partners p on p.f_id=o.f_partner "
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
    p.ltext(tr("Cashier"), 0);
    p.rtext(db.getString("f_staff"));
    p.br();
    p.ltext(tr("Delivery"), 0);
    p.rtext(db.getString("f_tablename"));
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
    dd.exec("select b.f_adgcode, b.f_dish, d.f_name, b.f_price, b.f_qty1, b.f_package "
            "from o_body b "
            "left join d_dish d on d.f_id=b.f_dish "
            "where b.f_header=:f_header and b.f_state=:f_state "
            "order by b.f_package ");
    int package = 0;
    while (dd.nextRow()) {
        if (dd.getInt("f_package") > 0) {
            if (package != dd.getInt("f_package")) {
                if (package > 0) {
                    p.br();
                    p.line();
                    p.br(2);
                    package = 0;
                }
                p.setFontBold(true);
                p.setFontSize(28);
                package = dd.getInt("f_package");
                p.ltext(packages[package]["f_name"].toString(), 0);
                p.br();
                p.ltext(QString("%1 x %2 = %3")
                        .arg(float_str(packages[package]["f_qty"].toDouble(), 2))
                        .arg(packages[package]["f_price"].toDouble(), 2)
                        .arg(packages[package]["f_qty"].toDouble() * packages[package]["f_price"].toDouble()), 0);
                p.br();
                p.setFontBold(false);
                p.setFontSize(26);
            }
            p.ltext(QString("*** %1 %2, %3 x%4").arg(tr("Class:")).arg(dd.getString("f_adgcode")).arg(dd.getString("f_name")).arg(float_str(dd.getDouble("f_qty1"), 2)), 0);
            p.br();
        } else {
            if (package > 0) {
                p.br();
                p.line();
                p.br(2);
                package = 0;
            }
            if (dd.getString(0).isEmpty()) {
                p.ltext(QString("%3").arg(dd.getString("f_name")), 0);
            } else {
                p.ltext(QString("%1 %2, %3").arg(tr("Class:")).arg(dd.getString("f_adgcode")).arg(dd.getString("f_name")), 0);
            }
            p.br();
            p.ltext(QString("%1 x %2 = %3").arg(float_str(dd.getDouble("f_qty1"), 2)).arg(dd.getDouble("f_price"), 2).arg(float_str(dd.getDouble("f_qty1") * dd.getDouble("f_price"), 2)), 0);
            p.br();
            p.line();
            p.br(2);
        }
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

    if (db.getString("f_phone").length() > 0) {
        p.br();
        p.br();
        p.ltext(tr("Customer"), 0);
        p.br();
        p.ltext(db.getString("f_phone"), 0);
        p.br();
        p.ltext(db.getString("f_address"), 0);
        p.br();
        p.ltext(db.getString("f_contact"), 0);
        p.br();
    }

    p.setFontSize(20);
    p.setFontBold(true);
    p.br(p.fLineHeight * 3);
    p.ltext(__c5config.getValue(param_recipe_footer_text), 0);
    p.br();
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.ltext(QString("%1: %2").arg(tr("Sample")).arg(db.getInt("f_print")), 0);
    p.br();
    p.br();
    p.br();
    p.br();
    p.br();
    p.br();
    p.br();
    p.ltext(".", 0);
    p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);

    db[":f_print"] = db.getInt("f_print") + 1;
    db.update("o_header", "f_id", fOrderUUID);


    qDebug() << printSecond << __c5config.getValue(param_shop_print_v2);
    if (printSecond && __c5config.getValue(param_shop_print_v2) == "1") {
        printReceipt(false);
    }
    return true;
}

bool payment::printTax(double cardAmount, bool useextpos)
{
    C5Database db(fDBParams);
    db[":f_header"] = fOrderUUID;
    db[":f_state"] = DISH_STATE_OK;
    db.exec("select b.f_adgcode, b.f_dish, d.f_name, b.f_price, b.f_qty1 "
            "from o_body b "
            "left join d_dish d on d.f_id=b.f_dish "
            "where b.f_header=:f_header and b.f_state=:f_state");
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), useextpos ? "true" : "false", C5Config::taxCashier(), C5Config::taxPin(), this);
    while (db.nextRow()) {
        pt.addGoods(C5Config::taxDept(), db.getString("f_adgcode"), db.getString("f_dish"), db.getString("f_name"), db.getDouble("f_price"), db.getDouble("f_qty1"), 0.0);
    }
    QString jsonIn, jsonOut, err;
    int result = 0;
    result = pt.makeJsonAndPrint(cardAmount, 0, jsonIn, jsonOut, err);
    db[":f_id"] = db.uuid();
    db[":f_order"] = fOrderUUID;
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_in"] = jsonIn;
    db[":f_out"] = jsonOut;
    db[":f_err"] = err;
    db[":f_result"] = result;
    db.insert("o_tax_log", false);
    if (__c5config.getValue(param_debuge_mode).toInt() == 1) {
        QSqlQuery q(db.fDb);
        pt.saveTimeResult(fOrderUUID, q);
    }
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

void payment::on_tblChange_cellClicked(int row, int column)
{
    QTableWidgetItem *item = ui->tblChange->item(row, column);
    if (!item) {
        return;
    }
    double v = item->text().toDouble();
    ui->leChange->setDouble(v - ui->leCash->getDouble());
}

void payment::on_btnCheckoutOther_clicked()
{
    C5Database db(fDBParams);
    db[":f_id"] = fOrderUUID;
    db.exec("select * from o_header where f_id=:f_id");
    if (!db.nextRow()) {
        C5Message::error(db.fLastError);
        return;
    }

    db[":f_id"] = fOrderUUID;
    db.exec("update o_header set f_amountother=f_amounttotal where f_id=:f_id");

    C5StoreDraftWriter dw(db);
    int counter = dw.counterAType(DOC_TYPE_CASH);
    if (counter == 0) {
        C5Message::error(dw.fErrorMsg);
        return;
    }

    if (printReceipt(true)) {
        fCanAccept = true;
        accept();
    }
}

void payment::on_btnCheckoutIdram_clicked()
{
    QNetworkAccessManager *na = new QNetworkAccessManager(this);
    connect(na, &QNetworkAccessManager::finished, this, [=](QNetworkReply *r) {
        if (r->error() != QNetworkReply::NoError) {
            C5Message::error(r->errorString());
            return;
        }
        QByteArray ba = r->readAll();
        qDebug() << ba;
#ifdef QT_DEBUG
        ba = QString("{\"Result\":[{\"TRANSACTION_ID\":\"22021100127207\",\"TR_DATE\":\"11/02/2022 10:21\",\"SRC_STATUS\":\"0\",\"COR_ACCOUNT_ID\":\"356943140\",\"REF_NUM\":\"\",\"DEBIT\":\"%1\",\"CREDIT\":\"0.00\",\"COMM\":\"0.00\",\"OPERATION_NAME\":\"\",\"COR_ACCOUNT_NAME\":\"\",\"EXT_TR_ID\":null}],\"OpCode\":0,\"OpDesc\":\"Err:EN-0\"}")
                .arg("1,500.00").toUtf8();
#endif
        QJsonDocument jdoc = QJsonDocument::fromJson(ba);
        QJsonObject jo = jdoc.object();
        qDebug() << jo;
        QJsonArray ja = jo["Result"].toArray();
        if (ja.count() > 0) {
            QJsonObject jr = ja.at(0).toObject();
            QString DEBIT = jr["DEBIT"].toString();
            DEBIT.replace(",", " ");
            if (str_float(DEBIT) > 0.01) {
                checkout(false, true);
            } else {
                C5Message::info(ba);
            }
        } else {
            C5Message::info(tr("Unpaid"));
        }
        sender()->deleteLater();
    });
    connect(na, &QNetworkAccessManager::sslErrors, this, [=](QNetworkReply *reply, const QList<QSslError> &error) {
       Q_UNUSED(error);
       C5Message::error(reply->errorString());
    });
    QNetworkRequest nr = QNetworkRequest(QUrl("https://money.idram.am/api/History/Search"));
    nr.setRawHeader("_SessionId_", __c5config.getValue(param_idram_session_id).toLatin1()); //"3497ae22-8623-45be-84d9-f9f9671b0628");
    nr.setRawHeader("_EncMethod_", "NONE");
    nr.setRawHeader("Content-Type", "application/json");
    //nr->setRawHeader("Content-Length", QString::number(m_data.length()).toLatin1());
    nr.setRawHeader("Cache-Control", "no-cache");
    nr.setRawHeader("Accept", "*/*");
    QString request = QString("{\"Detail\":\"%1\"}").arg(fOrderUUID);
    //QString request = QString("{\"Detail\":\"%1\"}").arg("229eb2c3-083f-4bf5-b410-8ced2b6450ce");
    na->post(nr, request.toLatin1());
}
