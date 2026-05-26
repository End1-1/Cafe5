#pragma once

#include <QDialog>

#include "menutypes.h"

namespace Ui {
class DlgDishDetails;
}

class DlgDishDetails : public QDialog
{
    Q_OBJECT

public:
    explicit DlgDishDetails(const MenuDish &dish, QWidget *parent = nullptr);
    ~DlgDishDetails() override;

private:
    Ui::DlgDishDetails *ui;
};
