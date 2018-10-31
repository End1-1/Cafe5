#ifndef C5FILTERWIDGET_H
#define C5FILTERWIDGET_H

#include "c5utils.h"
#include <QWidget>

class C5FilterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit C5FilterWidget(const QStringList &dbParams, QWidget *parent = 0);

    virtual QString condition() = 0;

protected:
    QStringList fDBParams;

signals:

public slots:
};

#endif // C5FILTERWIDGET_H
