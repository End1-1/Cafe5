#include "cr5goodsimages.h"
#include "ui_cr5goodsimages.h"
#include "c5goodsimage.h"
#include "c5database.h"

CR5GoodsImages::CR5GoodsImages(QWidget *parent) :
    C5Widget(parent),
    ui(new Ui::CR5GoodsImages)
{
    ui->setupUi(this);
    fLabel = tr("Images");
    fIcon = ":/images.png";
}

CR5GoodsImages::~CR5GoodsImages()
{
    delete ui;
}

QToolBar *CR5GoodsImages::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        createStandartToolbar(btn);
    }
    return fToolBar;
}

void CR5GoodsImages::postProcess()
{
    foreach (QWidget *w, fImages) {
        w->deleteLater();
    }
    fImages.clear();
    int r = 0, c = 0;
    C5Database db;
    db.exec("select g.f_id, g.f_name, gi.f_data, g.f_scancode "
            "from c_goods_images gi "
            "inner join c_goods g on g.f_id=gi.f_id "
            "order by 1 ");
    while (db.nextRow()) {
        C5GoodsImage *gi = new C5GoodsImage();
        gi->setProperty("goodsid", db.getString("f_id"));
        fImages.append(gi);
        QPixmap p;
        QByteArray ba = db.getValue("f_data").toByteArray();
        p.loadFromData(ba);
        gi->setImage(p.scaled(190, 190, Qt::KeepAspectRatio), db.getString("f_name"), db.getString("f_scancode"));
        gi->showCompressButton(ba.size() > 100000);
        ui->gl->addWidget(gi, r, c);
        c++;
        if (c > 4) {
            c = 0;
            r++;
        }
    }
}

void CR5GoodsImages::refresh()
{
    postProcess();
}
