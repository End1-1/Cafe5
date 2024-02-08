#include "imageloader.h"
#include "c5database.h"
#include "c5config.h"
#include <QThread>
#include <QPixmap>

ImageLoader::ImageLoader(int id, QObject *parent) :
    QObject(parent),
    fId(id)
{

}

void ImageLoader::start()
{
    QThread *t = new QThread();
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
    connect(t, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(t, SIGNAL(started()), this, SLOT(loadImage()));
    connect(this, SIGNAL(finished()), t, SLOT(quit()));
    setParent(0);
    moveToThread(t);
    t->start();
}

void ImageLoader::loadImage()
{
    C5Database db(__c5config.dbParams());
    db[":f_id"] = fId;
    db.exec("select * from c_goods_images where f_id=:f_id");
    if (db.nextRow()) {
        QPixmap p;
        if (p.loadFromData(db.getValue("f_data").toByteArray())) {
            emit imageLoaded(p);
        } else {
            emit noImage();
        }
    } else {
        emit noImage();
    }
    emit finished();
}
