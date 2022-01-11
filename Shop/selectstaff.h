#ifndef SELECTSTAFF_H
#define SELECTSTAFF_H

#include <QDialog>

namespace Ui {
class SelectStaff;
}

class Working;
class C5User;

class SelectStaff : public QDialog
{
    Q_OBJECT

public:
    explicit SelectStaff(QWidget *parent = nullptr);

    ~SelectStaff();

    C5User *fUser;

private slots:
    void on_leNum_returnPressed();

private:
    Ui::SelectStaff *ui;

    Working *fWorking;
};

#endif // SELECTSTAFF_H
