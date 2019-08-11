#ifndef SALES_H
#define SALES_H

#include <QDialog>

namespace Ui {
class Sales;
}

class Sales : public QDialog
{
    Q_OBJECT

public:
    explicit Sales(QWidget *parent = nullptr);

    ~Sales();

    static void showSales(QWidget *parent = nullptr);

private slots:
    void on_btnDateLeft_clicked();

    void on_btnDateRight_clicked();

    void on_btnPrint_clicked();

private:
    Ui::Sales *ui;

    void changeDate(int d);

    void refresh();
};

#endif // SALES_H
