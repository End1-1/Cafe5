#pragma once

#include "menutypes.h"

#include <QWidget>

class QFrame;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;

class DlgDishDetails : public QWidget
{
    Q_OBJECT

public:
    explicit DlgDishDetails(const MenuDish &dish, QWidget *parent = nullptr);

signals:
    void closed();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void buildUi();
    void syncCardGeometry();

    MenuDish m_dish;
    QFrame *m_card = nullptr;
};
