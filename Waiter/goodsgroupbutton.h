#pragma once
#include <QFrame>

class QLabel;

class GoodsGroupButton: public QFrame
{
    Q_OBJECT
public:
    GoodsGroupButton(const QString &text, QWidget *parent = nullptr);

    void setColor(int c);

protected:
    void mousePressEvent(QMouseEvent *e) override;

    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    QLabel* mLabel;

signals:
    void clicked();
};
