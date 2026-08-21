#include "dlgfindorder.h"
#include "ui_dlgfindorder.h"
#include "c5message.h"
#include "c5user.h"
#include "ninterface.h"

DlgFindOrder::DlgFindOrder(C5User *user, QWidget *parent)
    : C5ShopDialog(user, parent)
    , ui(new Ui::DlgFindOrder)
{
    ui->setupUi(this);
    ui->lePrefix->setFocus();
}

DlgFindOrder::~DlgFindOrder()
{
    delete ui;
}

void DlgFindOrder::on_btnOk_clicked()
{
    findOrder();
}

void DlgFindOrder::on_btnCancel_clicked()
{
    reject();
}

void DlgFindOrder::on_lePrefix_returnPressed()
{
    findOrder();
}

void DlgFindOrder::findOrder()
{
    const QString prefix = ui->lePrefix->text().trimmed();
    if (prefix.isEmpty()) {
        C5Message::error(tr("Enter receipt number"));
        ui->lePrefix->setFocus();
        return;
    }

    ui->btnOk->setEnabled(false);
    NInterface::query(QStringLiteral("/engine/v2/shop/view-order/get-by-prefix"),
                      mUser->mSessionKey,
                      this,
                      {{QStringLiteral("f_prefix"), prefix}},
                      [this](const QJsonObject &jo) {
                          ui->btnOk->setEnabled(true);
                          mOrderId = jo.value(QStringLiteral("id")).toString();
                          if (mOrderId.isEmpty()) {
                              C5Message::error(tr("Document is not exists"));
                              ui->lePrefix->setFocus();
                              ui->lePrefix->selectAll();
                              return;
                          }
                          accept();
                      },
                      [this](const QJsonObject &) {
                          ui->btnOk->setEnabled(true);
                          ui->lePrefix->setFocus();
                          ui->lePrefix->selectAll();
                          return false;
                      },
                      true);
}
