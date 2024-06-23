#ifndef DQTY_H
#define DQTY_H

#include <QDialog>

namespace Ui
{
class DQty;
}

class DQty : public QDialog
{
    Q_OBJECT

public:
    explicit DQty(QWidget *parent = 0);
    ~DQty();
    static double getQty(const QString &title, double max = 0, QWidget *parent = 0);

private slots:
    void on_leQty_returnPressed();

    void on_leQty_textChanged(const QString &arg1);

private:
    Ui::DQty *ui;

    double mMax;
};

#endif // DQTY_H
