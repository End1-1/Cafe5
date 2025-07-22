#ifndef STOREDECOMPLECTATION_H
#define STOREDECOMPLECTATION_H

#include "c5widget.h"

namespace Ui
{
class StoreDecomplectation;
}

class StoreDecomplectation : public C5Widget
{
    Q_OBJECT
public:
    explicit StoreDecomplectation(QWidget *parent = nullptr);
    ~StoreDecomplectation();

    virtual QToolBar* toolBar() override;

private:
    Ui::StoreDecomplectation* ui;

    int addGoodsRow();

private slots:
    void save();

    void draft();

    void on_btnPlus_clicked();
    void on_btnRemove_clicked();
};

#endif // STOREDECOMPLECTATION_H
