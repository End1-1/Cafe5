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
    explicit CR5SaleAndStoreFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5SaleAndStoreFilter();

    virtual QString condition() override;

    const QDate date1() const;

    const QDate date2() const;

    const QString store() const;

    const QString goodsGroup() const;

    const QString goods() const;

private:
    Ui::CR5SaleAndStoreFilter *ui;
};

#endif // CR5SALEANDSTOREFILTER_H
