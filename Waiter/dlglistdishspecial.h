#ifndef DLGLISTDISHSPECIAL_H
#define DLGLISTDISHSPECIAL_H

#include "c5dialog.h"

namespace Ui {
class DlgListDishSpecial;
}

class QListWidgetItem;

class DlgListDishSpecial : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgListDishSpecial(const QStringList &dbParams);
    ~DlgListDishSpecial();
    static QString getSpecial(const QString &dish, const QStringList &dbParams);

private slots:
    void on_lst_itemClicked(QListWidgetItem *item);

private:
    Ui::DlgListDishSpecial *ui;
    void loadComments(const QString &dish);
    QString fResult;
};

#endif // DLGLISTDISHSPECIAL_H
