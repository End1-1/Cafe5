#include "rabstracteditordialog.h"
#include "ui_rabstracteditordialog.h"
#include "ninterface.h"
#include "rworkstationconfigwidget.h"
#include "rwaiterorder.h"
#include "c5message.h"
#include "c5user.h"
#include <QShowEvent>

RAbstractEditorDialog::RAbstractEditorDialog(C5User *user, const QString &editorName)
    : C5Dialog(user),
      ui(new Ui::RAbstractEditorDialog),
      mEditorName(editorName),
      mWidget(nullptr)
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

        if(mEditorName == QStringLiteral("form_workstations") && !mWidget) {
            const int type = jdoc.value(QStringLiteral("f_type")).toInt();
            mWidget = RWorkstationConfigWidget::createForType(type, this, mUser);

            if(!mWidget) {
                C5Message::error(tr("Settings editor is not available for this workstation type"));
                reject();
                return;
            }

            if(auto *ws = qobject_cast<RWorkstationConfigWidget *>(mWidget)) {
                ws->setEditorKey(mEditorName);
            }
            connect(mWidget, &RAbstractSpecialWidget::goAccept, this, &QDialog::accept);
            ui->vl->insertWidget(0, mWidget);
            adjustSize();
        }

        if(!mWidget) {
            return;
        }

        mWidget->setup(jdoc);
    });
}

void RAbstractEditorDialog::createWidget()
{
    if(mEditorName == QStringLiteral("form_workstations")) {
        return;
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
    if(!mWidget) {
        return;
    }

    mWidget->save();
}
