#ifndef DLGREGISTERCARD_H
#define DLGREGISTERCARD_H

#include <QDialog>

namespace Ui {
class DlgRegisterCard;
}

class DlgRegisterCard : public QDialog
{
    Q_OBJECT

public:
    explicit DlgRegisterCard(QWidget *parent = nullptr);
    ~DlgRegisterCard();

private slots:
    void on_toolButton_clicked();

    void on_btnCancel_clicked();

    void on_btnRegister_clicked();

private:
    Ui::DlgRegisterCard *ui;
};

#endif // DLGREGISTERCARD_H
