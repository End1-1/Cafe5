#ifndef DLGLISTOFHALL_H
#define DLGLISTOFHALL_H

#include <QDialog>

namespace Ui {
class DlgListOfHall;
}

class DlgListOfHall : public QDialog
{
    Q_OBJECT

public:
    explicit DlgListOfHall(QWidget *parent = 0);

    ~DlgListOfHall();

    static bool getHall(QString &hall);

private slots:
    void on_lst_clicked(const QModelIndex &index);

private:
    Ui::DlgListOfHall *ui;

    QString fHall;
};

#endif // DLGLISTOFHALL_H
