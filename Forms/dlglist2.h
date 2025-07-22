#ifndef DLGLIST2_H
#define DLGLIST2_H

#include "c5dialog.h"

namespace Ui {
class DlgList2;
}

class DlgList2 : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgList2(QWidget *parent = nullptr);
    ~DlgList2();
    static int indexOfList(const QString &title, const QStringList &list);

private slots:
    void on_btnCancel_clicked();

    void on_btnOK_clicked();

private:
    Ui::DlgList2 *ui;
};

#endif // DLGLIST2_H
