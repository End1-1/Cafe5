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
    explicit DlgList(C5User *user);

    ~DlgList();
    
    static bool getValue(C5User *user, const QStringList &names, int &index);

private slots:
    void on_lst_clicked(const QModelIndex &index);

private:
    Ui::DlgList *ui;

    int fResult;
};

#endif // DLGLIST_H
