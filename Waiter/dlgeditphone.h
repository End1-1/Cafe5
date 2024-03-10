#ifndef DLGEDITPHONE_H
#define DLGEDITPHONE_H

#include <QDialog>

namespace Ui {
class DlgEditPhone;
}

class DlgEditPhone : public QDialog
{
    Q_OBJECT

public:
    explicit DlgEditPhone(QWidget *parent = nullptr);
    ~DlgEditPhone();

private:
    Ui::DlgEditPhone *ui;
};

#endif // DLGEDITPHONE_H
