#ifndef CR5SALEFROMSTOREFILTER_H
#define CR5SALEFROMSTOREFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5SaleFromStoreFilter;
}

class CR5SaleFromStoreFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5SaleFromStoreFilter(const QStringList &dbParams, QWidget *parent = 0);

    ~CR5SaleFromStoreFilter();

    virtual QString condition();

    QDate d1();

    QDate d2();

private:
    Ui::CR5SaleFromStoreFilter *ui;
};

#endif // CR5SALEFROMSTOREFILTER_H
