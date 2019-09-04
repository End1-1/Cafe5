#include "tablewidgetitem.h"
#include "doubleutils.h"
#include "dateutils.h"

TableWidgetItem::TableWidgetItem(int type)
{
    fDecimals = 2;
}

TableWidgetItem::TableWidgetItem(const QString &text, int type) :
    QTableWidgetItem(text, type)
{

}

QVariant TableWidgetItem::data(int role) const
{
    QVariant v = QTableWidgetItem::data(role);
    if (role == Qt::DisplayRole) {
       switch (v.type()) {
       case QVariant::Int:
           return v.toString();
       case QVariant::Date:
           return date_str(v.toDate());
       case QVariant::DateTime:
           return datetime_str(v.toDateTime());
       case QVariant::Time:
           return time_str(v.toTime());
       case QVariant::Double:
           return float_str(v.toDouble(), fDecimals);
       default:
           return v.toString();
       }
    }
    return v;
}
