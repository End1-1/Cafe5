#ifndef STOREINPUT_H
#define STOREINPUT_H

#include <QDialog>

namespace Ui {
class StoreInput;
}

class StoreInput : public QDialog
{
    Q_OBJECT

public:
    explicit StoreInput(QWidget *parent = nullptr);

    ~StoreInput();

private slots:
    void checkboxCheck(bool v);

    void on_btnView_clicked();

    void on_btnAccept_clicked();

    void on_btnDateLeft_clicked();

    void on_btnDateRight_clicked();

    void on_btnRefresh_clicked();

    void on_btnAcceptMode_clicked();

    void on_btnHistoryMode_clicked();

    void on_leFilter_textChanged(const QString &arg1);

    void on_chAll_clicked(bool checked);

    void on_btnStore_clicked();

    void on_btnStoreGoods_clicked();

private:
    Ui::StoreInput *ui;

    int fViewMode;

    void getList();

    void history();

    void storeByGroup();

    void storeByItems();

    void changeDate(int d);

    void refresh();

    void disableMenuButtons(QPushButton *b);
};

#endif // STOREINPUT_H
