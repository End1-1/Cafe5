#include "paymenttypenames.h"
#include "dict_payment_type.h"
#include "ninterface.h"
#include <QCoreApplication>
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>

namespace {

QHash<int, QString> &customNames()
{
    static QHash<int, QString> names;
    return names;
}

QHash<int, bool> &waiterFlags()
{
    static QHash<int, bool> flags;
    return flags;
}

} // namespace

namespace PaymentTypeNames
{

QString title(int paymentTypeId)
{
    const QString custom = customNames().value(paymentTypeId).trimmed();
    if (!custom.isEmpty()) {
        return custom;
    }
    const char *nm = payment_names.value(paymentTypeId);
    return nm ? QCoreApplication::translate("PaymentType", nm) : QString::number(paymentTypeId);
}

void setCustomName(int paymentTypeId, const QString &name)
{
    if (name.trimmed().isEmpty()) {
        customNames().remove(paymentTypeId);
    } else {
        customNames().insert(paymentTypeId, name.trimmed());
    }
}

bool waiterEnabled(int paymentTypeId)
{
    // Missing key → allowed (until server payload arrives / type not in editor list).
    return waiterFlags().value(paymentTypeId, true);
}

void setWaiterEnabled(int paymentTypeId, bool enabled)
{
    waiterFlags().insert(paymentTypeId, enabled);
}

void load(const QString &sessionKey, QObject *context, std::function<void()> done)
{
    NInterface::query(QStringLiteral("/engine/v2/officen/editors/get-all"),
                      sessionKey,
                      context,
                      {{QStringLiteral("editor"), QStringLiteral("form_payment_types")}},
                      [done](const QJsonObject &jdoc) {
                          customNames().clear();
                          waiterFlags().clear();
                          const QJsonArray rows = jdoc.value(QStringLiteral("rows")).toArray();
                          for (const QJsonValue &v : rows) {
                              const QJsonArray r = v.toArray();
                              setCustomName(r.at(0).toVariant().toInt(), r.at(2).toString());
                          }
                          const QJsonObject flags = jdoc.value(QStringLiteral("waiter_by_id")).toObject();
                          for (auto it = flags.begin(); it != flags.end(); ++it) {
                              setWaiterEnabled(it.key().toInt(), it.value().toVariant().toInt() != 0);
                          }
                          if (done) {
                              done();
                          }
                      },
                      [](const QJsonObject &) { return true; },
                      false);
}

} // namespace PaymentTypeNames
