#ifndef CR5GOODSMOVEMENTFILTER_H
#define CR5GOODSMOVEMENTFILTER_H

#include <QWidget>

namespace Ui {
class CR5GoodsMovementFilter;
}

class CR5GoodsMovementFilter : public QWidget
{
    Q_OBJECT

public:
    explicit CR5GoodsMovementFilter(QWidget *parent = 0);
    ~CR5GoodsMovementFilter();

private:
    Ui::CR5GoodsMovementFilter *ui;
};

#endif // CR5GOODSMOVEMENTFILTER_H
