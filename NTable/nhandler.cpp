#include "nhandler.h"
#include "nfilterdlg.h"
#include "c5database.h"
#include "c5waiterorder.h"
#include "c5mainwindow.h"
#include "c5storedoc.h"
#include "ntablemodel.h"
#include "c5saledoc.h"
#include "c5message.h"
#include <QToolButton>
#include <QGridLayout>
#include <QTableView>

#include "c5discountredeem.h"
#include "c5salefromstoreorder.h"
#include "c5costumerdebtpayment.h"
#include "c5dishwidget.h"

static const QString hDebt = "90dd520c-f072-11ee-b90b-7c10c9bcac82";
static const QString hShortDebt = "ec26fd1c-2391-11ef-a99a-7c10c9bcac82";
static const QString hDraftSale  = "39617ca7-8fa4-11ed-8ad3-1078d2d2b808";
static const QString hDiscountReturnAmount = "d563c585-aeb9-11f0-a2cb-8a884be02f31";
static const QString hMenuReview = "65a0e1d4-c843-11f0-9ee3-0a002700000e";

NHandler::NHandler(C5User *user, QObject *parent)
    : QObject{parent},
      mTableView(nullptr),
      mUser(user)
{
}

void NHandler::configure(NFilterDlg *filter, const QVariantList &handlers, QTableView *tv)
{
    mFilterDlg = filter;
    mHandlers = handlers;
    mTableView = tv;
}

void NHandler::handle(const QJsonArray &ja)
{
    if(mHandlers.length() < 2) {
        return;
    }

    int row = mTableView->currentIndex().row();
    auto *m = static_cast<NTableModel*>(mTableView->model());

    if(mHandlers.at(1).toString() == hDebt) {
        switch(mFilterDlg->filterValue("mode").toInt()) {
        case 1:
        case 4:
            if(!ja.at(3).toString().isEmpty()) {
                C5Database db;
                db[":f_id"] = ja.at(3).toString();
                db.exec("select f_source from o_header where f_id=:f_id");

                if(db.nextRow()) {
                    switch(abs(db.getInt(0))) {
                    case 1: {
                        C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>();
                        wo->setOrder(ja.at(3).toString());
                        break;
                    }

                    case 2: {
                        C5SaleFromStoreOrder::openOrder(mUser, ja.at(3).toString());
                        break;
                    }
                    }
                }
            } else if(!ja.at(4).toString().isEmpty()) {
                C5CostumerDebtPayment d(mUser, 0);
                d.setId(ja.at(4).toString());
                d.exec();
            } else if(!ja.at(5).toString().isEmpty()) {
                C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>();
                QString err;
                sd->openDoc(ja.at(5).toString(), err);

                if(!err.isEmpty()) {
                    C5Message::error(err);
                }
            }

            break;
        }
    } else if(mHandlers.at(1).toString() == hShortDebt) {
        C5CostumerDebtPayment d(mUser, 0);
        d.setId(ja.at(0).toString());
        d.exec();
    } else if(mHandlers.at(1).toString() == hDraftSale) {
        auto *retaildoc = __mainWindow->createTab<C5SaleDoc>();
        retaildoc->openDraft(ja.at(0).toString());
    } else if(mHandlers.at(1).toString() == hDiscountReturnAmount) {
        if(row < 0) {
            return;
        }

        int type = m->data(row, 3).toInt();

        switch(type) {
        case 4: {
        }

        case 9: {
            C5DiscountRedeem dr(mUser, m->data(row, 0).toString(), m->data(row, 0).toInt());
            dr.exec();
        }
        }
    } else if(mHandlers.at(1).toString()  == hMenuReview) {
        if(row < 0) {
            return;
        }

        auto *ep = new C5DishWidget();
        auto *e = C5Editor::createEditor(mUser, ep, m->data(row, 0).toInt());
        QList<QMap<QString, QVariant> > data;

        if(e->getResult(data)) {
        }

        delete e;
    }
}

void NHandler::handle(const QVariant &v)
{
    if(mHandlers.isEmpty()) {
        return;
    }

    if(mHandlers.at(1).toString()  == hMenuReview) {
        auto *ep = new C5DishWidget();
        auto *e = C5Editor::createEditor(mUser, ep, v.toInt());
        QList<QMap<QString, QVariant> > data;

        if(e->getResult(data)) {
        }

        delete e;
    }
}

void NHandler::toolWidget(QWidget *w)
{
    auto *gl = static_cast<QGridLayout*>(w->layout());
    NTableModel *m = nullptr;

    if(mTableView) {
        m = static_cast<NTableModel*>(mTableView->model());
    }

    if(!m) {
        return;
    }

    int row = mTableView->currentIndex().row();

    if(mHandlers.size() > 0) {
        if(mHandlers.contains(hDebt)) {
            auto *b = new QToolButton(w);
            b->setIcon(QIcon(":/new.png"));
            b->setText(tr("New customer payment"));
            b->setAutoRaise(true);
            b->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            connect(b, &QAbstractButton::clicked, [this]() {
                C5CostumerDebtPayment(mUser, BCLIENTDEBTS_SOURCE_SALE).exec();
            });
            gl->addWidget(b, 0, 0, Qt::AlignLeft);
            b = new QToolButton(w);
            b->setIcon(QIcon(":/new.png"));
            b->setText(tr("New partner payment"));
            b->setAutoRaise(true);
            b->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            connect(b, &QAbstractButton::clicked, [this]() {
                C5CostumerDebtPayment(mUser, BCLIENTDEBTS_SOURCE_INPUT).exec();
            });
            gl->addWidget(b, 1, 0, Qt::AlignLeft);
        } else if(mHandlers.contains(hShortDebt)) {
            auto *b = new QToolButton(w);
            b->setIcon(QIcon(":/new.png"));
            b->setText(tr("Spent accumulated"));
            b->setAutoRaise(true);
            b->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            connect(b, &QAbstractButton::clicked, this, [this, m, row]() {
                if(row < 0) {
                    return;
                }

                C5CostumerDebtPayment cb(mUser, BCLIENTDEBTS_SOURCE_SALE);
                cb.setProperty("uuid", m->data(row, 0).toString());
                cb.setPartnerAndAmount(m->data(row, 1, Qt::EditRole).toInt(), m->data(row, 6, Qt::EditRole).toDouble(),
                                       m->data(row, 0).toString());
                cb.exec();
            });
            gl->addWidget(b, 0, 0, Qt::AlignLeft);
        } else if(mHandlers.contains(hDraftSale)) {
        } else if(mHandlers.contains(hDiscountReturnAmount)) {
            auto * b  =  new QToolButton(w);
            b->setIcon(QIcon(":/new.png"));
            b->setText(tr("Redeem accumulated"));
            b->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            connect(b, &QToolButton::clicked, this, [this, m, row]() {
                int partner = 0;

                if(row > -1) {
                    partner = m->data(row, 2).toInt();
                }

                C5DiscountRedeem dr(mUser, "", partner);
                dr.exec();
            });
            gl->addWidget(b);
        } else if(mHandlers.contains(hMenuReview)) {
        }
    }
}
