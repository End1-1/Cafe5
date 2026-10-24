#include "rcashboxesreport.h"
#include <QDialog>
#include "c5message.h"
#include "c5user.h"
#include "dlgcashbox.h"
#include "ninterface.h"
#include "ui_rabstracteditorreport.h"

RCashboxesReport::RCashboxesReport(const QString &title, QIcon icon, const QString &editorName,
                                   QWidget *parent)
    : RAbstractEditorReport(title, icon, editorName, parent)
{
    setDeferredLoad(true);
}

void RCashboxesReport::on_tbl_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    const QModelIndex srcIndex = reportMapViewIndexToSource(index);
    if (!srcIndex.isValid()) {
        return;
    }

    openEditor(reportSourceCellData(srcIndex.row(), 0).toInt());
}

void RCashboxesReport::newData()
{
    openEditor(0);
}

void RCashboxesReport::removeAction()
{
    const int id = selectedRowId();
    if (id <= 0) {
        return;
    }

    if (C5Message::question(tr("Delete selected cashbox?")) != QDialog::Accepted) {
        return;
    }

    NInterface::query(QStringLiteral("/engine/v2/officen/editors/remove"),
                      mUser->mSessionKey,
                      this,
                      {{QStringLiteral("editor"), mEditorName}, {QStringLiteral("f_id"), id}},
                      [this](const QJsonObject &) { reloadReport(); },
                      [](const QJsonObject &) { return false; },
                      true);
}

void RCashboxesReport::openEditor(int id)
{
    DlgCashbox dlg(mUser, id, this);
    if (dlg.exec() == QDialog::Accepted) {
        reloadReport();
    }
}

int RCashboxesReport::selectedRowId() const
{
    const QModelIndex index = ui->tbl->currentIndex();
    if (!index.isValid()) {
        return 0;
    }

    const QModelIndex srcIndex = reportMapViewIndexToSource(index);
    if (!srcIndex.isValid()) {
        return 0;
    }

    return reportSourceCellData(srcIndex.row(), 0).toInt();
}
