#ifndef DQTY_H
#define DQTY_H

#include <QDialog>

namespace Ui {
class DQty;
}

class DQty : public QDialog
{
    Q_OBJECT

public:
    explicit DQty(QWidget *parent = 0);
    ~DQty();
    static double getQty(QWidget *parent = 0);

private slots:
    void on_leQty_returnPressed();

private:
    Ui::DQty *ui;
};

#endif // DQTY_H
