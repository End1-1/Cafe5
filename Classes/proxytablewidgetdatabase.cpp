#include "proxytablewidgetdatabase.h"
#include "c5database.h"
#include "c5tablewidget.h"

ProxyTableWidgetDatabase::ProxyTableWidgetDatabase()
{
}

void ProxyTableWidgetDatabase::fillTableWidgetRowFromDatabase(C5Database *db, C5TableWidget *t)
{
    int cols = db->columnCount();
    while (db->nextRow()) {
        int row = t->addEmptyRow();
        for (int i = 0; i < cols; i++) {
            QVariant v = db->getValue(i);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            switch (v.type()) {
                case QVariant::Double:
                    t->setString(row, i, QString::number(v.toDouble(), 'f', 3)
                                 .remove(QRegularExpression("(?!\\d[\\.\\,][1-9]+)0+$"))
                                 .remove(QRegularExpression("[\\.\\,]$")));
                    break;
                default:
                    t->setString(row, i, v.toString());
                    break;
            }
#else
            switch (v.typeId()) {
                case QMetaType::Double:
                    t->setString(row, i, QString::number(v.toDouble(), 'f', 3)
                                 .remove(QRegularExpression("(?!\\d[\\.\\,][1-9]+)0+$"))
                                 .remove(QRegularExpression("[\\.\\,]$")));
                    break;
                default:
                    t->setString(row, i, v.toString());
                    break;
            }
#endif
        }
    }
}
