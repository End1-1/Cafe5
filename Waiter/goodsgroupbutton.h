#pragma once
#include <QFrame>

class QLabel;
class QResizeEvent;
class QShowEvent;

class GoodsGroupButton: public QFrame
{
    Q_OBJECT
public:
    GoodsGroupButton(const QString &text, QWidget *parent = nullptr);

    void setColor(int c);

protected:
    void mousePressEvent(QMouseEvent *e) override;

    void mouseReleaseEvent(QMouseEvent *e) override;

    void resizeEvent(QResizeEvent *e) override;

    void showEvent(QShowEvent *e) override;

private:
    QLabel* mLabel;

    int mBasePointSize = 0;

    void cacheBaseFontSize();

    void scaleLabelFont();

signals:
    void clicked();
};
