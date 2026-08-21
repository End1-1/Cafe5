#include "rcashmovement.h"
#include <QDialog>
#include <QJsonObject>
#include <QToolBar>
#include "c5cashtransfer.h"
#include "c5config.h"
#include "c5mainwindow.h"
#include "c5revenuecashop.h"
#include "c5user.h"
#include "ninterface.h"

RCashMovement::RCashMovement(const QString &title, QIcon icon, const QString &editorName)
    : RAbstractEditorReport(title, icon, editorName)
{}

QToolBar* RCashMovement::toolBar()
{
    const bool firstBuild = (fToolBar == nullptr);
    QToolBar *tb = RAbstractEditorReport::toolBar();

    if(firstBuild && tb) {
        tb->addAction(QIcon(":/cash.png"), tr("Cash\nmovement"), this, [this] { openCashTransfer(); });
    }

    return tb;
}

void RCashMovement::openCashTransfer()
{
    auto *dlg = new C5CashTransfer(mUser, this);

    const QJsonObject currencyObj = filterObject(QStringLiteral("currency"));
    if(!currencyObj.isEmpty()) {
        dlg->setCurrencyId(currencyObj.value(QStringLiteral("currency")).toInt(1));
    }

    const QJsonObject cashboxObj = filterObject(QStringLiteral("cashbox"));
    if(!cashboxObj.isEmpty()) {
        const int cashboxId = cashboxObj.value(QStringLiteral("cashbox")).toInt();
        if(cashboxId > 0) {
            const QString cashboxName = __c5config.getRegValue(
                QStringLiteral("rfilter_%1_cashbox_name").arg(mEditorName)).toString();
            dlg->setSourceCashbox(cashboxId,
                                  cashboxName.isEmpty() ? QString::number(cashboxId) : cashboxName);
        }
    }

    if(dlg->exec() == QDialog::Accepted) {
        getData();
    }
    dlg->deleteLater();
}

void RCashMovement::on_tbl_doubleClicked(const QModelIndex &index)
{
    const QJsonObject viewModeObj = filterObject(QStringLiteral("viewmode"));
    if(viewModeObj.isEmpty() || viewModeObj.value(QStringLiteral("viewmode")).toInt() != 2) {
        RAbstractEditorReport::on_tbl_doubleClicked(index);
        return;
    }

    if(!index.isValid()) {
        return;
    }

    const QModelIndex srcIndex = reportMapViewIndexToSource(index);
    if(!srcIndex.isValid()) {
        return;
    }

    const QString orderId = reportSourceCellData(srcIndex.row(), 3).toString().trimmed();
    if(!orderId.isEmpty()) {
        return;
    }

    bool okConv = false;
    const int opId = reportSourceCellData(srcIndex.row(), 0).toString().trimmed().toInt(&okConv);
    if(!okConv || opId <= 0) {
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/waiter/cashbox/get-cash-operation"),
                         mUser->mSessionKey,
                         this,
                         {{"id", opId}},
                         [this](const QJsonObject &jo) {
                             const QJsonObject op = jo.value(QStringLiteral("operation")).toObject();
                             if(op.isEmpty()) {
                                 return;
                             }
                             auto *dlg = new C5RevenueCashOp(mUser, this);
                             dlg->applyOperationForEdit(op);
                             if(dlg->exec() == QDialog::Accepted) {
                                 getData();
                             }
                             dlg->deleteLater();
                         });
}

void RCashMovement::newData()
{
    int cashboxId = 0;
    int currencyId = 1;
    const QJsonObject cashboxObj = filterObject("cashbox");
    if(!cashboxObj.isEmpty()) {
        cashboxId = cashboxObj.value("cashbox").toInt();
    }
    const QJsonObject currencyObj = filterObject("currency");
    if(!currencyObj.isEmpty()) {
        currencyId = currencyObj.value("currency").toInt(1);
    }

    C5RevenueCashOp d(mUser, this);
    d.setCashboxAndCurrency(cashboxId, currencyId);
    if(d.exec() == QDialog::Accepted) {
        getData();
    }
}
