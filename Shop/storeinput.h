#ifndef STOREINPUT_H
#define STOREINPUT_H

#include "c5shopdialog.h"

namespace Ui
{
class StoreInput;
}

class C5User;

class StoreInput : public C5ShopDialog
{
    Q_OBJECT

public:
    explicit StoreInput(C5User *user);

    ~StoreInput();

private slots:
    void checkboxCheck(bool v);

    void checkBoxAcceptResponse(const QJsonObject &jdoc);

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

    void on_btnExit_clicked();

    void on_btnMinimize_clicked();

    void on_btnDocs_clicked();

    void on_btnNewMovement_clicked();

private:
    Ui::StoreInput* ui;

    int fViewMode;

    C5User* fUser;

    void getList();

    void history();

    void storeByGroup();

    void storeByItems();

    void docs();

    void changeDate(int d);

    void refresh();

    void disableMenuButtons(QPushButton *b);
};

#endif // STOREINPUT_H
