#include "c5goodsimage.h"
#include "ui_c5goodsimage.h"
#include "ce5goods.h"
#include "c5editor.h"
#include "c5database.h"
#include <QBuffer>

C5GoodsImage::C5GoodsImage(QWidget *parent) :
    C5Widget(parent),
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

void C5GoodsImage::showCompressButton(bool b)
{
    ui->btnCompress->setVisible(b);
}

void C5GoodsImage::on_btnEdit_clicked()
{
    int id = property("goodsid").toInt();
    CE5Goods *ep = new CE5Goods();
    C5Editor *e = C5Editor::createEditor(ep, id);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        C5Database db;
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

void C5GoodsImage::on_btnCompress_clicked()
{
    C5Database db;
    db[":f_id"] = property("goodsid");
    db.exec("select f_data from c_goods_images where f_id=:f_id");
    if (db.nextRow()) {
        QByteArray ba = db.getValue("f_data").toByteArray();
        QPixmap pm;
        pm.loadFromData(ba);
        do {
            pm = pm.scaled(pm.width() * 0.8,  pm.height() * 0.8);
            ba.clear();
            QBuffer buff( &ba);
            buff.open(QIODevice::WriteOnly);
            pm.save( &buff, "JPG");
        } while (ba.size() > 100000);
        C5Database db;
        db[":f_id"] =  property("goodsid");
        db.exec("delete from c_goods_images where f_id=:f_id");
        db[":f_id"] = property("goodsid");
        db[":f_image"] = ba;
        db.exec("insert into c_goods_images (f_id, f_data) values (:f_id, :f_image)");
    }
    ui->btnCompress->setVisible(false);
}
