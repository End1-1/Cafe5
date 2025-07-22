#ifndef CR5GOODSIMAGES_H
#define CR5GOODSIMAGES_H

#include "c5widget.h"

namespace Ui {
class CR5GoodsImages;
}

class CR5GoodsImages : public C5Widget
{
    Q_OBJECT

public:
    explicit CR5GoodsImages(QWidget *parent = nullptr);

    ~CR5GoodsImages();

    virtual QToolBar *toolBar();

    virtual void postProcess();

private:
    Ui::CR5GoodsImages *ui;

    QList<QWidget*> fImages;

private slots:
    void refresh();
};

#endif // CR5GOODSIMAGES_H
