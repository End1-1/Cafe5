#pragma once

#include <QWidget>

class DishCardWidget;

class DishCardAnimHost : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal slideY READ slideY WRITE setSlideY)

public:
    explicit DishCardAnimHost(DishCardWidget *card, QWidget *parent = nullptr);

    DishCardWidget *card() const { return m_card; }

    qreal slideY() const { return m_slideY; }
    void setSlideY(qreal y);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void syncCardGeometry();

    DishCardWidget *m_card = nullptr;
    qreal m_slideY = 0.0;
};
