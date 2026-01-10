#ifndef DLGLISTOFDISHCOMMENTS_H
#define DLGLISTOFDISHCOMMENTS_H

#include "c5dialog.h"

namespace Ui
{
class DlgListOfDishComments;
}

class QTableWidgetItem;

class DlgListOfDishComments : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgListOfDishComments(const QString &path);

    ~DlgListOfDishComments();

    static bool getComment(const QString &caption, QString &comment);

    static bool getText(const QString &caption, const QString &valuesPath, QString &comment);

    virtual void showEvent(QShowEvent *e) override;

private slots:
    void kbdAccept();

    void on_tbl_itemClicked(QTableWidgetItem *item);

private:
    Ui::DlgListOfDishComments* ui;

    QString fResult;
};

#endif // DLGLISTOFDISHCOMMENTS_H
