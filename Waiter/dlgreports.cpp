#include "dlgreports.h"
#include "ui_dlgreports.h"
#include "dlgreportslist.h"
#include "c5printjson.h"
#include "c5translator.h"
#include "c5user.h"
#include "datadriver.h"
#include "dlgreceiptlanguage.h"
#include "dlglistofhall.h"
#include "printtaxn.h"
#include "c5message.h"
#include "c5printing.h"
#include "c5utils.h"
#include "ndataprovider.h"
#include "dlgcashbuttonopions.h"
#include "c5config.h"
#include "dlgviewcashreport.h"
#include "qinputdialog.h"
#include <QSettings>

DlgReports::DlgReports(const QStringList &dbParams, C5User *user) :
    C5Dialog(dbParams),
    ui(new Ui::DlgReports),
    fUser(user)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowFullScreen);
    ui->date1->setDate(QDate::currentDate());
    ui->date2->setDate(QDate::currentDate());
    fShiftId = 0;
    fCurrentHall = 0;
    setLangIcon();
}

DlgReports::~DlgReports()
{
    delete ui;
}

void DlgReports::openReports(C5User *user)
{
    DlgReports *d = new DlgReports(QStringList(), user);
    d->getDailyCommon();
    d->exec();
    delete d;
}

void DlgReports::getDailyCommon(const QDate &date1, const QDate &date2)
{
    //TODO: CHECK L1
    fHttp->createHttpQueryLambda("/engine/reports/dailycommon.php", {
        {"date1", date1.toString(FORMAT_DATE_TO_STR_MYSQL)},
        {"date2", date2.toString(FORMAT_DATE_TO_STR_MYSQL)},
        {"hall", fCurrentHall},
        {"opened", ui->btnOpened->isChecked()}
    }, [](const QJsonObject &jdoc) {
    }, [](const QJsonObject &je) {
        Q_UNUSED(je);
    });
}

void DlgReports::setLangIcon()
{
    switch (C5Config::getRegValue("receipt_language").toInt()) {
        case 0:
            ui->btnReceiptLanguage->setIcon(QIcon(":/armenia.png"));
            C5Config::setRegValue("receipt_language", LANG_AM);
            break;
        case 1:
            ui->btnReceiptLanguage->setIcon(QIcon(":/usa.png"));
            C5Config::setRegValue("receipt_language", LANG_EN);
            break;
        case 2:
            ui->btnReceiptLanguage->setIcon(QIcon(":/russia.png"));
            C5Config::setRegValue("receipt_language", LANG_RU);
            break;
    }
}

void DlgReports::cashDocResponse(const QJsonObject &jdoc)
{
    int bs = 22;
    QFont font(qApp->font());
    font.setPointSize(bs);
    C5Printing p;
    p.setSceneParams(700, 2600, QPageLayout::Portrait);
    p.setFont(font);
    p.image("./logo_receipt.png", Qt::AlignHCenter);
    p.br();
    p.setFontSize(bs + 2);
    p.setFontBold(true);
    p.ctext(QDate::currentDate().toString(FORMAT_DATE_TO_STR));
    p.br();
    p.setFontSize(bs + 4);
    p.setFontBold(true);
    p.ctext(jdoc["purpose"].toString());
    p.br();
    p.setFontSize(bs);
    p.setFontBold(false);
    p.ctext(float_str(jdoc["cash_in"].toDouble(), 2));
    p.br();
    p.setFontSize(bs - 4);
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.setFontSize(bs - 4);
    p.setFontBold(false);
    p.print(C5Config::localReceiptPrinter(), QPageSize::Custom);
    fHttp->httpQueryFinished(sender());
}

void DlgReports::handleDailyCommon(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() != 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    QJsonArray ja = obj["report"].toArray();
    ui->tbl->clearContents();
    ui->tbl->setRowCount(0);
    double total = 0;
    for (int i = 0; i < ja.count(); i++) {
        QJsonObject o = ja.at(i).toObject();
        int r = ui->tbl->addEmptyRow();
        ui->tbl->setString(r, 0, o["f_id"].toString());
        ui->tbl->setString(r, 1, o["f_order"].toString());
        ui->tbl->setString(r, 2, o["f_datecash"].toString());
        ui->tbl->setString(r, 3, o["f_timeclose"].toString());
        ui->tbl->setString(r, 4, o["f_hall"].toString());
        ui->tbl->setString(r, 5, o["f_table"].toString());
        ui->tbl->setString(r, 6, o["f_staff"].toString());
        ui->tbl->setString(r, 7, float_str(o["f_amounttotal"].toDouble(), 2));
        ui->tbl->setString(r, 8, o["f_receiptnumber"].toString());
        total += o["f_amounttotal"].toDouble();
    }
    ui->tblTotal->setColumnCount(ui->tbl->columnCount());
    ui->tbl->resizeColumnsToContents();
    ui->tbl->setColumnWidth(0, 0);
    for (int i = 0; i < ui->tbl->columnCount(); i++) {
        ui->tblTotal->setColumnWidth(i, ui->tbl->columnWidth(i));
    }
    ui->tblTotal->setString(0, 7, float_str(total, 2));
    QStringList l;
    l.append(QString::number(ui->tbl->rowCount()));
    ui->tblTotal->setVerticalHeaderLabels(l);
}

void DlgReports::reportListResponse(const QJsonObject &obj)
{
    fHttp->httpQueryFinished(sender());
    QString file;
    if (!DlgReportsList::getReport(obj["list"].toArray(), file)) {
        return;
    }
    fHttp->createHttpQuery(QString("/engine/waiter/reports/%1.php").arg(file),
    QJsonObject{{"date1", ui->date1->date().toString(FORMAT_DATE_TO_STR_MYSQL)},
        {"date2", ui->date2->date().toString(FORMAT_DATE_TO_STR_MYSQL)}},
    SLOT(printReportResponse(QJsonObject)));
}

void DlgReports::printReportResponse(const QJsonObject &obj)
{
    if (obj["reply"].toInt() != 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    QJsonArray ja = obj["report"].toArray();
    ja.append(QJsonObject{{"cmd", "print"}, {"printer", __c5config.localReceiptPrinter()}, {"pagesize", QPageSize::Custom}});
    C5PrintJson *pj = new C5PrintJson(ja);
    pj->start();
    fHttp->httpQueryFinished(sender());
}

void DlgReports::handleReceipt(const QJsonObject &obj)
{
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
    }
}

void DlgReports::handleTaxback(const QJsonObject &obj)
{
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    C5Message::info(obj["msg"].toString());
}

void DlgReports::handleTaxReport(const QJsonObject &obj)
{
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    } else {
        C5Message::info(tr("Printed"));
    }
}

void DlgReports::on_btnRefresh_clicked()
{
    getDailyCommon(ui->date1->date(), ui->date2->date());
}

void DlgReports::on_btnDateLeft1_clicked()
{
    ui->date1->setDate(ui->date1->date().addDays(-1));
}

void DlgReports::on_btnDateLeft2_clicked()
{
    ui->date2->setDate(ui->date2->date().addDays(-1));
}

void DlgReports::on_btnDateRight1_clicked()
{
    ui->date1->setDate(ui->date1->date().addDays(1));
}

void DlgReports::on_btnDateRight2_clicked()
{
    ui->date2->setDate(ui->date2->date().addDays(1));
}

void DlgReports::on_btnExit_clicked()
{
    accept();
}

void DlgReports::on_btnReports_clicked()
{
    fHttp->createHttpQuery("/engine/waiter/reports/list.php", QJsonObject(), SLOT(reportListResponse(QJsonObject)));
}

void DlgReports::on_btnPrintOrderReceipt_clicked()
{
    QModelIndexList l = ui->tbl->selectionModel()->selectedRows();
    if (l.count() == 0) {
        return;
    }
    QString orderid = ui->tbl->getString(l.at(0).row(), 0);
    C5Database db(__c5config.dbParams());
    db[":f_id"] = orderid;
    db.exec("select * from o_header where f_id=:f_id");
    if (!db.nextRow()) {
        C5Message::error(tr("Invalid order id"));
        return;
    }
    int tax = 0;
    if (db.getDouble("f_amounttotal") - db.getDouble("f_amountcash") < 0.001) {
        db[":f_id"] = orderid;
        db.exec("select f_receiptnumber from o_tax where f_id=:f_id");
        if (db.nextRow()) {
            if (db.getInt("f_receiptnumber") == 0) {
                tax = C5Message::question(tr("Do you want to print fiscal receipt?")) == QDialog::Accepted ? 1 : 0;
            }
        } else {
            tax = C5Message::question(tr("Do you want to print fiscal receipt?")) == QDialog::Accepted ? 1 : 0;
        }
    } else if (db.getDouble("f_amountcard") > 0.001) {
        tax = 1;
    }
    //TODO: CHECK L3
    fHttp->createHttpQueryLambda("/engine/waiter/printreceipt.php", {
        {"staffid", fUser->id()},
        {"f_receiptlanguage", C5Config::getRegValue("receipt_language").toInt()},
        {"order", orderid},
        {"printtax", tax},
        {"receipt_printer", C5Config::fSettingsName}
    }, [this](const QJsonObject &jo) {
        auto np = new NDataProvider(this);
        np->overwriteHost("http", "127.0.0.1", 8080);
        connect(np, &NDataProvider::done, this, [](const QJsonObject &jjo) {
        });
        connect(np, &NDataProvider::error, this, [](const QString &err) {
        });
        np->getData("/printjson", jo);
    }, [](const QJsonObject &je) {
        Q_UNUSED(je);
    });
}

void DlgReports::on_btnReceiptLanguage_clicked()
{
    int r = DlgReceiptLanguage::receipLanguage();
    if (r > -1) {
        C5Config::setRegValue("receipt_language", r);
        setLangIcon();
    }
}

void DlgReports::on_btnHall_clicked()
{
    int hall;
    if (!DlgListOfHall::getHall(hall)) {
        return;
    }
    fCurrentHall = hall;
    ui->btnHall->setText(fCurrentHall == 0 ? tr("All") : dbhall->name(fCurrentHall));
}

void DlgReports::on_btnReturnTaxReceipt_clicked()
{
    QModelIndexList l = ui->tbl->selectionModel()->selectedRows();
    if (l.count() == 0) {
        return;
    }
    //TODO: CHECK L7
    auto np = new NDataProvider(this);
    np->overwriteHost("http", "127.0.0.1", 8080);
    connect(np, &NDataProvider::done, this, [](const QJsonObject &jo) {
    });
    connect(np, &NDataProvider::error, this, [](const QString &err) {
    });
    np->getData("/taxreturn", {
        {"order",  ui->tbl->getString(l.at(0).row(), 0)}
    });
}

void DlgReports::on_btnPrintTaxX_clicked()
{
    //TODO: CHECK L6
    auto np = new NDataProvider(this);
    np->overwriteHost("http", "127.0.0.1", 8080);
    connect(np, &NDataProvider::done, this, [](const QJsonObject &jo) {
    });
    connect(np, &NDataProvider::error, this, [](const QString &err) {
    });
    np->getData("/taxreport", {
        {"d1", ui->date1->date().toString(FORMAT_DATE_TO_STR_MYSQL)},
        {"d2", ui->date2->date().toString(FORMAT_DATE_TO_STR_MYSQL)},
        {"type", report_x}
    });
}

void DlgReports::on_btnPrintTaxZ_clicked()
{
    //TODO: CHECK L5
    auto np = new NDataProvider(this);
    np->overwriteHost("http", "127.0.0.1", 8080);
    connect(np, &NDataProvider::done, this, [](const QJsonObject &jo) {
    });
    connect(np, &NDataProvider::error, this, [](const QString &err) {
    });
    np->getData("/taxreport", {
        {"d1", ui->date1->date().toString(FORMAT_DATE_TO_STR_MYSQL)},
        {"d2", ui->date2->date().toString(FORMAT_DATE_TO_STR_MYSQL)},
        {"type", report_z}
    });
}

void DlgReports::on_btnCashobx_clicked()
{
    bool closecash = false;
    bool printreport = false;
    switch (DlgCashButtonOpions::getOptions()) {
        case 0:
            return;
        case 1:
            break;
        case 2:
            printreport = true;
            break;
        case 3:
            printreport = true;
            closecash = true;
            break;
    }
    C5Database db(__c5config.dbParams());
    db.exec("SELECT 'Մուտք', 'title', 0, '' "
            "UNION "
            "SELECT en.f_name AS f_cashname, ah.f_comment, sum(e.f_amount), '' "
            "FROM e_cash e "
            "LEFT JOIN e_cash_names en ON en.f_id=e.f_cash "
            "LEFT join a_header ah ON ah.f_id=e.f_header "
            "WHERE e.f_sign=1 and ah.f_sessionid IS NULL "
            "GROUP BY 1, 2 "
            "UNION "
            "SELECT 'Ելք', 'title', 0, '' "
            "union "
            "SELECT en.f_name AS f_cashname, concat_ws(' ',ah.f_comment, p.f_name) as f_comment,  sum(e.f_amount*-1), "
            "e.f_id "
            "FROM e_cash e "
            "LEFT JOIN e_cash_names en ON en.f_id=e.f_cash "
            "LEFT join a_header ah ON ah.f_id=e.f_header "
            "left join c_partners p on p.f_id=ah.f_partner "
            "WHERE e.f_sign=-1 and ah.f_sessionid IS NULL "
            "GROUP BY 1, 2, 4 "
            "UNION "
            "SELECT 'վերջնահաշվարկ', 'title', 0, '' "
            "union "
            "SELECT en.f_name AS f_cashname, '',  sum(e.f_amount*e.f_sign), '' "
            "FROM e_cash e "
            "LEFT JOIN e_cash_names en ON en.f_id=e.f_cash "
            "LEFT join a_header ah ON ah.f_id=e.f_header "
            "WHERE ah.f_sessionid IS NULL "
            "GROUP BY 1");
    if (!printreport) {
        DlgViewCashReport d;
        double total = -1000000000;
        while (db.nextRow()) {
            if (db.getString(1) == "title") {
                if (total > -1000000000) {
                    d.addTotal(total);
                }
                total = 0;
                d.addTitle(db.getString(0));
                continue;
            }
            d.addRow(db.getString(0) + " " + db.getString(1), db.getDouble(2));
            total += db.getDouble(2);
        }
        if (total > -1000000000) {
            d.addTotal(total);
        }
        if (d.exec() == QDialog::Accepted) {
            printreport = true;
            db.first();
        } else {
            return;
        }
    }
    if (printreport) {
        int bs = 22;
        QFont font(qApp->font());
        font.setPointSize(bs);
        C5Printing p;
        p.setSceneParams(650, 2600, QPageLayout::Portrait);
        p.setFont(font);
        p.image("./logo_receipt.png", Qt::AlignHCenter);
        p.br();
        p.setFontSize(bs + 2);
        p.setFontBold(true);
        p.ctext(QDate::currentDate().toString(FORMAT_DATE_TO_STR));
        p.br();
        if (closecash) {
            p.setFontSize(bs + 4);
            p.setFontBold(true);
            p.ctext(tr("Cash closing"));
            p.br();
        }
        double total = -1000000000;
        while (db.nextRow()) {
            if (db.getString(1) == "title") {
                if (total > -1000000000) {
                    p.setFontSize(bs + 2);
                    p.setFontBold(true);
                    p.ltext(tr("Total"), 0, 490);
                    p.rtext(float_str(total, 2));
                    p.br();
                    p.line();
                    p.br();
                    p.line();
                    p.br();
                    p.line();
                    p.br(2);
                    p.br(2);
                }
                total = 0;
                p.setFontSize(bs + 2);
                p.setFontBold(true);
                p.ctext(db.getString(0));
                p.br();
                p.line();
                p.br();
                continue;
            }
            p.setFontSize(bs);
            p.setFontBold(false);
            total += db.getDouble(2);
            p.ltext(db.getString(0) + " " + db.getString(1), 0, 490);
            p.rtext(float_str(db.getDouble(2), 2));
            p.br();
        }
        if (total > -1000000000) {
            p.setFontSize(bs + 2);
            p.setFontBold(true);
            p.ltext(tr("Total"), 0, 490);
            p.rtext(float_str(total, 2));
            p.br();
            p.line();
            p.br();
            p.line();
            p.br();
            p.line();
            p.br(2);
            p.br(2);
        }
        p.setFontSize(bs - 4);
        p.ltext(tr("Printed"), 0);
        p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
        p.br();
        p.print(C5Config::localReceiptPrinter(), QPageSize::Custom);
    }
    if (closecash) {
        db[":f_sessionid"] = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss");
        db.exec("update a_header set f_sessionid=:f_sessionid where f_sessionid is null");
    }
}

void DlgReports::on_btnCashInput_clicked()
{
    bool ok;
    double cash = QInputDialog::getDouble(this, tr("Cash input"), "", 0, 0, 999999999, 0, &ok);
    if (!ok || cash < 0.001) {
        return;
    }
    QJsonObject jo;
    jo["cash_in"] = __c5config.cashId();
    jo["cash_out"] = 0;
    jo["purpose"] = tr("Change input");
    jo["date"] = QDate::currentDate().toString(FORMAT_DATE_TO_STR_MYSQL);
    jo["amount_in"] = cash;
    jo["amount_out"] = 0;
    fHttp->createHttpQuery("/engine/worker/cashdoc.php", jo, SLOT(cashDocResponse(QJsonObject)));
}
