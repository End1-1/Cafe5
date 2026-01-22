#include "rabstracteditordialog.h"
#include "ui_rabstracteditordialog.h"
#include "ninterface.h"
#include "rwaiterstationconfig.h"
#include "c5user.h"
#include <QShowEvent>

RAbstractEditorDialog::RAbstractEditorDialog(C5User *user, const QString &editorName)
    : C5Dialog(user), ui(new Ui::RAbstractEditorDialog),
      mEditorName(editorName)
{
    ui->setupUi(this);
    createWidget();
}

RAbstractEditorDialog::~RAbstractEditorDialog() { delete ui; }

void RAbstractEditorDialog::showEvent(QShowEvent *e)
{
    C5Dialog::showEvent(e);

    if(e->spontaneous()) {
        return;
    }

    NInterface::query1("/engine/v2/officen/editors/get-item", mUser->mSessionKey, this, {{"editor", mEditorName}},
    [this](const QJsonObject & jdoc) {
        setWindowTitle(jdoc["title"].toString());
    });
}

void RAbstractEditorDialog::createWidget()
{
    QWidget *w = nullptr;

    if(mEditorName == "Workstations") {
        w = new RWaiterStationConfig();
    }

    if(w) {
        ui->vl->insertWidget(0, w);
    }

    adjustSize();
}
