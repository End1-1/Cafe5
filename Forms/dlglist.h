#ifndef DLGLIST_H
#define DLGLIST_H

#include "c5dialog.h"

namespace Ui {
class DlgList;
}

class DlgList : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgList(const QStringList &dbParams, QWidget *parent = nullptr);
    ~DlgList();
    static int indexOfList(const QString &title, const QStringList &dbParams, const QStringList &list);

private slots:
    void on_btnCancel_clicked();

    void on_btnOK_clicked();

private:
    Ui::DlgList *ui;
};

#endif // DLGLIST_H
