#ifndef MFPROCESSPRODUCTPRICEUPDATE_H
#define MFPROCESSPRODUCTPRICEUPDATE_H

#include <QWidget>

namespace Ui {
class MFProcessProductPriceUpdate;
}

class MFProcessProductPriceUpdate : public QWidget
{
    Q_OBJECT

public:
    explicit MFProcessProductPriceUpdate(QWidget *parent = nullptr);
    ~MFProcessProductPriceUpdate();
    QDate date1() const;
    QDate date2() const;

private slots:
    void on_btnUpdatePrices_clicked();

private:
    Ui::MFProcessProductPriceUpdate *ui;

signals:
    void startUpdate();
};

#endif // MFPROCESSPRODUCTPRICEUPDATE_H
