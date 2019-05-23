#ifndef DLGLISTOFDISHCOMMENTS_H
#define DLGLISTOFDISHCOMMENTS_H

#include <QDialog>

namespace Ui {
class DlgListOfDishComments;
}

class QTableWidgetItem;

class DlgListOfDishComments : public QDialog
{
    Q_OBJECT

public:
    explicit DlgListOfDishComments(QWidget *parent = nullptr);

    ~DlgListOfDishComments();

    static bool getComment(QString &comment);

private slots:
    void kbdAccept();

    void on_btnCancel_clicked();

    void on_tbl_itemClicked(QTableWidgetItem *item);

private:
    Ui::DlgListOfDishComments *ui;

    QString fResult;
};

#endif // DLGLISTOFDISHCOMMENTS_H
