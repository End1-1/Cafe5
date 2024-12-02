#include "ce5dishpart1.h"
#include "ui_ce5dishpart1.h"
#include <QMenu>
#include <QFileDialog>
#include <QBuffer>

CE5DishPart1::CE5DishPart1(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5DishPart1)
{
    ui->setupUi(this);
}

CE5DishPart1::~CE5DishPart1()
{
    delete ui;
}

QString CE5DishPart1::title()
{
    return tr("Dish department");
}

QString CE5DishPart1::table()
{
    return "d_part1";
}

void CE5DishPart1::setId(int id)
{
    CE5Editor::setId(id);
    C5Database db(fDBParams);
    if (ui->leImageUUID->text().isEmpty()) {
        ui->leImageUUID->setText(C5Database::uuid());
    }
    db[":f_id"] = ui->leImageUUID->text();
    db.exec("select * from s_images where f_id=:f_id");
    if (db.nextRow()) {
        QString base64 = db.getString("f_data");
        if (!base64.isEmpty()) {
            QPixmap pix;
            QByteArray ba = QByteArray::fromBase64(base64.toLatin1());
            pix.loadFromData(ba);
            ui->lbImg->setPixmap(pix.scaled(ui->lbImg->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}

void CE5DishPart1::clear()
{
    CE5Editor::clear();
    ui->lbImg->setText(tr("Right click to select image"));
    ui->leImageUUID->setText(C5Database::uuid());
}

void CE5DishPart1::uploadImage()
{
    if (ui->leCode->getInteger() == 0) {
        if (C5Message::question(tr("You should to save before upload an image")) ==  QDialog::Accepted) {
            QString err;
            QList<QMap<QString, QVariant> > data;
            if (!save(err, data)) {
                C5Message::error(err);
                return;
            }
        } else {
            return;
        }
    }
    QString fn = QFileDialog::getOpenFileName(this, tr("Image"), "", "*.jpg;*.png;*.bmp");
    if (fn.isEmpty()) {
        return;
    }
    QPixmap pm;
    if (!pm.load(fn)) {
        C5Message::error(tr("Could not load image"));
        return;
    }
    ui->lbImg->setPixmap(pm.scaled(ui->lbImg->size(), Qt::KeepAspectRatio));
    QByteArray ba;
    do {
        pm = pm.scaled(pm.width() * 0.8,  pm.height() * 0.8);
        ba.clear();
        QBuffer buff( &ba);
        buff.open(QIODevice::WriteOnly);
        pm.save( &buff, "JPG");
    } while (ba.size() > 100000 );
    C5Database db(fDBParams);
    db[":f_id"] = ui->leImageUUID->text();
    db.exec("delete from s_images where f_id=:f_id");
    db[":f_id"] = ui->leImageUUID->text();
    db[":f_image"] = QString(ba.toBase64());
    db.exec("insert into s_images (f_id, f_data) values (:f_id, :f_image)");
}

void CE5DishPart1::removeImage()
{
    if (C5Message::question(tr("Remove image")) !=  QDialog::Accepted) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_id"] = ui->leImageUUID->text();
    db.exec("delete from s_images where f_id=:f_id");
    ui->lbImg->setText(tr("Image"));
}

void CE5DishPart1::on_lbImg_customContextMenuRequested(const QPoint &pos)
{
    QMenu *m = new QMenu(this);
    m->addAction(QIcon(":/new.png"), tr("Upload image"), this, SLOT(uploadImage()));
    m->addAction(QIcon(":/delete.png"), tr("Remove image"), this, SLOT(removeImage()));
    m->popup(ui->lbImg->mapToGlobal(pos));
}
