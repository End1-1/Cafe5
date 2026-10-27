#pragma once

#include <QObject>
#include <QString>
#include <functional>

/** Payment type display names: custom name from the server (cash_payment_types.f_name), else translated default. */
namespace PaymentTypeNames
{
QString title(int paymentTypeId);

void setCustomName(int paymentTypeId, const QString &name);

/** When false, Waiter must not show or accept this payment type (default true until loaded). */
bool waiterEnabled(int paymentTypeId);

void setWaiterEnabled(int paymentTypeId, bool enabled);

/** Transfer types: when false, payment does not ask for a partner. Default true until loaded. */
bool partnerRequired(int paymentTypeId);

void setPartnerRequired(int paymentTypeId, bool required);

/** Fetch custom names / Waiter flags from the server; done() runs after they are applied. */
void load(const QString &sessionKey, QObject *context, std::function<void()> done = {});
}
