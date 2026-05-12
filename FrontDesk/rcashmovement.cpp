#include "rcashmovement.h"
#include "c5mainwindow.h"
#include "c5revenuecashop.h"
#include "ninterface.h"
#include <QDialog>
#include <QJsonObject>

RCashMovement::RCashMovement(const QString &title, QIcon icon, const QString &editorName)
    : RAbstractEditorReport(title, icon, editorName)
{}

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

    const QString orderId = reportSourceCellData(srcIndex.row(), 2).toString().trimmed();
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
