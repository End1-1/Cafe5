#include "rprintaliasesreport.h"
#include <QDialog>
#include "c5message.h"
#include "c5user.h"
#include "dlgprintalias.h"
#include "ninterface.h"
#include "ui_rabstracteditorreport.h"

RPrintAliasesReport::RPrintAliasesReport(const QString &title, QIcon icon, const QString &editorName,
                                         QWidget *parent)
    : RAbstractEditorReport(title, icon, editorName, parent)
{
    setDeferredLoad(true);
}

void RPrintAliasesReport::on_tbl_doubleClicked(const QModelIndex &index)
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

void RPrintAliasesReport::newData()
{
    openEditor(0);
}

void RPrintAliasesReport::removeAction()
{
    const int id = selectedRowId();
    if (id <= 0) {
        return;
    }

    if (C5Message::question(tr("Delete selected print alias?")) != QDialog::Accepted) {
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

void RPrintAliasesReport::openEditor(int id)
{
    DlgPrintAlias dlg(mUser, id, this);
    if (dlg.exec() == QDialog::Accepted) {
        reloadReport();
    }
}

int RPrintAliasesReport::selectedRowId() const
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
