#ifndef C5FILTERWIDGET_H
#define C5FILTERWIDGET_H

#include "c5utils.h"
#include "c5widget.h"

class C5FilterWidget : public C5Widget
{
    Q_OBJECT

public:
    explicit C5FilterWidget(const QStringList &dbParams, QWidget *parent = 0);

    virtual QString condition() = 0;

signals:

public slots:
};

#endif // C5FILTERWIDGET_H
