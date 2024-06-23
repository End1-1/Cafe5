#ifndef QPROGRESSINDICATOR_H
#define QPROGRESSINDICATOR_H

#include <QColor>
#include <QWidget>

class QProgressIndicator : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int delay READ animationDelay WRITE setAnimationDelay)
    Q_PROPERTY(bool displayedWhenStopped READ isDisplayedWhenStopped WRITE setDisplayedWhenStopped)
    Q_PROPERTY(QColor color READ color WRITE setColor)
public:
    QProgressIndicator(QWidget *parent = 0);

    int animationDelay() const
    {
        return m_delay;
    }

    bool isAnimated() const;

    bool isDisplayedWhenStopped() const;

    const QColor &color() const
    {
        return m_color;
    }

    virtual QSize sizeHint() const;
    int heightForWidth(int w) const;
public slots:

    void startAnimation();

    void stopAnimation();

    void setAnimationDelay(int delay);

    void setDisplayedWhenStopped(bool state);

    void setColor(const QColor &color);

protected:
    virtual void timerEvent(QTimerEvent *event);
    virtual void paintEvent(QPaintEvent *event);

private:
    int m_angle;
    int m_timerId;
    int m_delay;
    bool m_displayedWhenStopped;
    QColor m_color;
};

#endif // QPROGRESSINDICATOR_H
