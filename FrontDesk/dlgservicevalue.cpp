#include "dlgservicevalue.h"
#include "ui_dlgservicevalue.h"
#include "c5message.h"
#include "c5user.h"
#include "ninterface.h"
#include <QDoubleValidator>

DlgServiceValue::DlgServiceValue(C5User *user, int id, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgServiceValue)
    , mUser(user)
    , mId(id)
{
    ui->setupUi(this);
    ui->leValue->setValidator(new QDoubleValidator(-999.999, 999.999, 3, ui->leValue));
    ui->btnDelete->setVisible(mId > 0);
    setWindowTitle(mId > 0 ? tr("Edit service value") : tr("New service value"));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgServiceValue::trySave);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(ui->btnDelete, &QPushButton::clicked, this, &DlgServiceValue::tryDelete);

    loadData();
}

DlgServiceValue::~DlgServiceValue()
{
    delete ui;
}

void DlgServiceValue::loadData()
{
    if(mId <= 0) {
        ui->leValue->setDouble(0.0);
        ui->leComment->clear();
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/officen/servicevalues/get"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("f_id"), mId}},
                       [this](const QJsonObject &jdoc) {
                           const QJsonObject row = jdoc.value(QStringLiteral("row")).toObject();
                           mId = row.value(QStringLiteral("f_id")).toInt(mId);
                           ui->leValue->setDouble(row.value(QStringLiteral("f_value")).toDouble());
                           ui->leComment->setText(row.value(QStringLiteral("f_comment")).toString());
                       });
}

void DlgServiceValue::trySave()
{
    const double value = ui->leValue->getDouble();
    const QString comment = ui->leComment->text().trimmed();

    NInterface::query(QStringLiteral("/engine/v2/officen/servicevalues/save"),
                      mUser->mSessionKey,
                      this,
                      {
                          {QStringLiteral("f_id"), mId},
                          {QStringLiteral("f_value"), value},
                          {QStringLiteral("f_comment"), comment},
                      },
                      [this](const QJsonObject &jdoc) {
                          mId = jdoc.value(QStringLiteral("f_id")).toInt(mId);
                          accept();
                      },
                      [](const QJsonObject &) { return false; },
                      true);
}

void DlgServiceValue::tryDelete()
{
    if(mId <= 0) {
        return;
    }

    if(C5Message::question(tr("Delete selected service value?")) != QDialog::Accepted) {
        return;
    }

    NInterface::query(QStringLiteral("/engine/v2/officen/servicevalues/remove"),
                      mUser->mSessionKey,
                      this,
                      {{QStringLiteral("f_id"), mId}},
                      [this](const QJsonObject &) { accept(); },
                      [](const QJsonObject &) { return false; },
                      true);
}
