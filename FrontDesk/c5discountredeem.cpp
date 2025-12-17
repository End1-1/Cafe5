#include "c5discountredeem.h"
#include "ui_c5discountredeem.h"
#include "c5cache.h"
#include "c5utils.h"
#include "c5message.h"
#include "c5database.h"
#include "ninterface.h"

C5DiscountRedeem::C5DiscountRedeem(const QString &id, int partnerId)
    : C5Dialog(), ui(new Ui::C5DiscountRedeem)
{
    ui->setupUi(this);
    ui->leTransaction->setText(id);
    ui->lePartner->setSelector(ui->lePartnerName, cache_goods_partners);
    ui->lePartner->setCallbackDialog(this);

    if(id.isEmpty() == false) {
        ui->lePartner->setEnabled(false);
    }

    if(partnerId > 0) {
        ui->lePartner->setValue(partnerId);
    }

    if(id.isEmpty() == false) {
        ui->leTransaction->setText(id);
        fHttp->createHttpQueryLambda("/engine/v2/officen/discount-statistic/transaction", {
            {"f_id", ui->leTransaction->text()},

        },
        [this](const QJsonObject & jo) {
            QSignalBlocker(ui->cbCard);
            QSignalBlocker(ui->leAmountToRedeem);
            ui->lePartner->setValue(jo["f_partner"].toInt());
            ui->leDate->setDate(QDate::fromString(jo["f_date"].toString(), FORMAT_DATE_TO_STR_MYSQL));
            ui->cbCard->setCurrentIndex(ui->cbCard->findData(jo["f_card"].toInt()));
            ui->leCurrentBalance->setDouble(jo["f_balance"].toDouble());
            ui->leAmountToRedeem->setDouble(abs(jo["f_amount"].toDouble()));
            ui->btnRedeem->setEnabled(false);
        },
        [this](const QJsonObject je) {
            reject();
        });
    } else {
        ui->btnDeleteRecord->setEnabled(false);
    }
}

C5DiscountRedeem::~C5DiscountRedeem() { delete ui; }

void C5DiscountRedeem::selectorCallback(int selector, const QJsonArray &data)
{
    switch(selector) {
    case cache_goods_partners:
        processPartner(data);
        break;
    }
}

void C5DiscountRedeem::on_btnCancel_clicked()
{
    reject();
}

void C5DiscountRedeem::on_btnRedeem_clicked()
{
    QString err;

    if(ui->lePartner->getInteger() == 0) {
        err += tr("Partner must be selected")  + "<br>";
    }

    if(ui->leAmountToRedeem->getDouble() < 0.1) {
        err += tr("Amount must be greater then zero") + "<br>";
    }

    int cardId = ui->cbCard->currentData().toInt();

    if(cardId < 1) {
        err += tr("Invalid card id") + "<br>";
    }

    if(!err.isEmpty()) {
        C5Message::error(err);
        return;
    }

    if(ui->leTransaction->text().isEmpty()) {
        ui->leTransaction->setText(C5Database::uuid());
    }

    fHttp->createHttpQueryLambda("/engine/v2/officen/discount-statistic/redeem", {
        {"f_id", ui->leTransaction->text()},
        {"partner", ui->lePartner->getInteger()},
        {"f_card",  cardId},
        {"f_type", 9},
        {"f_amount", ui->leAmountToRedeem->getDouble()}
    },
    [this](const QJsonObject & jo) {
        accept();
    },
    [this](const QJsonObject je) {
    });
}

void C5DiscountRedeem::processPartner(const QJsonArray &data)
{
    fHttp->createHttpQueryLambda("/engine/v2/officen/discount-statistic/get-cards", {
        {"partner", ui->lePartner->getInteger()},
    },
    [this](const QJsonObject & jo) {
        QJsonArray ja = jo["data"].toArray();
        QSignalBlocker guard(ui->cbCard);
        ui->cbCard->clear();

        for(int i = 0; i < ja.count(); i++) {
            const QJsonObject jc = ja.at(i).toObject();
            ui->cbCard->addItem(QString("%1 (%2)").arg(jc["f_code"].toString(), float_str(jc["f_amount"].toDouble(), 0)), jc["f_id"].toInt());
            ui->cbCard->setItemData(ui->cbCard->count() - 1, jc["f_amount"].toDouble(), Qt::UserRole + 1);
        }

        ui->cbCard->setCurrentIndex(0);

        if(ui->cbCard->currentIndex() == 0) {
            on_cbCard_currentIndexChanged(0);
        }
    },
    [this](const QJsonObject je) {
    });
}

void C5DiscountRedeem::on_leAmountToRedeem_textChanged(const QString &arg1)
{
    if(str_float(arg1) > ui->leCurrentBalance->getDouble()) {
        ui->leAmountToRedeem->setDouble(ui->leCurrentBalance->getDouble());
    }
}

void C5DiscountRedeem::on_cbCard_currentIndexChanged(int index)
{
    if(index < 0) {
        ui->leCurrentBalance->clear();
        return;
    }

    ui->leCurrentBalance->setDouble(ui->cbCard->itemData(index, Qt::UserRole + 1).toDouble());
}

void C5DiscountRedeem::on_btnDeleteRecord_clicked()
{
    if(C5Message::error(tr("Confirm delete record")) != QDialog::Accepted) {
        return;
    }

    fHttp->createHttpQueryLambda("/engine/v2/officen/discount-statistic/delete-transaction", {
        {"f_id", ui->leTransaction->text()},

    },
    [this](const QJsonObject & jo) {
        accept();
    },
    [this](const QJsonObject je) {
        reject();
    });
}
