#include "datahelper.h"

#include <QLocale>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QLineEdit>
#include <QWidget>

#include "eqlineedit.h"
#include "stringutils.h"

DataHelper::DataHelper() = default;

namespace {

bool widgetMatchesKey(const QWidget *w, const QString &key)
{
    if (!w || key.isEmpty()) {
        return false;
    }
    return w->property("Field").toString() == key;
}

QWidget *findWidgetForKey(QWidget *root, const QString &key)
{
    if (widgetMatchesKey(root, key)) {
        return root;
    }
    const QList<QWidget *> children = root->findChildren<QWidget *>(QString(), Qt::FindChildrenRecursively);
    for (QWidget *w : children) {
        if (widgetMatchesKey(w, key)) {
            return w;
        }
    }
    return nullptr;
}

QLineEdit *resolveLineEdit(QWidget *w)
{
    if (!w) {
        return nullptr;
    }
    if (auto *le = qobject_cast<QLineEdit *>(w)) {
        return le;
    }
    return w->findChild<QLineEdit *>();
}

QString dataTypeFor(QWidget *boundWidget, QLineEdit *le)
{
    QString dt = le ? le->property("DataType").toString() : QString();
    if (dt.isEmpty() && boundWidget) {
        dt = boundWidget->property("DataType").toString();
    }
    return dt.trimmed().toLower();
}

void setLineEditFromJson(QLineEdit *le, const QJsonValue &jv, const QString &dataType)
{
    if (!le) {
        return;
    }

    if (dataType == QLatin1String("integer")) {
        int n = 0;
        if (jv.type() == QJsonValue::Double) {
            n = static_cast<int>(jv.toDouble());
        } else if (jv.type() == QJsonValue::String) {
            n = jv.toString().toInt();
        } else {
            n = jv.toInt();
        }
        if (auto *eq = qobject_cast<EQLineEdit *>(le)) {
            eq->setInt(n);
        } else {
            le->setText(QString::number(n));
        }
        return;
    }
    if (dataType == QLatin1String("decimal")) {
        double d = 0;
        if (jv.type() == QJsonValue::String) {
            d = jv.toString().toDouble();
        } else {
            d = jv.toDouble();
        }
        if (auto *eq = qobject_cast<EQLineEdit *>(le)) {
            eq->setDouble(d);
        } else {
            le->setText(float_str(d, 2));
        }
        return;
    }

    if (jv.isNull() || jv.isUndefined()) {
        le->clear();
        return;
    }
    le->setText(jv.toVariant().toString());
}

bool nullableFor(QWidget *w, QLineEdit *le)
{
    const QVariant qn = w->property("Nullable");
    if (qn.isValid()) {
        return qn.toBool();
    }
    if (le) {
        const QVariant ln = le->property("Nullable");
        if (ln.isValid()) {
            return ln.toBool();
        }
    }
    return false;
}

QJsonValue lineEditToJsonValue(QLineEdit *le, const QString &dataType, bool nullable)
{
    if (!le) {
        return QJsonValue(QJsonValue::Null);
    }

    const QString dt = dataType.trimmed().toLower();

    if (dt == QLatin1String("integer")) {
        int n = 0;
        if (auto *eq = qobject_cast<EQLineEdit *>(le)) {
            n = eq->asInt();
        } else {
            n = le->text().trimmed().toInt();
        }
        if (nullable && n == 0) {
            return QJsonValue(QJsonValue::Null);
        }
        return QJsonValue(n);
    }
    if (dt == QLatin1String("decimal")) {
        const QString t = le->text().trimmed();
        if (nullable && t.isEmpty()) {
            return QJsonValue(QJsonValue::Null);
        }
        double d = 0;
        if (auto *eq = qobject_cast<EQLineEdit *>(le)) {
            d = eq->asDouble();
        } else {
            d = QLocale().toDouble(t);
        }
        return QJsonValue(d);
    }

    const QString s = le->text();
    if (nullable && s.isEmpty()) {
        return QJsonValue(QJsonValue::Null);
    }
    return QJsonValue(s);
}

bool isDisplayOnly(QWidget *w, QLineEdit *le)
{
    if (!w) {
        return false;
    }
    if (w->property("DisplayOnly").toBool()) {
        return true;
    }
    return le && le->property("DisplayOnly").toBool();
}

} // namespace

void DataHelper::setWidgetData(QWidget *dataWidget, const QJsonObject &jdoc)
{
    if (!dataWidget) {
        return;
    }
    for (auto it = jdoc.constBegin(); it != jdoc.constEnd(); ++it) {
        QWidget *w = findWidgetForKey(dataWidget, it.key());
        if (!w) {
            continue;
        }
        QLineEdit *le = resolveLineEdit(w);
        if (!le) {
            continue;
        }
        if (isDisplayOnly(w, le)) {
            continue;
        }
        const QString dt = dataTypeFor(w, le);
        setLineEditFromJson(le, it.value(), dt);
    }
}

bool DataHelper::saveData(QWidget *dataWidget)
{
    m_fieldSavePayload = QJsonArray();
    if (!dataWidget) {
        return false;
    }

    QList<QWidget *> widgets;
    widgets.append(dataWidget);
    widgets.append(dataWidget->findChildren<QWidget *>(QString(), Qt::FindChildrenRecursively));

    for (QWidget *w : widgets) {
        const QString field = w->property("Field").toString();
        if (field.isEmpty()) {
            continue;
        }
        QLineEdit *le = resolveLineEdit(w);
        if (!le) {
            continue;
        }
        if (isDisplayOnly(w, le)) {
            continue;
        }
        const QString dt = dataTypeFor(w, le);
        const bool nullable = nullableFor(w, le);

        QJsonObject row;
        row[QStringLiteral("field")] = field;
        row[QStringLiteral("table")] = w->property("Table").toString();
        row[QStringLiteral("value")] = lineEditToJsonValue(le, dt, nullable);
        m_fieldSavePayload.append(row);
    }

    return true;
}
