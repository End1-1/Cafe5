#include "userphoto.h"
#include "ui_userphoto.h"

UserPhoto::UserPhoto(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserPhoto)
{
    ui->setupUi(this);
}

UserPhoto::~UserPhoto()
{
    delete ui;
}

void UserPhoto::setImage(const QPixmap &p, const QString &name, const QString &scancode)
{
    ui->lbImage->setPixmap(p.scaled(ui->lbImage->size(), Qt::KeepAspectRatio));
    ui->lbName->setText(name);
    ui->lbNum->setText(scancode);
}
