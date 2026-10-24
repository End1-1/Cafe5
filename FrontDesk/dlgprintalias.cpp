#include "dlgprintalias.h"
#include "ui_dlgprintalias.h"
#include "c5message.h"
#include "c5user.h"
#include "ninterface.h"
#include <QJsonArray>
#include <QJsonObject>

DlgPrintAlias::DlgPrintAlias(C5User *user, int id, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgPrintAlias)
    , mUser(user)
    , mId(id)
{
    ui->setupUi(this);
    ui->btnDelete->setVisible(mId > 0);
    setWindowTitle(mId > 0 ? tr("Edit print alias") : tr("New print alias"));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgPrintAlias::trySave);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(ui->btnDelete, &QPushButton::clicked, this, &DlgPrintAlias::tryDelete);

    loadData();
}

DlgPrintAlias::~DlgPrintAlias()
{
    delete ui;
}

void DlgPrintAlias::fillPrinters(const QJsonArray &printers)
{
    const QString current = ui->cbPrinter->currentText().trimmed();
    ui->cbPrinter->clear();
    for (const QJsonValue &v : printers) {
        const QString name = v.toString().trimmed();
        if (!name.isEmpty()) {
            ui->cbPrinter->addItem(name);
        }
    }
    if (!current.isEmpty()) {
        const int idx = ui->cbPrinter->findText(current);
        if (idx >= 0) {
            ui->cbPrinter->setCurrentIndex(idx);
        } else {
            ui->cbPrinter->setEditText(current);
        }
    }
}

void DlgPrintAlias::applyRow(const QJsonObject &row)
{
    mId = row.value(QStringLiteral("f_id")).toInt(mId);
    ui->leAlias->setText(row.value(QStringLiteral("f_alias")).toString());
    const QString printer = row.value(QStringLiteral("f_printer")).toString().trimmed();
    const int idx = ui->cbPrinter->findText(printer);
    if (idx >= 0) {
        ui->cbPrinter->setCurrentIndex(idx);
    } else {
        ui->cbPrinter->setEditText(printer);
    }
}

void DlgPrintAlias::loadData()
{
    NInterface::query1(QStringLiteral("/engine/v2/officen/editors/get-item"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("editor"), QStringLiteral("form_print_aliases")},
                        {QStringLiteral("id"), mId}},
                       [this](const QJsonObject &jdoc) {
                           fillPrinters(jdoc.value(QStringLiteral("printers")).toArray());
                           applyRow(jdoc.value(QStringLiteral("row")).toObject());
                       });
}

void DlgPrintAlias::trySave()
{
    const QString alias = ui->leAlias->text().trimmed();
    const QString printer = ui->cbPrinter->currentText().trimmed();
    if (alias.isEmpty()) {
        C5Message::error(tr("Alias is required"));
        return;
    }
    if (printer.isEmpty()) {
        C5Message::error(tr("Printer is required"));
        return;
    }

    NInterface::query(QStringLiteral("/engine/v2/officen/editors/save"),
                      mUser->mSessionKey,
                      this,
                      {
                          {QStringLiteral("editor"), QStringLiteral("form_print_aliases")},
                          {QStringLiteral("f_id"), mId},
                          {QStringLiteral("f_alias"), alias},
                          {QStringLiteral("f_printer"), printer},
                      },
                      [this](const QJsonObject &jdoc) {
                          mId = jdoc.value(QStringLiteral("f_id")).toInt(mId);
                          accept();
                      },
                      [](const QJsonObject &) { return false; },
                      true);
}

void DlgPrintAlias::tryDelete()
{
    if (mId <= 0) {
        return;
    }

    if (C5Message::question(tr("Delete selected print alias?")) != QDialog::Accepted) {
        return;
    }

    NInterface::query(QStringLiteral("/engine/v2/officen/editors/remove"),
                      mUser->mSessionKey,
                      this,
                      {{QStringLiteral("editor"), QStringLiteral("form_print_aliases")},
                       {QStringLiteral("f_id"), mId}},
                      [this](const QJsonObject &) { accept(); },
                      [](const QJsonObject &) { return false; },
                      true);
}
