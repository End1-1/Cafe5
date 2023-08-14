#ifndef DLGLIST2_H
#define DLGLIST2_H

#include "c5dialog.h"

namespace Ui {
class DlgList;
}

class DlgList2 : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgList2(const QStringList &dbParams, QWidget *parent = nullptr);
    ~DlgList2();
    static int indexOfList(const QString &title, const QStringList &dbParams, const QStringList &list);

private slots:
    void on_btnCancel_clicked();

    void on_btnOK_clicked();

private:
    Ui::DlgList *ui;
};

#endif // DLGLIST2_H
