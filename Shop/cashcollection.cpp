#include "cashcollection.h"
#include "ui_cashcollection.h"
#include "c5storedraftwriter.h"
#include "c5config.h"
#include "c5utils.h"
#include "c5message.h"
#include "c5user.h"

CashCollection::CashCollection() :
    C5Dialog(),
    ui(new Ui::CashCollection)
{
    ui->setupUi(this);
    fMax = 0;
    fCoinCashId = 0;
    ui->lePurpose->setText(tr("Cash collection") + " " + QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    fHttp->createHttpQueryLambda("/engine/users/index.php", {{"class", "elinadaily"}, {"method", "get"},
        {"config", C5Config::fMainJson["id"].toInt()}
    },
    [this](const QJsonObject & jo) {
        fMax = jo["cash"].toDouble();
        ui->leAmount->setDouble(fMax);
        ui->leAmountCard->setDouble(jo["card"].toDouble());
        ui->leAmountPrepaid->setDouble(jo["prepaid"].toDouble());
        QJsonObject joo = jo["daily"].toObject();
        ui->leRInit->setDouble(joo["f_prevday"].toDouble());
        ui->leRInput->setDouble(joo["f_income"].toDouble());
        ui->leInputOther->setDouble(joo["f_inputother"].toDouble());
        ui->leRSale->setDouble(joo["f_sale"].toDouble());
        ui->leROut->setDouble(joo["f_output"].toDouble());
        ui->leRDiscount->setDouble(joo["f_discount"].toDouble());
        ui->leRFinal->setDouble(joo["f_final"].toDouble());
        ui->leRCheck->setDouble(joo["f_check"].toDouble());
    }, [](const QJsonObject & je) {
    });
    fCoinCashId = __c5config.fMainJson["coincash_id"].toInt();
    ui->lbAmountCoin->setVisible(fCoinCashId > 0);
    ui->leAmountCoin->setVisible(fCoinCashId > 0);
    adjustSize();
}

CashCollection::~CashCollection()
{
    delete ui;
}

void CashCollection::on_btnCancel_clicked()
{
    reject();
}

void CashCollection::on_btnSave_clicked()
{
    // if(ui->leAmount->getDouble() < 0.001) {
    //     C5Message::error(tr("Amount must be greater then 0"));
    //     return;
    // }
    if(ui->leAmountCoin->getDouble() > 0.001) {
        fHttp->createHttpQuery("/engine/cashdesk/create.php",
        QJsonObject {
            {"date", QDate::currentDate().toString(FORMAT_DATE_TO_STR_MYSQL)},
            {"operator", __user->id() },
            {"cashin", 0},
            {"cashout", __c5config.cashId()},
            {"remarks", tr("Amount coin")},
            {"amount", ui->leAmountCoin->text()},
            {"config", C5Config::fMainJson["id"].toInt()},
            {"daily_check", ui->leRCheck->getDouble()}
        }, SLOT(responseOfCreate(QJsonObject)));
    } else {
        collectCash();
    }
}

void CashCollection::on_leAmount_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);

    if(ui->leAmount->getDouble() > fMax) {
        ui->leAmount->setDouble(fMax);
    }
}

void CashCollection::responseOfCreate(const QJsonObject &jdoc)
{
    Q_UNUSED(jdoc);
    fHttp->httpQueryFinished(sender());
    collectCash();
    fHttp->createHttpQuery("/engine/cashdesk/create.php",
    QJsonObject {
        {"date", QDate::currentDate().toString(FORMAT_DATE_TO_STR_MYSQL)},
        {"operator", __user->id() },
        {"cashin", fCoinCashId},
        {"cashout", 0},
        {"remarks", tr("Amount coin")},
        {"amount", ui->leAmountCoin->text()}
    }, SLOT(responseOfCreateIn(QJsonObject)));
}

void CashCollection::responseOfCreateIn(const QJsonObject &jdoc)
{
    Q_UNUSED(jdoc);
    fHttp->httpQueryFinished(sender());
}

void CashCollection::collectCash()
{
    C5Database db;
    C5StoreDraftWriter dw(db);
    int counter = dw.counterAType(DOC_TYPE_CASH);

    if(counter == 0) {
        C5Message::error(dw.fErrorMsg);
        return;
    }

    QString headerPrefix;
    int headerId;

    if(!dw.hallId(headerPrefix, headerId, __c5config.defaultHall())) {
        C5Message::error(dw.fErrorMsg);
        return;
    }

    QString cashdocid;

    if(!dw.writeAHeader(cashdocid, QString::number(counter), DOC_STATE_SAVED, DOC_TYPE_CASH, __user->id(),
                        QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0,
                        ui->leAmount->getDouble() - ui->leAmountCoin->getDouble(),
                        ui->lePurpose->text(), 1, __c5config.getValue(param_default_currency).toInt())) {
        C5Message::error(dw.fErrorMsg);
        return;
    }

    if(!dw.writeAHeaderCash(cashdocid, 0, __c5config.cashId(), 0, "", "")) {
        C5Message::error(dw.fErrorMsg);
        return;
    }

    QString cashUUID;

    if(!dw.writeECash(cashUUID, cashdocid, __c5config.cashId(), -1, ui->lePurpose->text(),
                      ui->leAmount->getDouble() - ui->leAmountCoin->getDouble(),
                      cashUUID, 1)) {
        C5Message::error(dw.fErrorMsg);
        return;
    }

    C5Message::info(tr("Saved"));
    accept();
}
