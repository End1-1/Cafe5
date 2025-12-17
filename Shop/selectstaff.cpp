#include "selectstaff.h"
#include "ui_selectstaff.h"
#include "working.h"
#include "userphoto.h"
#include "c5user.h"
#include "c5config.h"

SelectStaff::SelectStaff(Working *w) :
    C5Dialog(),
    ui(new Ui::SelectStaff),
    fWorking(w)
{
    ui->setupUi(this);
    fUser = new C5User;
    int r = 0, c = 0;

    for(int i = 0; i < fWorking->fCurrentUsers.count(); i++) {
        const IUser &u = fWorking->fCurrentUsers.at(i);
        UserPhoto *up = new UserPhoto();
        up->setImage(u.photo, u.name, QString::number(i + 1));
        ui->gl->addWidget(up, r, c++);

        if(c > 3) {
            c = 0;
            r++;
        }
    }

    adjustSize();
}

SelectStaff::~SelectStaff()
{
    delete ui;
    delete fUser;
}

void SelectStaff::on_leNum_returnPressed()
{
    if(ui->leNum->getInteger() < 1 || ui->leNum->getInteger() > fWorking->fCurrentUsers.count()) {
        ui->leNum->clear();
        return;
    }

    const IUser &u = fWorking->fCurrentUsers.at(ui->leNum->getInteger() - 1);

    if(fUser->loadFromDB(u.id, fHttp)) {
        accept();
    }
}
