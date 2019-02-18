#ifndef WORKING_H
#define WORKING_H

#include <QDialog>

namespace Ui {
class Working;
}

class Working : public QDialog
{
    Q_OBJECT

public:
    explicit Working(QWidget *parent = 0);

    ~Working();

private:
    Ui::Working *ui;

    void getGoodsList();

private slots:
    void shortcutF1();

    void shortcutF2();

    void shortcutF3();

    void shortcutF4();

    void shortcutDown();

    void shortcutUp();

    void on_btnNewOrder_clicked();

    void on_btnConnection_clicked();

    void on_leCode_returnPressed();

    void on_btnSaveOrder_clicked();

    void on_leCode_textChanged(const QString &arg1);
    void on_btnSaveOrderNoTax_clicked();
};

#endif // WORKING_H
