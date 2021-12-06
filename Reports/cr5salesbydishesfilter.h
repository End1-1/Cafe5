#ifndef CR5SALESBYDISHESFILTER_H
#define CR5SALESBYDISHESFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5SalesByDishesFilter;
}

class CR5SalesByDishesFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5SalesByDishesFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5SalesByDishesFilter();

    virtual QString condition() override;

private slots:
    void on_chUseCloseDateTime_clicked(bool checked);

private:
    Ui::CR5SalesByDishesFilter *ui;
};

#endif // CR5SALESBYDISHESFILTER_H
