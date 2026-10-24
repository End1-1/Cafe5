#include "dlgapikey.h"
#include "ui_dlgapikey.h"
#include "c5user.h"
#include "ninterface.h"
#include <QJsonObject>

DlgApiKey::DlgApiKey(C5User *user, const QString &partner, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgApiKey)
    , mUser(user)
    , mPartner(partner)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgApiKey::trySave);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    loadData();
}

DlgApiKey::~DlgApiKey()
{
    delete ui;
}

void DlgApiKey::applyRow(const QJsonObject &row)
{
    mPartner = row.value(QStringLiteral("f_partner")).toString();
    ui->leCode->setText(mPartner);
    ui->leName->setText(row.value(QStringLiteral("f_title")).toString());
    ui->leToken->setText(row.value(QStringLiteral("f_token")).toString());
    ui->cbEnabled->setChecked(row.value(QStringLiteral("f_enabled")).toVariant().toInt() != 0);
    ui->lblHint->setText(row.value(QStringLiteral("f_hint")).toString());
    const bool hasKey = row.value(QStringLiteral("f_has_key")).toVariant().toInt() != 0;
    ui->leToken->setPlaceholderText(hasKey ? tr("Leave empty to keep the current key") : QString());
}

void DlgApiKey::loadData()
{
    NInterface::query1(QStringLiteral("/engine/v2/officen/editors/get-item"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("editor"), QStringLiteral("form_api_keys")},
                        {QStringLiteral("partner"), mPartner}},
                       [this](const QJsonObject &jdoc) {
                           applyRow(jdoc.value(QStringLiteral("row")).toObject());
                       });
}

void DlgApiKey::trySave()
{
    NInterface::query(QStringLiteral("/engine/v2/officen/editors/save"),
                      mUser->mSessionKey,
                      this,
                      {
                          {QStringLiteral("editor"), QStringLiteral("form_api_keys")},
                          {QStringLiteral("partner"), mPartner},
                          {QStringLiteral("f_token"), ui->leToken->text().trimmed()},
                          {QStringLiteral("f_enabled"), ui->cbEnabled->isChecked()},
                      },
                      [this](const QJsonObject &) { accept(); },
                      [](const QJsonObject &) { return false; },
                      true);
}
