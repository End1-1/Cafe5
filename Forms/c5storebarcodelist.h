#ifndef C5STOREBARCODELIST_H
#define C5STOREBARCODELIST_H

#include <QDialog>

namespace Ui {
class C5StoreBarcodeList;
}

class C5StoreBarcodeList : public QDialog
{
    Q_OBJECT

public:
    explicit C5StoreBarcodeList(QWidget *parent = nullptr);

    ~C5StoreBarcodeList();

    static bool getList(int &r1, int &r2);

private slots:
    void on_btnReject_clicked();

    void on_btnOK_clicked();

private:
    Ui::C5StoreBarcodeList *ui;
};

#endif // C5STOREBARCODELIST_H
