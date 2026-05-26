#pragma once

#include <QFrame>

#include "menutypes.h"

class QMouseEvent;

class QLabel;
class QPushButton;

class DishCardWidget : public QFrame
{
    Q_OBJECT

public:
    enum class Style { Grid, Popular, Listing };

    explicit DishCardWidget(Style style, const MenuDish &dish, QWidget *parent = nullptr);

signals:
    void addToCartClicked(int dishId);
    void infoClicked(int dishId);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    void buildGridCard(const MenuDish &dish);
    void buildTileCard(const MenuDish &dish, const char *cardObjectName);

    int m_dishId = 0;
    QPushButton *m_infoButton = nullptr;
};
