#ifndef C5GOODSIMAGE_H
#define C5GOODSIMAGE_H

#include "c5widget.h"

namespace Ui {
class C5GoodsImage;
}

class C5GoodsImage : public C5Widget
{
    Q_OBJECT

public:
    explicit C5GoodsImage(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5GoodsImage();

    void setImage(const QPixmap &p, const QString &name, const QString &scancode);

    void showCompressButton(bool b);

private slots:
    void on_btnEdit_clicked();

    void on_btnCompress_clicked();

private:
    Ui::C5GoodsImage *ui;
};

#endif // C5GOODSIMAGE_H
