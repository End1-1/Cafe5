#ifndef CR5CONSUPTIONBYSALESQTY_H
#define CR5CONSUPTIONBYSALESQTY_H

#include <QDialog>

namespace Ui {
class cr5consuptionbysalesqty;
}

class cr5consuptionbysalesqty : public QDialog
{
    Q_OBJECT

public:
    enum ResultType {rtOk, rtCancel, rtAdd, rtClear};
    explicit cr5consuptionbysalesqty(QWidget *parent = nullptr);
    ~cr5consuptionbysalesqty();
    static ResultType qty(QWidget *parent, double &qty);

private slots:
    void on_btnCancel_clicked();

    void on_btnAdd_clicked();

    void on_btnClear_clicked();

    void on_btnOk_clicked();

private:
    Ui::cr5consuptionbysalesqty *ui;
    ResultType fResultType;
};

#endif // CR5CONSUPTIONBYSALESQTY_H
