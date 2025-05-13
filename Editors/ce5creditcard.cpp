#include "ce5creditcard.h"
#include "ui_ce5creditcard.h"
#include "c5database.h"
#include <QFileDialog>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QBuffer>

CE5CreditCard::CE5CreditCard(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5CreditCard)
{
    ui->setupUi(this);
}

CE5CreditCard::~CE5CreditCard()
{
    delete ui;
}

void CE5CreditCard::setId(int id)
{
    CE5Editor::setId(id);
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select f_image from o_credit_card where f_id=:f_id");
    if (db.nextRow()) {
        QString base64 = db.getString(0);
        if (!base64.isEmpty()) {
            QPixmap p;
            p.loadFromData(QByteArray::fromBase64(base64.toLatin1()), "PNG");
            if (!ui->gv->scene()) {
                ui->gv->setScene(new QGraphicsScene());
            }
            ui->gv->scene()->clear();
            ui->gv->scene()->addPixmap(p);
        }
    }
}

void CE5CreditCard::clear()
{
    CE5Editor::clear();
    if (ui->gv->scene()) {
        ui->gv->scene()->clear();
    }
}

bool CE5CreditCard::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    if (!CE5Editor::save(err, data)) {
        return false;
    }
    QList<QGraphicsItem *> ol = ui->gv->scene()->items();
    if (ol.count() > 0) {
        QGraphicsPixmapItem *p = static_cast<QGraphicsPixmapItem *>(ol.at(0));
        QByteArray ba;
        QBuffer buff( &ba);
        p->pixmap().toImage().save( &buff, "PNG");
        QString str = ba.toBase64().data();
        C5Database db(fDBParams);
        db[":f_image"] = str;
        db.update(table(), where_id(ui->leCode->getInteger()));
    }
    return true;
}

void CE5CreditCard::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select image"), "", "*.png");
    if (fileName.isEmpty()) {
        return;
    }
    QPixmap p;
    p.load(fileName);
    ui->gv->setScene(new QGraphicsScene());
    ui->gv->scene()->addPixmap(p);
}
