#ifndef C5FILTERWIDGET_H
#define C5FILTERWIDGET_H

#include "c5utils.h"
#include "c5widget.h"

class C5FilterWidget : public C5Widget
{
    Q_OBJECT

public:
    explicit C5FilterWidget(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QString condition() = 0;

    virtual void saveFilter(QWidget *parent);

    virtual void restoreFilter(QWidget *parent);

signals:

public slots:
};

#endif // C5FILTERWIDGET_H
