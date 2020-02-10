#ifndef CR5GOODSFILTER_H
#define CR5GOODSFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5GoodsFilter;
}

class CR5GoodsFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5GoodsFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5GoodsFilter();

    virtual QString condition();

private:
    Ui::CR5GoodsFilter *ui;

    void addCond(QString &w, const QString &cond);
};

#endif // CR5GOODSFILTER_H
