#include "selfboardordersubmit.h"

#include "appsettings.h"
#include "ndataprovider.h"
#include "ninterface.h"
#include "serverconfig.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <memory>

namespace {

QString apiErrorMessage(const QJsonObject &jdoc)
{
    QString msg = jdoc.value(QStringLiteral("message")).toString();
    if (msg.isEmpty()) {
        msg = jdoc.value(QStringLiteral("errorMessage")).toString();
    }
    return msg;
}

QJsonObject dishDataJson(const MenuDish &dish)
{
    QJsonObject fData;
    const MenuDynamicAttributes attrs = dish.dynamicAttributes();
    QJsonObject dyn = attrs.toJsonObject();
    if (!dyn.isEmpty()) {
        fData.insert(QStringLiteral("f_dynamic_attributes"), dyn);
    }
    if (dish.packageId > 0) {
        fData.insert(QStringLiteral("f_package_id"), dish.packageId);
        if (!dish.packageName.isEmpty()) {
            fData.insert(QStringLiteral("f_package_name"), dish.packageName);
        }
    }
    if (!dish.selectedModificators.isEmpty()) {
        QJsonArray modificators;
        for (const MenuSelectedModificator &modificator : dish.selectedModificators) {
            QJsonObject item;
            item.insert(QStringLiteral("f_id"), modificator.id);
            item.insert(QStringLiteral("f_name"), modificator.name);
            item.insert(QStringLiteral("f_price"), modificator.price);
            modificators.append(item);
        }
        fData.insert(QStringLiteral("f_modificators"), modificators);
    }
    return fData;
}

QJsonObject buildAddDishParams(const CartLine &line,
                               int tableId,
                               int cashboxId,
                               double serviceFactor,
                               int rowIndex,
                               bool emptyOrder,
                               const QString &orderId)
{
    const MenuDish &dish = line.dish;
    QJsonObject params{
        {QStringLiteral("dish"), dish.id},
        {QStringLiteral("dish_name"), dish.name},
        {QStringLiteral("table"), tableId},
        {QStringLiteral("qty"), line.quantity},
        {QStringLiteral("type"), dish.type},
        {QStringLiteral("row"), rowIndex * 100},
        {QStringLiteral("price"), dish.price},
        {QStringLiteral("store"), dish.store},
        {QStringLiteral("print1"), dish.print1},
        {QStringLiteral("print2"), dish.print2},
        {QStringLiteral("cashbox_id"), cashboxId},
        {QStringLiteral("count_service"), dish.countService ? 1 : 0},
        {QStringLiteral("count_discount"), dish.countDiscount ? 1 : 0},
        {QStringLiteral("empty_order"), emptyOrder},
        {QStringLiteral("service_factor"), serviceFactor},
        {QStringLiteral("f_data"), dishDataJson(dish)},
    };
    if (!orderId.isEmpty()) {
        params.insert(QStringLiteral("order_id"), orderId);
    }
    return params;
}

} // namespace

void SelfBoardOrderSubmit::submit(OrderCart *cart,
                                  SelfBoardServiceMode serviceMode,
                                  QObject *context,
                                  FinishedCallback finished)
{
    if (!cart || cart->isEmpty()) {
        finished(false, QString(), QCoreApplication::translate("SelfBoardOrderSubmit", "Cart is empty"));
        return;
    }

    AppSettings::loadFromSettings();
    const int tableId = serviceMode == SelfBoardServiceMode::TakeAway
                            ? AppSettings::tableTakeAway()
                            : AppSettings::tableDineIn();
    const int cashboxId = AppSettings::cashboxId();
    const double serviceFactor = AppSettings::serviceFactor();

    if (tableId <= 0) {
        finished(false, QString(), QCoreApplication::translate("SelfBoardOrderSubmit", "Configure table id in settings"));
        return;
    }
    if (cashboxId <= 0) {
        finished(false, QString(), QCoreApplication::translate("SelfBoardOrderSubmit", "Configure cashbox id in settings"));
        return;
    }

    struct State
    {
        QVector<CartLine> lines;
        QString orderId;
        int nextIndex = 0;
        int tableId = 0;
        int cashboxId = 0;
        double serviceFactor = 0.0;
        double totalAmount = 0.0;
        QString sessionKey;
        QObject *context = nullptr;
        FinishedCallback finished;
    };

    auto state = std::make_shared<State>();
    state->lines = cart->lines();
    state->tableId = tableId;
    state->cashboxId = cashboxId;
    state->serviceFactor = serviceFactor;
    state->totalAmount = cart->totalAmount();
    state->sessionKey = ServerConfig::sessionKey();
    state->context = context;
    state->finished = std::move(finished);

    const auto fail = [state](const QString &error) {
        if (state->finished) {
            state->finished(false, QString(), error);
        }
    };

    const auto addNextDish = std::make_shared<std::function<void()>>();
    const auto printService = std::make_shared<std::function<void()>>();
    const auto setCardPayment = std::make_shared<std::function<void()>>();
    const auto closeOrder = std::make_shared<std::function<void()>>();

    *closeOrder = [state, fail, closeOrder]() {
        NInterface::query(
            QStringLiteral("/engine/v2/waiter/order/close-order"),
            state->sessionKey,
            state->context,
            QJsonObject{
                {QStringLiteral("id"), state->orderId},
                {QStringLiteral("fiscal"), QJsonObject{}},
                {QStringLiteral("cashbox_id"), state->cashboxId},
                {QStringLiteral("cost_depend_on_service_and_discount"), false},
                {QStringLiteral("cash_session_id"), 0},
            },
            [state, fail](const QJsonObject &jdoc) {
                if (jdoc.value(QStringLiteral("status")).toInt() != 1) {
                    fail(apiErrorMessage(jdoc));
                    return;
                }
                const QJsonObject order = jdoc.value(QStringLiteral("order")).toObject();
                const QString orderNumber = order.value(QStringLiteral("f_prefix")).toString();
                if (state->finished) {
                    state->finished(true, orderNumber, QString());
                }
            },
            [fail](const QJsonObject &jerr) -> bool {
                fail(apiErrorMessage(jerr));
                return true;
            },
            true,
            120000,
            true);
    };

    *setCardPayment = [state, fail, setCardPayment, closeOrder]() {
        NInterface::query(
            QStringLiteral("/engine/v2/waiter/order/set-amount"),
            state->sessionKey,
            state->context,
            QJsonObject{
                {QStringLiteral("id"), state->orderId},
                {QStringLiteral("payment_field"), QStringLiteral("f_amount_card")},
                {QStringLiteral("amount"), state->totalAmount},
            },
            [state, fail, closeOrder](const QJsonObject &jdoc) {
                if (jdoc.value(QStringLiteral("status")).toInt() != 1) {
                    fail(apiErrorMessage(jdoc));
                    return;
                }
                (*closeOrder)();
            },
            [fail](const QJsonObject &jerr) -> bool {
                fail(apiErrorMessage(jerr));
                return true;
            },
            true,
            120000,
            true);
    };

    *printService = [state, fail, printService, setCardPayment]() {
        NInterface::query(
            QStringLiteral("/engine/v2/waiter/order/print-service-check"),
            state->sessionKey,
            state->context,
            QJsonObject{{QStringLiteral("header_id"), state->orderId}},
            [state, fail, setCardPayment](const QJsonObject &jdoc) {
                if (jdoc.value(QStringLiteral("status")).toInt() != 1) {
                    fail(apiErrorMessage(jdoc));
                    return;
                }
                const QJsonObject order = jdoc.value(QStringLiteral("order")).toObject();
                const double serverTotal = order.value(QStringLiteral("f_amounttotal")).toDouble();
                if (serverTotal > 0.0) {
                    state->totalAmount = serverTotal;
                }
                (*setCardPayment)();
            },
            [fail](const QJsonObject &jerr) -> bool {
                fail(apiErrorMessage(jerr));
                return true;
            },
            true,
            120000,
            true);
    };

    *addNextDish = [state, fail, addNextDish, printService]() {
        if (state->nextIndex >= state->lines.size()) {
            (*printService)();
            return;
        }

        const CartLine &line = state->lines.at(state->nextIndex);
        const bool emptyOrder = state->orderId.isEmpty();
        const QJsonObject params = buildAddDishParams(
            line,
            state->tableId,
            state->cashboxId,
            state->serviceFactor,
            state->nextIndex,
            emptyOrder,
            state->orderId);

        NInterface::query(
            QStringLiteral("/engine/v2/waiter/order/add-dish"),
            state->sessionKey,
            state->context,
            params,
            [state, fail, addNextDish](const QJsonObject &jdoc) {
                if (jdoc.value(QStringLiteral("status")).toInt() != 1) {
                    fail(apiErrorMessage(jdoc));
                    return;
                }
                const QJsonObject order = jdoc.value(QStringLiteral("order")).toObject();
                const QString orderId = order.value(QStringLiteral("f_id")).toString();
                if (orderId.isEmpty()) {
                    fail(QCoreApplication::translate("SelfBoardOrderSubmit", "Order id is missing in server response"));
                    return;
                }
                state->orderId = orderId;
                ++state->nextIndex;
                (*addNextDish)();
            },
            [fail](const QJsonObject &jerr) -> bool {
                fail(apiErrorMessage(jerr));
                return true;
            },
            true,
            120000,
            true);
    };

    (*addNextDish)();
}
