#include "ce5dishpart2.h"
#include "ui_ce5dishpart2.h"
#include "c5cache.h"
#include "ce5dishpart1.h"
#include <QColorDialog>
#include <QMenu>
#include <QFileDialog>
#include <QBuffer>

CE5DishPart2::CE5DishPart2(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5DishPart2)
{
    ui->setupUi(this);
    ui->leDept->setSelector(dbParams, ui->leDeptName, cache_dish_part1);
    ui->leParent->setSelector(dbParams, ui->leParentName, cache_dish_part2);
    ui->lePosition->setSelector(dbParams, ui->lePositionName, cache_users_groups);
    connect(ui->leColor, SIGNAL(doubleClicked()), this, SLOT(selectColor()));
}

CE5DishPart2::~CE5DishPart2()
{
    delete ui;
}

QString CE5DishPart2::title()
{
    return tr("Dish type");
}

QString CE5DishPart2::table()
{
    return "d_part2";
}

bool CE5DishPart2::checkData(QString &err)
{
    CE5Editor::checkData(err);
    if (ui->leParent->getInteger()) {
        if (ui->leParent->getInteger() == ui->leCode->getInteger()) {
            err += "<br>" + tr("Cannot parent itself");
        }
    }
    return err.isEmpty();
}

void CE5DishPart2::setId(int id)
{
    CE5Editor::setId(id);
    C5Database db(fDBParams);
    db[":f_id"] = ui->leImageUUID->text();
    db.exec("select * from s_images where f_id=:f_id");
    if (db.nextRow()) {
        QString base64 = db.getString("f_data");
        if (!base64.isEmpty()) {
            QPixmap pix;
            pix.loadFromData(QByteArray::fromBase64(base64.toLatin1()));
            ui->lbImg->setPixmap(pix.scaled(ui->lbImg->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}

void CE5DishPart2::clear()
{
    CE5Editor::clear();
    ui->lbImg->setText(tr("Right click to select image"));
    ui->leImageUUID->setText(C5Database::uuid());
}

void CE5DishPart2::selectColor()
{
    QColor initColor = QColor::fromRgb(ui->leColor->color());
    int color = QColorDialog::getColor(initColor, this, tr("Background color")).rgb();
    ui->leColor->setColor(color);
    ui->leColor->setInteger(color);
}

void CE5DishPart2::on_btnNewPart_clicked()
{
    CE5DishPart1 *ep = new CE5DishPart1(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        ui->leDept->setValue(data.at(0)["f_id"].toString());
    }
    delete e;
}

void CE5DishPart2::on_lbImg_customContextMenuRequested(const QPoint &pos)
{
    QMenu *m = new QMenu(this);
    m->addAction(QIcon(":/new.png"), tr("Upload image"), this, SLOT(uploadImage()));
    m->addAction(QIcon(":/delete.png"), tr("Remove image"), this, SLOT(removeImage()));
    m->popup(ui->lbImg->mapToGlobal(pos));
}

void CE5DishPart2::uploadImage()
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
        if (!ui->chDonNotResize->isChecked()) {
            pm = pm.scaled(pm.width() * 0.8,  pm.height() * 0.8);
        }
        ba.clear();
        QBuffer buff(&ba);
        buff.open(QIODevice::WriteOnly);
        pm.save(&buff, "PNG");
    } while (ba.size() > 100000 && !ui->chDonNotResize->isChecked());

    C5Database db(fDBParams);
    db[":f_id"] = ui->leImageUUID->text();
    db.exec("delete from s_images where f_id=:f_id");
    db[":f_id"] = ui->leImageUUID->text();
    db[":f_image"] = ba.toBase64();
    db.exec("insert into s_images (f_id, f_data) values (:f_id, :f_image)");
}

void CE5DishPart2::removeImage()
{
    if (C5Message::question(tr("Remove image")) !=  QDialog::Accepted) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_id"] = ui->leImageUUID->text();
    db.exec("delete from s_images where f_id=:f_id");
    ui->lbImg->setText(tr("Image"));
}
