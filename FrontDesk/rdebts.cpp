#include "rdebts.h"
#include <QAction>
#include <QDialog>
#include <QJsonObject>
#include <QPointer>
#include <QToolBar>
#include "c5message.h"
#include "c5user.h"
#include "dlgdebtsredeem.h"
#include "ninterface.h"

RDebts::RDebts(const QString &title, QIcon icon, const QString &editorName)
    : RAbstractEditorReport(title, icon, editorName)
{}

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
