#ifndef TABLEWIDGETITEM_H
#define TABLEWIDGETITEM_H

#include <QTableWidgetItem>

class TableWidgetItem : public QTableWidgetItem
{
public:
    explicit TableWidgetItem(int type = Type);

    explicit TableWidgetItem(const QString &text, int type = Type);

    virtual QVariant data(int role) const override;

    inline QString text() {return data(Qt::DisplayRole).toString(); }

    int fDecimals;
};

#endif // TABLEWIDGETITEM_H
