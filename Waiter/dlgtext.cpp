#include "dlgtext.h"
#include "ui_dlgtext.h"

DlgText::DlgText(C5User *user) :
    C5Dialog(user),
    ui(new Ui::DlgText)
{
    ui->setupUi(this);
    connect(ui->kbd, &RKeyboard::accept, this, &DlgText::accept);
    connect(ui->kbd, &RKeyboard::reject, this, &DlgText::reject);
}

DlgText::~DlgText()
{
    delete ui;
}

bool DlgText::getText(C5User *user, const QString &title, QString &defaultText)
{
    DlgText d(user);
    d.setWindowTitle(title);
    d.ui->kbd->setText(defaultText);

    if(d.exec() == QDialog::Accepted) {
        defaultText = d.ui->kbd->text();
        return true;
    }

    return false;
}
