#include "selectstaff.h"
#include "ui_selectstaff.h"
#include "working.h"
#include "userphoto.h"

SelectStaff::SelectStaff(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectStaff)
{
    ui->setupUi(this);
    Working *w = static_cast<Working*>(parent);
    int r = 0, c = 0;
    for (int i = 0; i < w->fCurrentUsers.count(); i++) {
        const IUser &u = w->fCurrentUsers.at(i);
        UserPhoto *up = new UserPhoto();
        up->setImage(u.photo, u.name, QString::number(i + 1));
        ui->gl->addWidget(up, r, c++);
        if (c > 3) {
            c = 0;
            r++;
        }
    }
    adjustSize();
}

SelectStaff::~SelectStaff()
{
    delete ui;
}
