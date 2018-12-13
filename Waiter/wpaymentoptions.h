#ifndef WPAYMENTOPTIONS_H
#define WPAYMENTOPTIONS_H

#include <QWidget>

namespace Ui {
class WPaymentOptions;
}

class WPaymentOptions : public QWidget
{
    Q_OBJECT

public:
    explicit WPaymentOptions(QWidget *parent = 0);

    ~WPaymentOptions();

private:
    Ui::WPaymentOptions *ui;

signals:
    void clearClicked();

    void calc();

private slots:
    void on_btnRemove_clicked();

    void on_btnCalc_clicked();
};

#endif // WPAYMENTOPTIONS_H
