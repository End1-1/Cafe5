#include "selectstaff.h"
#include "ui_selectstaff.h"
#include "working.h"
#include "userphoto.h"
#include "c5message.h"
#include "c5lineedit.h"

#include <QScreen>
#include <QGuiApplication>

SelectStaff::SelectStaff(Working *w, C5User *user) :
    C5Dialog(user),
    ui(new Ui::SelectStaff),
    fWorking(w)
{
    ui->setupUi(this);
    setWindowTitle(tr("Staff"));

    const int cardW = 160;
    const int screenW = QGuiApplication::primaryScreen()
                            ? QGuiApplication::primaryScreen()->availableGeometry().width()
                            : 1280;
    int cols = qMax(1, (screenW - 80) / cardW);

    int r = 0, c = 0;
    for(int i = 0; i < fWorking->fCurrentUsers.count(); i++) {
        const IUser &u = fWorking->fCurrentUsers.at(i);
        UserPhoto *up = new UserPhoto();
        up->setImage(u.photo, u.name, QString::number(i + 1));
        ui->gl->addWidget(up, r, c++);
        if(c >= cols) {
            c = 0;
            r++;
        }
    }

    ui->leNum->setFocus();
    adjustSize();
}

SelectStaff::~SelectStaff()
{
    delete ui;
}

bool SelectStaff::select(Working *w, C5User *user, int &staffId)
{
    if(!w || w->fCurrentUsers.isEmpty()) {
        C5Message::info(tr("No staff checked in"));
        return false;
    }
    SelectStaff dlg(w, user);
    if(dlg.exec() != QDialog::Accepted) {
        return false;
    }
    staffId = dlg.mUserId;
    return staffId > 0;
}

void SelectStaff::on_leNum_returnPressed()
{
    if(ui->leNum->getInteger() < 1 || ui->leNum->getInteger() > fWorking->fCurrentUsers.count()) {
        ui->leNum->clear();
        return;
    }

    const IUser &u = fWorking->fCurrentUsers.at(ui->leNum->getInteger() - 1);
    mUserId = u.id;

    if(mUserId > 0) {
        accept();
    }
}
