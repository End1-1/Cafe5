#pragma once

#include <QObject>
#include <QString>
#include <functional>

/** Payment type display names: custom name from the server (cash_payment_types.f_name), else translated default. */
namespace PaymentTypeNames
{
QString title(int paymentTypeId);

void setCustomName(int paymentTypeId, const QString &name);

/** Fetch custom names from the server; done() runs after the names are applied. */
void load(const QString &sessionKey, QObject *context, std::function<void()> done = {});
}
