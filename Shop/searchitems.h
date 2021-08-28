#ifndef SEARCHITEMS_H
#define SEARCHITEMS_H

#include <QDialog>

namespace Ui {
class SearchItems;
}

class SearchItems : public QDialog
{
    Q_OBJECT

public:
    explicit SearchItems(QWidget *parent = nullptr);
    ~SearchItems();

private slots:
    void on_btnSearch_clicked();

    void on_leCode_returnPressed();

    void on_btnReserve_clicked();

    void on_btnViewReservations_clicked();

    void on_btnViewAllReservations_clicked();

    void on_btnEditReserve_clicked();

private:
    Ui::SearchItems *ui;
};

#endif // SEARCHITEMS_H
