#include "dlgcl.h"
#include "ui_dlgcl.h"
#include "c5user.h"
#include "ninterface.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QPointer>

DlgCL::DlgCL(C5User *user) :
    C5Dialog(user),
    ui(new Ui::DlgCL)
{
    ui->setupUi(this);
    setWindowTitle(tr("Debt partner"));
    connect(ui->kbd, SIGNAL(textChanged(QString)), this, SLOT(searchCL(QString)));
    connect(ui->kbd, SIGNAL(accept()), this, SLOT(kbdAccept()));
    connect(ui->kbd, SIGNAL(reject()), this, SLOT(reject()));
    ui->tblGuest->configColumns({120, 400, 180});
    loadPartners(QString());
}

DlgCL::~DlgCL()
{
    delete ui;
}

bool DlgCL::getCL(C5User *user, QString &id, QString &name)
{
    DlgCL d(user);
    if (d.exec() != QDialog::Accepted) {
        return false;
    }
    id = d.ui->tblGuest->getString(d.fRow, 0);
    name = d.ui->tblGuest->getString(d.fRow, 1);
    return !id.isEmpty();
}

void DlgCL::loadPartners(const QString &query)
{
    if (!mUser) {
        return;
    }
    QPointer<DlgCL> self(this);
    NInterface::query1(QStringLiteral("/engine/v2/waiter/customer/debt-holders"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("query"), query}},
                       [self](const QJsonObject &jo) {
                           if (!self) {
                               return;
                           }
                           self->fillTable(jo.value(QStringLiteral("partners")).toArray());
                       });
}

void DlgCL::fillTable(const QJsonArray &partners)
{
    ui->tblGuest->setRowCount(0);
    for (const QJsonValue &v : partners) {
        const QJsonObject o = v.toObject();
        const int row = ui->tblGuest->addEmptyRow();
        ui->tblGuest->setString(row, 0, QString::number(o.value(QStringLiteral("f_id")).toInt()));
        ui->tblGuest->setString(row, 1, o.value(QStringLiteral("f_name")).toString());
        ui->tblGuest->setString(row, 2, o.value(QStringLiteral("f_phone")).toString());
    }
}

void DlgCL::kbdAccept()
{
    QModelIndexList ml = ui->tblGuest->selectionModel()->selectedRows();
    if (ml.count() == 0) {
        return;
    }
    fRow = ml.at(0).row();
    accept();
}

void DlgCL::searchCL(const QString &arg)
{
    // Local filter is enough for already loaded list; re-query when typing.
    if (arg.trimmed().size() <= 1) {
        loadPartners(QString());
        return;
    }
    loadPartners(arg.trimmed());
}

void DlgCL::on_tblGuest_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    kbdAccept();
}
