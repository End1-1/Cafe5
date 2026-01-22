#include "dlgsplitorder.h"
#include "ui_dlgsplitorder.h"
#include "dlgpassword.h"
#include "waiterdishwidget.h"
#include "waiterdishwidget.h"
#include "ninterface.h"
#include "c5user.h"
#include "c5utils.h"
#include "c5message.h"
#include "dlgtables.h"
#include <QScrollBar>
#include <QPointer>
#include <QTimer>

DlgSplitOrder::DlgSplitOrder(WaiterOrder wo, C5User *user) :
    C5WaiterDialog(user),
    ui(new Ui::DlgSplitOrder)
{
    ui->setupUi(this);
    mOrder = wo;
    mOrder1 = wo;
    restoreLeftSide();
    ui->lbLeft->setText(wo.tableName);
}

DlgSplitOrder::~DlgSplitOrder()
{
    delete ui;
}

void DlgSplitOrder::accept()
{
    overrideAcceptReject(true);
}

void DlgSplitOrder::reject()
{
    overrideAcceptReject(false);
}

void DlgSplitOrder::focused1(const QString &id)
{
    for(int i = 0; i < ui->vo1->count(); i++) {
        auto *l = ui->vo1->itemAt(i);

        if(auto *ow = qobject_cast<WaiterDishWidget*>(l->widget())) {
            ow->checkFocus(id);
        }
    }
}

void DlgSplitOrder::focused2(const QString &id)
{
    for(int i = 0; i < ui->vo2->count(); i++) {
        auto *l = ui->vo2->itemAt(i);

        if(auto *ow = qobject_cast<WaiterDishWidget*>(l->widget())) {
            ow->checkFocus(id);
        }
    }
}

void DlgSplitOrder::on_btnChoseTable_clicked()
{
    QPointer<DlgSplitOrder> self(this);
    DlgTables d(self->mUser);
    int tableId = d.exec();

    if(tableId == 0) {
        return;
    }

    if(tableId == self->mOrder1.table) {
        C5Message::error(tr("Same tables"));
        return;
    }

    NInterface::query1("/engine/v2/waiter/order/open-table", self->mUser->mSessionKey, self, {
        {"table", tableId},
        {"locksrc", hostinfo},
        {"create_empty", true}
    },
    [self](const QJsonObject  & jdoc1) {
        if(!self) {
            return;
        }

        self->restoreLeftSide();
        self->mOrder2 = JsonParser<WaiterOrder>::fromJson(jdoc1["order"].toObject());
        self->ui->lbRight->setText(self->mOrder2.tableName);

        while(auto *l =  self->ui->vo2->takeAt(0)) {
            if(auto *w = l->widget()) {
                w->deleteLater();
            }

            delete l;
        }

        for(int i = 0 ; i <  self->mOrder2.dishes.size(); i++) {
            WaiterDish w = self->mOrder2.dishes.at(i);
            auto *ow = new WaiterDishWidget(w, false);;
            self->ui->vo2->addWidget(ow);
            ow->updateDish(w);
            connect(ow, &WaiterOrderItemWidget::focused, self, &DlgSplitOrder::focused2);
        }

        self->ui->vo2->addStretch();
    });
}

void DlgSplitOrder::on_btnExit_clicked()
{
    reject();
}

void DlgSplitOrder::on_btnLeftToRight_clicked()
{
    if(mOrder2.table == 0) {
        C5Message::error(tr("Select destination table"));
        return;
    }

    int index = -1;

    for(int i = 0; i < mOrder1.dishes.count(); i++) {
        if(mOrder1.dishes.at(i).state != DISH_STATE_OK) {
            continue;
        }

        if(auto *l = ui->vo1->itemAt(i)) {
            if(auto w = qobject_cast<WaiterOrderItemWidget*>(l->widget())) {
                if(w->isFocused()) {
                    index = i;
                    break;
                }
            }
        }
    }

    if(index > -1) {
        moveItemLeftRight(index, mOrder1, mOrder2, ui->vo1, ui->vo2);
    }
}

bool DlgSplitOrder::moveItem(C5OrderDriver *or1, C5OrderDriver *or2, int row)
{
    return false;
}

void DlgSplitOrder::overrideAcceptReject(bool isAccept)
{
    auto exitFunc = [ = ]() {
        if(isAccept) {
            C5WaiterDialog::accept();
        } else {
            C5WaiterDialog::reject();
        }
    };

    if(mOrder2.table == 0) {
        exitFunc();
    } else {
        NInterface::query1("/engine/v2/waiter/order/unlock-table", mUser->mSessionKey, this, {
            {"table", mOrder2.table},
            {"locksrc", hostinfo}
        },
        [ = ](const QJsonObject & jdoc) {
            exitFunc();
        });
    }
}

void DlgSplitOrder::moveItemLeftRight(int index, WaiterOrder &w1, WaiterOrder &w2, QVBoxLayout *l1, QVBoxLayout *l2)
{
    w2.dishes.append(w1.dishes.at(index));
    w1.dishes.removeAt(index);

    if(auto *l = l1->takeAt(index)) {
        if(auto *w = l->widget()) {
            w->deleteLater();
        }

        delete l;
    }

    WaiterDish w = w2.dishes.last();
    auto *ow = new WaiterDishWidget(w, false);
    int insertPos = qMax(0, l2->count() - 1);
    l2->insertWidget(insertPos, ow);
    ow->updateDish(w);
    disconnect(ow, &WaiterOrderItemWidget::focused, this, &DlgSplitOrder::focused1);
    disconnect(ow, &WaiterOrderItemWidget::focused, this, &DlgSplitOrder::focused2);

    if(l1 == ui->vo1) {
        connect(ow, &WaiterOrderItemWidget::focused, this, &DlgSplitOrder::focused1);
    } else {
        connect(ow, &WaiterOrderItemWidget::focused, this, &DlgSplitOrder::focused2);
    }
}

void DlgSplitOrder::on_btnAllLeftToRigh_clicked()
{
    if(mOrder2.table == 0) {
        C5Message::error(tr("Select destination table"));
        return;
    }

    for(int i = mOrder1.dishes.count() - 1; i >= 0; --i) {
        if(mOrder1.dishes.at(i).state != DISH_STATE_OK) {
            continue;
        }

        auto *item = ui->vo1->itemAt(i);

        if(!item) {
            continue;
        }

        auto *w = qobject_cast<WaiterOrderItemWidget*>(item->widget());

        if(!w) {
            continue;
        }

        moveItemLeftRight(i, mOrder1, mOrder2, ui->vo1, ui->vo2);
    }
}

void DlgSplitOrder::on_btnRightToLeft_clicked()
{
    if(mOrder1.table == 0) {
        C5Message::error(tr("Select destination table"));
        return;
    }

    int index = -1;

    for(int i = 0; i < mOrder2.dishes.count(); i++) {
        if(mOrder2.dishes.at(i).state != DISH_STATE_OK) {
            continue;
        }

        if(auto *l = ui->vo2->itemAt(i)) {
            if(auto w = qobject_cast<WaiterOrderItemWidget*>(l->widget())) {
                if(w->isFocused()) {
                    index = i;
                    break;
                }
            }
        }
    }

    if(index > -1) {
        moveItemLeftRight(index, mOrder2, mOrder1, ui->vo2, ui->vo1);
    }
}

void DlgSplitOrder::on_btnAllRightToLeft_clicked()
{
    if(mOrder1.table == 0) {
        C5Message::error(tr("Select source table"));
        return;
    }

    for(int i = mOrder2.dishes.count() - 1; i >= 0; --i) {
        if(mOrder2.dishes.at(i).state != DISH_STATE_OK) {
            continue;
        }

        auto *item = ui->vo2->itemAt(i);

        if(!item) {
            continue;
        }

        auto *w = qobject_cast<WaiterOrderItemWidget*>(item->widget());

        if(!w) {
            continue;
        }

        moveItemLeftRight(i, mOrder2, mOrder1, ui->vo2, ui->vo1);
    }
}

void DlgSplitOrder::on_btnSave_clicked()
{
    QJsonArray jd;
    auto func = [&jd](WaiterOrder & wo1, WaiterOrder & wo2) {
        for(int i = 0; i < wo1.dishes.size(); i++) {
            auto d = wo1.dishes.at(i);

            if(d.state != DISH_STATE_OK) {
                continue;
            }

            if(d.header != wo1.id) {
                jd.append(QJsonObject{{"f_header", wo1.id}, {"f_id", d.id}, {"f_from_table", wo2.tableName}});
            }
        }
    };
    func(mOrder1, mOrder2);
    func(mOrder2, mOrder1);

    if(jd.isEmpty()) {
        C5Message::error(tr("No items to transfer"));
        return;
    }

    NInterface::query1("/engine/v2/waiter/order/transfer-items", mUser->mSessionKey, this, {
        {"data", jd},
        {"id1", mOrder1.id},
        {"id2", mOrder2.id}
    },
    [this](const QJsonObject & jdoc) {
        Q_UNUSED(jdoc);
        C5Message::info(tr("Transfer completed"));
        accept();
    });
}

void DlgSplitOrder::restoreLeftSide()
{
    mOrder1 = mOrder;

    while(auto *l = ui->vo1->takeAt(0)) {
        if(auto *w = l->widget()) {
            w->deleteLater();
        }

        delete l;
    }

    for(int i = 0 ; i <  mOrder1.dishes.size(); i++) {
        WaiterDish w = mOrder1.dishes.at(i);
        auto *ow = new WaiterDishWidget(w, false);;
        ui->vo1->addWidget(ow);
        ow->updateDish(w);
        connect(ow, &WaiterOrderItemWidget::focused, this, &DlgSplitOrder::focused1);
    }

    ui->vo1->addStretch();
}
