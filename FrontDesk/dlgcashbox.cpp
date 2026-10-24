#include "dlgcashbox.h"
#include "ui_dlgcashbox.h"
#include "c5codenameselectorfunctions.h"
#include "c5message.h"
#include "c5user.h"
#include "ninterface.h"
#include <QJsonObject>

DlgCashbox::DlgCashbox(C5User *user, int id, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgCashbox)
    , mUser(user)
    , mId(id)
{
    ui->setupUi(this);
    ui->wCurrency->selectorCallback = currencyItemSelector;
    ui->btnDelete->setVisible(mId > 0);
    setWindowTitle(mId > 0 ? tr("Edit cashbox") : tr("New cashbox"));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgCashbox::trySave);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(ui->btnDelete, &QPushButton::clicked, this, &DlgCashbox::tryDelete);

    loadData();
}

DlgCashbox::~DlgCashbox()
{
    delete ui;
}

void DlgCashbox::applyRow(const QJsonObject &row)
{
    mId = row.value(QStringLiteral("f_id")).toInt(mId);
    ui->leName->setText(row.value(QStringLiteral("f_name")).toString());
    ui->wCurrency->setCodeAndName(row.value(QStringLiteral("f_currency_id")).toInt(1),
                                  row.value(QStringLiteral("f_currency_name")).toString());
}

void DlgCashbox::loadData()
{
    NInterface::query1(QStringLiteral("/engine/v2/officen/editors/get-item"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("editor"), QStringLiteral("form_cashboxes")},
                        {QStringLiteral("id"), mId}},
                       [this](const QJsonObject &jdoc) {
                           applyRow(jdoc.value(QStringLiteral("row")).toObject());
                       });
}

void DlgCashbox::trySave()
{
    const QString name = ui->leName->text().trimmed();
    if (name.isEmpty()) {
        C5Message::error(tr("Name is required"));
        return;
    }

    const int currencyId = ui->wCurrency->value();
    if (currencyId <= 0) {
        C5Message::error(tr("Currency is required"));
        return;
    }

    NInterface::query(QStringLiteral("/engine/v2/officen/editors/save"),
                      mUser->mSessionKey,
                      this,
                      {
                          {QStringLiteral("editor"), QStringLiteral("form_cashboxes")},
                          {QStringLiteral("f_id"), mId},
                          {QStringLiteral("f_name"), name},
                          {QStringLiteral("f_currency_id"), currencyId},
                      },
                      [this](const QJsonObject &jdoc) {
                          mId = jdoc.value(QStringLiteral("f_id")).toInt(mId);
                          accept();
                      },
                      [](const QJsonObject &) { return false; },
                      true);
}

void DlgCashbox::tryDelete()
{
    if (mId <= 0) {
        return;
    }

    if (C5Message::question(tr("Delete selected cashbox?")) != QDialog::Accepted) {
        return;
    }

    NInterface::query(QStringLiteral("/engine/v2/officen/editors/remove"),
                      mUser->mSessionKey,
                      this,
                      {{QStringLiteral("editor"), QStringLiteral("form_cashboxes")},
                       {QStringLiteral("f_id"), mId}},
                      [this](const QJsonObject &) { accept(); },
                      [](const QJsonObject &) { return false; },
                      true);
}
