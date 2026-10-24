#include "rapikeysreport.h"
#include "dlgapikey.h"
#include <QDialog>

RApiKeysReport::RApiKeysReport(const QString &title, QIcon icon, const QString &editorName,
                               QWidget *parent)
    : RAbstractEditorReport(title, icon, editorName, parent)
{
    setDeferredLoad(true);
}

void RApiKeysReport::on_tbl_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    const QModelIndex srcIndex = reportMapViewIndexToSource(index);
    if (!srcIndex.isValid()) {
        return;
    }

    const QString partner = reportSourceCellData(srcIndex.row(), 0).toString().trimmed();
    if (partner.isEmpty()) {
        return;
    }

    DlgApiKey dlg(mUser, partner, this);
    if (dlg.exec() == QDialog::Accepted) {
        reloadReport();
    }
}
