#ifndef DLGSTOREUTILS_H
#define DLGSTOREUTILS_H

#include "c5dialog.h"

namespace Ui {
class DlgStoreUtils;
}

class DlgStoreUtils : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgStoreUtils(const QStringList &dbParams);
    ~DlgStoreUtils();

private slots:

    void on_btnWriteAllDocuments_clicked();

private:
    Ui::DlgStoreUtils *ui;
};

#endif // DLGSTOREUTILS_H
