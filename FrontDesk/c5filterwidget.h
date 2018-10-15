#ifndef C5FILTERWIDGET_H
#define C5FILTERWIDGET_H

#include <QWidget>

class C5FilterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit C5FilterWidget(QWidget *parent = 0);

    virtual QString condition() = 0;

signals:

public slots:
};

#endif // C5FILTERWIDGET_H
