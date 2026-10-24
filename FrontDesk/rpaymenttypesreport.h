#pragma once

#include "rabstracteditorreport.h"

class RPaymentTypesReport : public RAbstractEditorReport
{
    Q_OBJECT

public:
    explicit RPaymentTypesReport(const QString &title, QIcon icon, const QString &editorName,
                                 QWidget *parent = nullptr);

protected:
    void on_tbl_doubleClicked(const QModelIndex &index) override;
};
