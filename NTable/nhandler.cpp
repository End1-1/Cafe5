#include "nhandler.h"
#include "nfilterdlg.h"
#include "c5database.h"
#include "c5waiterorder.h"
#include "c5salefromstoreorder.h"
#include "c5mainwindow.h"
#include "c5costumerdebtpayment.h"
#include "c5storedoc.h"

static const QString hDebt = "90dd520c-f072-11ee-b90b-7c10c9bcac82";

NHandler::NHandler(QObject *parent)
    : QObject{parent}
{

}

void NHandler::configure(NFilterDlg *filter, const QVariantList &handlers)
{
    mFilterDlg = filter;
    mHandlers = handlers;
}

void NHandler::handle(const QJsonArray &ja)
{
    if (mHandlers.length() < 2) {
        return;
    }
    if (mHandlers.at(1).toString() == hDebt) {
        switch (mFilterDlg->filterValue("mode").toInt()) {
        case 1:
            if (!ja.at(3).toString().isEmpty()) {
                C5Database db(__c5config.dbParams());
                db[":f_id"] = ja.at(3).toString();
                db.exec("select f_source from o_header where f_id=:f_id");
                if (db.nextRow()) {
                    switch (abs(db.getInt(0))) {
                    case 1: {
                        C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>(__c5config.dbParams());
                        wo->setOrder(ja.at(3).toString());
                        break;
                    }
                    case 2: {
                        C5SaleFromStoreOrder::openOrder(__c5config.dbParams(), ja.at(3).toString());
                        break;
                    }
                    }
                }
            } else if (!ja.at(4).toString().isEmpty()) {
                C5CostumerDebtPayment d(0, __c5config.dbParams());
                d.setId(ja.at(4).toString());
                d.exec();
            } else if(!ja.at(5).toString().isEmpty()) {
                C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>(__c5config.dbParams());
                QString err;
                sd->openDoc(ja.at(5).toString(), err);
                if (!err.isEmpty()) {
                    C5Message::error(err);
                }
            }
            break;
        }
    }
}
