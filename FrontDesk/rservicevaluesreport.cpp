#include "rservicevaluesreport.h"
#include <QDialog>
#include <QTableView>
#include "c5message.h"
#include "c5user.h"
#include "dlgservicevalue.h"
#include "ninterface.h"
#include "ui_rabstracteditorreport.h"

RServiceValuesReport::RServiceValuesReport(const QString &title, QIcon icon, const QString &editorName, QWidget *parent)
    : RAbstractEditorReport(title, icon, editorName, parent)
{
    setDeferredLoad(true);
    connect(ui->tbl, &QTableView::doubleClicked, this, [this](const QModelIndex &index) {
        if(!index.isValid()) {
            return;
        }

        const QModelIndex srcIndex = reportMapViewIndexToSource(index);
        if(!srcIndex.isValid()) {
            return;
        }

        openEditor(reportSourceCellData(srcIndex.row(), 0).toInt());
    });
}

void RServiceValuesReport::newData()
{
    openEditor(0);
}

void RServiceValuesReport::removeAction()
{
    const int id = selectedRowId();
    if(id <= 0) {
        return;
    }

    if(C5Message::question(tr("Delete selected service value?")) != QDialog::Accepted) {
        return;
    }

    NInterface::query(QStringLiteral("/engine/v2/officen/servicevalues/remove"),
                      mUser->mSessionKey,
                      this,
                      {{QStringLiteral("f_id"), id}},
                      [this](const QJsonObject &) { reloadReport(); },
                      [](const QJsonObject &) { return false; },
                      true);
}

void RServiceValuesReport::openEditor(int id)
{
    DlgServiceValue dlg(mUser, id, this);
    if(dlg.exec() == QDialog::Accepted) {
        reloadReport();
    }
}

int RServiceValuesReport::selectedRowId() const
{
    const QModelIndex index = ui->tbl->currentIndex();
    if(!index.isValid()) {
        return 0;
    }

    const QModelIndex srcIndex = reportMapViewIndexToSource(index);
    if(!srcIndex.isValid()) {
        return 0;
    }

    return reportSourceCellData(srcIndex.row(), 0).toInt();
}
