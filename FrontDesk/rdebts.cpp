#include "rdebts.h"
#include <QAction>
#include <QDialog>
#include <QJsonObject>
#include <QPointer>
#include <QTableView>
#include <QToolBar>
#include "c5message.h"
#include "c5user.h"
#include "dlgdebtsredeem.h"
#include "ninterface.h"
#include "ui_rabstracteditorreport.h"

RDebts::RDebts(const QString &title, QIcon icon, const QString &editorName)
    : RAbstractEditorReport(title, icon, editorName)
{
    // Auto-connect to base slot may skip the override; wire explicitly.
    disconnect(ui->tbl, &QTableView::doubleClicked, this, nullptr);
    connect(ui->tbl, &QTableView::doubleClicked, this, &RDebts::handleDebtsDoubleClick);
}

void RDebts::reloadDebts()
{
    getData();
}

QToolBar *RDebts::toolBar()
{
    RAbstractEditorReport::toolBar();
    if(!mDebtRedeemActionInserted) {
        QAction *const first = fToolBar->actions().isEmpty() ? nullptr : fToolBar->actions().first();
        QAction *const a = new QAction(QIcon(QStringLiteral(":/cash.png")), tr("Redeem debt"), this);
        QObject::connect(a, &QAction::triggered, this, &RDebts::redeemDebt);
        a->setProperty("name", QStringLiteral("redeem_debt"));
        if(first) {
            fToolBar->insertAction(first, a);
        } else {
            fToolBar->addAction(a);
        }
        mDebtRedeemActionInserted = true;
    }
    return fToolBar;
}

void RDebts::on_tbl_doubleClicked(const QModelIndex &index)
{
    handleDebtsDoubleClick(index);
}

void RDebts::handleDebtsDoubleClick(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }
    const QModelIndex srcIndex = reportMapViewIndexToSource(index);
    if(!srcIndex.isValid()) {
        return;
    }

    const QJsonObject vm = filterObject(QStringLiteral("viewmode"));
    const int viewMode = vm.value(QStringLiteral("viewmode")).toInt(1);
    // Detailed modes only: columns Kind / DebtId exist there
    if(viewMode != 2 && viewMode != 4) {
        return;
    }

    const QString kind = reportSourceCellData(srcIndex.row(), 2).toString();
    const int debtId = reportSourceCellData(srcIndex.row(), 1).toInt();
    if(debtId <= 0 || kind == QStringLiteral("opening")) {
        return;
    }
    // Store purchases / sales are not editable as cash redeem; API will also reject.
    if(kind == QStringLiteral("store") || kind == QStringLiteral("order")) {
        return;
    }

    const int docType = (viewMode == 2) ? 1 : 2;
    const int currencyId = filterObject(QStringLiteral("currency")).value(QStringLiteral("currency")).toInt(0);
    if(currencyId <= 0) {
        C5Message::error(tr("Select currency in parameters."));
        return;
    }
    editRedeemDebt(debtId, docType, currencyId);
}

void RDebts::redeemDebt()
{
    const QJsonObject vm = filterObject(QStringLiteral("viewmode"));
    const int viewMode = vm.value(QStringLiteral("viewmode")).toInt(1);
    const int docType = (viewMode == 1 || viewMode == 2) ? 1 : 2;

    const int currencyId = filterObject(QStringLiteral("currency")).value(QStringLiteral("currency")).toInt(0);
    if(currencyId <= 0) {
        C5Message::error(tr("Select currency in parameters."));
        return;
    }

    DlgDebtsRedeem dlg(this, docType, currencyId);
    if(dlg.exec() != QDialog::Accepted) {
        return;
    }

    const int partnerId = dlg.partnerId();
    if(partnerId <= 0) {
        return;
    }

    QPointer<RDebts> self(this);
    QJsonObject params;
    params.insert(QStringLiteral("doc_type"), docType);
    params.insert(QStringLiteral("partner_id"), partnerId);
    params.insert(QStringLiteral("currency_id"), currencyId);
    params.insert(QStringLiteral("amount"), dlg.amount());
    params.insert(QStringLiteral("cashbox_id"), dlg.cashboxId());
    params.insert(QStringLiteral("payment_type_id"), dlg.paymentTypeId());
    params.insert(QStringLiteral("comment"), dlg.comment());
    params.insert(QStringLiteral("date"), dlg.redeemDateMysql());
    NInterface::query1(QStringLiteral("/engine/v2/waiter/cashbox/redeem-debt"),
                       mUser->mSessionKey,
                       this,
                       params,
                       [self](const QJsonObject &) {
                           if(self) {
                               C5Message::info(RDebts::tr("Redemption recorded."));
                               self->reloadDebts();
                           }
                       });
}

void RDebts::editRedeemDebt(int debtId, int docType, int currencyId)
{
    QPointer<RDebts> self(this);
    const QString sessionKey = mUser->mSessionKey;
    QJsonObject loadParams;
    loadParams.insert(QStringLiteral("debt_id"), debtId);
    NInterface::query1(QStringLiteral("/engine/v2/waiter/cashbox/get-redeem-debt"),
                       sessionKey,
                       this,
                       loadParams,
                       [self, docType, currencyId, sessionKey](const QJsonObject &jdoc) {
                           if(!self) {
                               return;
                           }
                           const QJsonObject redeem = jdoc.value(QStringLiteral("redeem")).toObject();
                           if(redeem.isEmpty()) {
                               C5Message::error(RDebts::tr("Not found"));
                               return;
                           }

                           DlgDebtsRedeem dlg(self, docType, currencyId);
                           dlg.loadRedeem(redeem);
                           if(dlg.exec() != QDialog::Accepted) {
                               return;
                           }

                           QJsonObject params;
                           params.insert(QStringLiteral("debt_id"), dlg.debtId());
                           if(dlg.deleteRequested()) {
                               NInterface::query1(QStringLiteral("/engine/v2/waiter/cashbox/delete-redeem-debt"),
                                                  sessionKey,
                                                  self,
                                                  params,
                                                  [self](const QJsonObject &) {
                                                      if(self) {
                                                          C5Message::info(RDebts::tr("Debt payment deleted."));
                                                          self->reloadDebts();
                                                      }
                                                  });
                               return;
                           }

                           params.insert(QStringLiteral("amount"), dlg.amount());
                           params.insert(QStringLiteral("payment_type_id"), dlg.paymentTypeId());
                           params.insert(QStringLiteral("comment"), dlg.comment());
                           params.insert(QStringLiteral("date"), dlg.redeemDateMysql());
                           NInterface::query1(QStringLiteral("/engine/v2/waiter/cashbox/update-redeem-debt"),
                                              sessionKey,
                                              self,
                                              params,
                                              [self](const QJsonObject &) {
                                                  if(self) {
                                                      C5Message::info(RDebts::tr("Debt payment updated."));
                                                      self->reloadDebts();
                                                  }
                                              });
                       });
}
