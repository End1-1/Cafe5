#include "dlgfiscalmachine.h"
#include "ui_dlgfiscalmachine.h"
#include "c5message.h"
#include "c5user.h"
#include "ninterface.h"
#include <QJsonObject>

DlgFiscalMachine::DlgFiscalMachine(C5User *user, int id, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgFiscalMachine)
    , mUser(user)
    , mId(id)
{
    ui->setupUi(this);
    ui->btnDelete->setVisible(mId > 0);
    setWindowTitle(mId > 0 ? tr("Edit fiscal machine") : tr("New fiscal machine"));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgFiscalMachine::trySave);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(ui->btnDelete, &QPushButton::clicked, this, &DlgFiscalMachine::tryDelete);

    loadData();
}

DlgFiscalMachine::~DlgFiscalMachine()
{
    delete ui;
}

void DlgFiscalMachine::applyRow(const QJsonObject &row)
{
    mId = row.value(QStringLiteral("f_id")).toInt(mId);
    ui->leName->setText(row.value(QStringLiteral("f_name")).toString());
    ui->leIp->setText(row.value(QStringLiteral("f_ip")).toString());
    ui->sbPort->setValue(row.value(QStringLiteral("f_port")).toInt());
    ui->lePassword->setText(row.value(QStringLiteral("f_password")).toString());
    ui->leOpPin->setText(row.value(QStringLiteral("f_op_pin")).toString());
    ui->leOpPass->setText(row.value(QStringLiteral("f_op_pass")).toString());
    ui->chExternalPos->setChecked(row.value(QStringLiteral("f_external_pos")).toInt() == 1);
    ui->sbDefaultDept->setValue(row.value(QStringLiteral("f_default_dept")).toInt(1));
}

void DlgFiscalMachine::loadData()
{
    if (mId <= 0) {
        applyRow(QJsonObject{
            {QStringLiteral("f_id"), 0},
            {QStringLiteral("f_default_dept"), 1},
        });
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/officen/editors/get-item"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("editor"), QStringLiteral("form_fiscal_machines")},
                        {QStringLiteral("id"), mId}},
                       [this](const QJsonObject &jdoc) {
                           applyRow(jdoc.value(QStringLiteral("row")).toObject());
                       });
}

void DlgFiscalMachine::trySave()
{
    const QString name = ui->leName->text().trimmed();
    if (name.isEmpty()) {
        C5Message::error(tr("Name is required"));
        return;
    }

    NInterface::query(QStringLiteral("/engine/v2/officen/editors/save"),
                      mUser->mSessionKey,
                      this,
                      {
                          {QStringLiteral("editor"), QStringLiteral("form_fiscal_machines")},
                          {QStringLiteral("f_id"), mId},
                          {QStringLiteral("f_name"), name},
                          {QStringLiteral("f_ip"), ui->leIp->text().trimmed()},
                          {QStringLiteral("f_port"), ui->sbPort->value()},
                          {QStringLiteral("f_password"), ui->lePassword->text()},
                          {QStringLiteral("f_op_pin"), ui->leOpPin->text().trimmed()},
                          {QStringLiteral("f_op_pass"), ui->leOpPass->text()},
                          {QStringLiteral("f_external_pos"), ui->chExternalPos->isChecked() ? 1 : 0},
                          {QStringLiteral("f_default_dept"), ui->sbDefaultDept->value()},
                      },
                      [this](const QJsonObject &jdoc) {
                          mId = jdoc.value(QStringLiteral("f_id")).toInt(mId);
                          accept();
                      },
                      [](const QJsonObject &) { return false; },
                      true);
}

void DlgFiscalMachine::tryDelete()
{
    if (mId <= 0) {
        return;
    }

    if (C5Message::question(tr("Delete selected fiscal machine?")) != QDialog::Accepted) {
        return;
    }

    NInterface::query(QStringLiteral("/engine/v2/officen/editors/remove"),
                      mUser->mSessionKey,
                      this,
                      {{QStringLiteral("editor"), QStringLiteral("form_fiscal_machines")},
                       {QStringLiteral("f_id"), mId}},
                      [this](const QJsonObject &) { accept(); },
                      [](const QJsonObject &) { return false; },
                      true);
}
