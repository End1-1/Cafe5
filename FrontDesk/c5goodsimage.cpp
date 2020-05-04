#include "c5goodsimage.h"
#include "ui_c5goodsimage.h"
#include "ce5goods.h"
#include "c5editor.h"

C5GoodsImage::C5GoodsImage(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5GoodsImage)
{
    ui->setupUi(this);
}

C5GoodsImage::~C5GoodsImage()
{
    delete ui;
}

void C5GoodsImage::setImage(const QPixmap &p, const QString &name, const QString &scancode)
{
    ui->lbImage->setPixmap(p);
    ui->lbName->setText(name);
    ui->lbScancode->setText(scancode);
}

void C5GoodsImage::on_btnEdit_clicked()
{
    int id = property("goodsid").toInt();
    CE5Goods *ep = new CE5Goods(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, id);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        C5Database db(fDBParams);
        db[":f_id"] = id;
        db.exec("select g.f_id, g.f_name, gi.f_data, g.f_scancode "
                "from c_goods_images gi "
                "inner join c_goods g on g.f_id=gi.f_id "
                "where g.f_id=:f_id");
        if (db.nextRow()) {
            QPixmap p;
            p.loadFromData(db.getValue("f_data").toByteArray());
            setImage(p.scaled(190, 190, Qt::KeepAspectRatio), db.getString("f_name"), db.getString("f_scancode"));
        }
    }
    delete e;
}
