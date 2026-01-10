#ifndef DLGDIRTYSTOREDOC_H
#define DLGDIRTYSTOREDOC_H

#include "c5dialog.h"

namespace Ui {
class DlgDirtyStoreDoc;
}

class DlgDirtyStoreDoc : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgDirtyStoreDoc(C5User *user, const QString &uuid);
    ~DlgDirtyStoreDoc();

private slots:
    void on_btnEdit_clicked();

    void on_tbl_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void on_btnRemove_clicked();

    void on_btnSave_clicked();

private:
    Ui::DlgDirtyStoreDoc *ui;

    QString fUuid;

    void countTotal();

    void correctDocumentAmount();
};

#endif // DLGDIRTYSTOREDOC_H
