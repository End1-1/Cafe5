#include "dlgprinter.h"
#include "ui_dlgprinter.h"
#include "c5message.h"
#include "c5user.h"
#include "ninterface.h"
#include <QJsonObject>

DlgPrinter::DlgPrinter(C5User *user, int id, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgPrinter)
    , mUser(user)
    , mId(id)
{
    ui->setupUi(this);
    ui->btnDelete->setVisible(mId > 0);
    setWindowTitle(mId > 0 ? tr("Edit printer") : tr("New printer"));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgPrinter::trySave);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(ui->btnDelete, &QPushButton::clicked, this, &DlgPrinter::tryDelete);

    loadData();
}

DlgPrinter::~DlgPrinter()
{
    delete ui;
}

void DlgPrinter::applyRow(const QJsonObject &row)
{
    mId = row.value(QStringLiteral("f_id")).toInt(mId);
    ui->leName->setText(row.value(QStringLiteral("f_name")).toString());
}

void DlgPrinter::loadData()
{
    NInterface::query1(QStringLiteral("/engine/v2/officen/editors/get-item"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("editor"), QStringLiteral("form_printers")},
                        {QStringLiteral("id"), mId}},
                       [this](const QJsonObject &jdoc) {
                           applyRow(jdoc.value(QStringLiteral("row")).toObject());
                       });
}

void DlgPrinter::trySave()
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
                          {QStringLiteral("editor"), QStringLiteral("form_printers")},
                          {QStringLiteral("f_id"), mId},
                          {QStringLiteral("f_name"), name},
                      },
                      [this](const QJsonObject &jdoc) {
                          mId = jdoc.value(QStringLiteral("f_id")).toInt(mId);
                          accept();
                      },
                      [](const QJsonObject &) { return false; },
                      true);
}

void DlgPrinter::tryDelete()
{
    if (mId <= 0) {
        return;
    }

    if (C5Message::question(tr("Delete selected printer?")) != QDialog::Accepted) {
        return;
    }

    NInterface::query(QStringLiteral("/engine/v2/officen/editors/remove"),
                      mUser->mSessionKey,
                      this,
                      {{QStringLiteral("editor"), QStringLiteral("form_printers")},
                       {QStringLiteral("f_id"), mId}},
                      [this](const QJsonObject &) { accept(); },
                      [](const QJsonObject &) { return false; },
                      true);
}
