#ifndef CE5DISHPART1_H
#define CE5DISHPART1_H

#include "ce5editor.h"

namespace Ui
{
class CE5DishPart1;
}

class CE5DishPart1 : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5DishPart1(QWidget *parent = nullptr);

    ~CE5DishPart1();

    virtual QString title();

    virtual QString table();

    virtual void setId(int id) override;

    virtual void clear() override;

private slots:
    void uploadImage();

    void removeImage();

    void on_lbImg_customContextMenuRequested(const QPoint &pos);

private:
    Ui::CE5DishPart1 *ui;
};

#endif // CE5DISHPART1_H
