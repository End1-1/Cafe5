#ifndef CR5SALEANDSTOREFILTER_H
#define CR5SALEANDSTOREFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5SaleAndStoreFilter;
}

class CR5SaleAndStoreFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5SaleAndStoreFilter(QWidget *parent = nullptr);

    ~CR5SaleAndStoreFilter();

    enum Display {dNone = 0, dGoods, dGroups};

    virtual QString condition() override;

    virtual QString filterText() override;

    const QDate date1() const;

    const QDate date2() const;

    const QString store() const;

    const QString goodsGroup() const;

    const QString goods() const;

    Display display();

private:
    Ui::CR5SaleAndStoreFilter *ui;
};

#endif // CR5SALEANDSTOREFILTER_H
