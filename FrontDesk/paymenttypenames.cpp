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

void load(const QString &sessionKey, QObject *context, std::function<void()> done)
{
    NInterface::query(QStringLiteral("/engine/v2/officen/editors/get-all"),
                      sessionKey,
                      context,
                      {{QStringLiteral("editor"), QStringLiteral("form_payment_types")}},
                      [done](const QJsonObject &jdoc) {
                          customNames().clear();
                          const QJsonArray rows = jdoc.value(QStringLiteral("rows")).toArray();
                          for (const QJsonValue &v : rows) {
                              const QJsonArray r = v.toArray();
                              setCustomName(r.at(0).toVariant().toInt(), r.at(2).toString());
                          }
                          if (done) {
                              done();
                          }
                      },
                      [](const QJsonObject &) { return true; },
                      false);
}

} // namespace PaymentTypeNames
