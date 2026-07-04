#pragma once

#include <QFrame>
#include <QPixmap>
#include <QPoint>
#include <QString>

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

    int dishId() const;
    QPixmap thumbnailPixmap() const;
    QPoint flyStartGlobalPos() const;

signals:
    void addToCartClicked(int dishId);
    void infoClicked(int dishId);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    void buildGridCard(const MenuDish &dish);
    void buildTileCard(const MenuDish &dish, const char *cardObjectName);
    void applyCardLabelStyles();

    int m_dishId = 0;
    QString m_imagePath;
    QPushButton *m_infoButton = nullptr;
};
