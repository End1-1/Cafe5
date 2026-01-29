#include "rabstracteditordialog.h"
#include "ui_rabstracteditordialog.h"
#include "ninterface.h"
#include "rwaiterstationconfig.h"
#include "rwaiterorder.h"
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

void RAbstractEditorDialog::setId(const QJsonValue &id)
{
    mId = id;
}

void RAbstractEditorDialog::showEvent(QShowEvent *e)
{
    C5Dialog::showEvent(e);

    if(e->spontaneous()) {
        return;
    }

    NInterface::query1("/engine/v2/officen/editors/get-item", mUser->mSessionKey, this, {
        {"editor", mEditorName},
        {"id", mId}
    },
    [this](const QJsonObject & jdoc) {
        setWindowTitle(jdoc["title"].toString());
        mWidget->setup(jdoc);
    });
}

void RAbstractEditorDialog::createWidget()
{
    if(mEditorName == "Workstations") {
        mWidget = new RWaiterStationConfig();
    }

    if(mEditorName == "CashSessions") {
        mWidget = new RWaiterOrder();
    }

    if(mWidget) {
        connect(mWidget, &RAbstractSpecialWidget::goAccept, this, &QDialog::accept);
        ui->vl->insertWidget(0, mWidget);
    }

    adjustSize();
}

void RAbstractEditorDialog::on_btnCancel_clicked()
{
    reject();
}

void RAbstractEditorDialog::on_btnSave_clicked()
{
    mWidget->save();
}
