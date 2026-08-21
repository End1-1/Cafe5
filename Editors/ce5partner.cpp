#include "ce5partner.h"
#include "ui_ce5partner.h"
#include "c5cache.h"
#include "c5editor.h"
#include "c5message.h"
#include <QCompleter>

CE5Partner::CE5Partner(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::CE5Partner)
{
    ui->setupUi(this);

    QCompleter *c = new QCompleter();
    c->setCaseSensitivity(Qt::CaseInsensitive);
    ui->cbGroup->setCompleter(c);
    ui->cbState->setCompleter(c);
    ui->cbCategory->setCompleter(c);
    ui->cbGroup->setDBValues("select f_id, f_name from c_partners_group");
    ui->cbState->setDBValues("select f_id, f_name from c_partners_state");
    ui->cbCategory->setDBValues("select f_id, f_name from c_partners_category");
    ui->cbSaleType->setDBValues("select f_id, f_name from o_sale_type where f_id in (1,2)");
    ui->cbManager->setDBValues("select f_id, concat_ws(' ', f_last, f_first) from s_user order by 2");
}

CE5Partner::~CE5Partner()
{
    delete ui;
}

QString CE5Partner::title()
{
    return tr("Partner");
}

QString CE5Partner::table()
{
    return "c_partners";
}

void CE5Partner::setId(int id)
{
    CE5Editor::setId(id);
}

bool CE5Partner::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    Q_UNUSED(data);
    if (!checkData(err)) {
        return false;
    }

    savePartner(true);
    err = QStringLiteral("json");
    return true;
}

bool CE5Partner::checkData(QString &err)
{
    CE5Editor::checkData(err);
    if (ui->cbCategory->currentData().toInt() == 0) {
        err += tr("Select category") + "\r\n";
    }
    if (ui->cbGroup->currentData().toInt() == 0) {
        err += tr("Select group") + "\r\n";
    }
    if (ui->cbState->currentData().toInt() == 0) {
        err += tr("Select state") + "\r\n";
    }
    return err.isEmpty();
}

bool CE5Partner::isOnline()
{
    return true;
}

bool CE5Partner::canCopy()
{
    return true;
}

void CE5Partner::copyObject()
{
    ui->leCode->clear();
}

QJsonObject CE5Partner::makeSaveJson() const
{
    auto intOrNull = [](int v) -> QJsonValue {
        return v == 0 ? QJsonValue(QJsonValue::Null) : QJsonValue(v);
    };
    auto doubleOrNull = [](double v) -> QJsonValue {
        return qFuzzyIsNull(v) ? QJsonValue(QJsonValue::Null) : QJsonValue(v);
    };

    QJsonObject jo;
    const int id = ui->leCode->getInteger();
    if (id > 0) {
        jo.insert(QStringLiteral("f_id"), id);
    } else {
        jo.insert(QStringLiteral("f_id"), QJsonValue(QJsonValue::Null));
    }
    jo.insert(QStringLiteral("f_category"), ui->cbCategory->currentData().toInt());
    jo.insert(QStringLiteral("f_state"), ui->cbState->currentData().toInt());
    jo.insert(QStringLiteral("f_group"), ui->cbGroup->currentData().toInt());
    jo.insert(QStringLiteral("f_taxcode"), ui->lineEdit_7->text().trimmed());
    jo.insert(QStringLiteral("f_taxname"), ui->lineEdit_2->text().trimmed());
    jo.insert(QStringLiteral("f_name"), ui->lineEdit_9->text().trimmed());
    jo.insert(QStringLiteral("f_contact"), ui->lineEdit_3->text().trimmed());
    jo.insert(QStringLiteral("f_info"), ui->lineEdit_4->text().trimmed());
    jo.insert(QStringLiteral("f_phone"), ui->lineEdit_5->text().trimmed());
    jo.insert(QStringLiteral("f_email"), ui->lineEdit_6->text().trimmed());
    jo.insert(QStringLiteral("f_address"), ui->lineEdit_8->text().trimmed());
    jo.insert(QStringLiteral("f_legal_address"), ui->lineEdit_11->text().trimmed());
    jo.insert(QStringLiteral("f_permanent_discount"), doubleOrNull(ui->lineEdit_10->getDouble()));
    jo.insert(QStringLiteral("f_price_politic"), intOrNull(ui->cbSaleType->currentData().toInt()));
    jo.insert(QStringLiteral("f_manager"), intOrNull(ui->cbManager->currentData().toInt()));
    return jo;
}

QMap<QString, QVariant> CE5Partner::makeResultRow(int id) const
{
    QMap<QString, QVariant> row;
    row.insert(QStringLiteral("f_id"), id);
    row.insert(QStringLiteral("f_category"), ui->cbCategory->currentText());
    row.insert(QStringLiteral("f_state"), ui->cbState->currentText());
    row.insert(QStringLiteral("f_group"), ui->cbGroup->currentText());
    row.insert(QStringLiteral("f_name"), ui->lineEdit_9->text().trimmed());
    row.insert(QStringLiteral("f_taxname"), ui->lineEdit_2->text().trimmed());
    row.insert(QStringLiteral("f_taxcode"), ui->lineEdit_7->text().trimmed());
    row.insert(QStringLiteral("f_contact"), ui->lineEdit_3->text().trimmed());
    row.insert(QStringLiteral("f_info"), ui->lineEdit_4->text().trimmed());
    row.insert(QStringLiteral("f_phone"), ui->lineEdit_5->text().trimmed());
    row.insert(QStringLiteral("f_email"), ui->lineEdit_6->text().trimmed());
    row.insert(QStringLiteral("f_address"), ui->lineEdit_8->text().trimmed());
    row.insert(QStringLiteral("f_legal_address"), ui->lineEdit_11->text().trimmed());
    row.insert(QStringLiteral("f_discount"), ui->lineEdit_10->getDouble());
    row.insert(QStringLiteral("f_saletype"), ui->cbSaleType->currentText());
    row.insert(QStringLiteral("f_manager"), ui->cbManager->currentText());
    return row;
}

void CE5Partner::savePartner(bool closeEditor)
{
    fCloseOnSaveResponse = closeEditor;
    fHttp->createHttpQueryLambda(
        QStringLiteral("/engine/v2/common/partners/save"),
        makeSaveJson(),
        [this](const QJsonObject &jdoc) { saveResponse(jdoc); },
        [](const QJsonObject &) { return false; });
}

void CE5Partner::saveResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
    if (jdoc.value(QStringLiteral("status")).toInt() != 1) {
        C5Message::error(jdoc.value(QStringLiteral("message")).toString());
        return;
    }

    const QJsonObject partner = jdoc.value(QStringLiteral("partner")).toObject();
    const int id = partner.value(QStringLiteral("f_id")).toInt();
    ui->leCode->setInteger(id);
    ui->leCode->setEnabled(false);
    C5Cache::cache(cache_goods_partners)->refresh();

    if (C5Editor *editor = qobject_cast<C5Editor *>(fEditor)) {
        editor->appendResultRow(makeResultRow(id));
    }

    if (fCloseOnSaveResponse) {
        emit Accept();
    }
}

void CE5Partner::on_btnClearManager_clicked()
{
    ui->cbManager->setCurrentIndex(-1);
    if (ui->leCode->getInteger() == 0) {
        return;
    }
    savePartner(false);
}
