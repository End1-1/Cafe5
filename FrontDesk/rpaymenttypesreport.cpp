#include "rpaymenttypesreport.h"
#include "dlgpaymenttype.h"
#include <QDialog>

RPaymentTypesReport::RPaymentTypesReport(const QString &title, QIcon icon, const QString &editorName,
                                         QWidget *parent)
    : RAbstractEditorReport(title, icon, editorName, parent)
{
    setDeferredLoad(true);
}

void RPaymentTypesReport::on_tbl_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    const QModelIndex srcIndex = reportMapViewIndexToSource(index);
    if (!srcIndex.isValid()) {
        return;
    }

    DlgPaymentType dlg(mUser, reportSourceCellData(srcIndex.row(), 0).toInt(), this);
    if (dlg.exec() == QDialog::Accepted) {
        reloadReport();
    }
}
