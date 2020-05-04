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
    void on_btnView_clicked();

    void on_btnAccept_clicked();

private:
    Ui::StoreInput *ui;

    void getList();
};

#endif // STOREINPUT_H
