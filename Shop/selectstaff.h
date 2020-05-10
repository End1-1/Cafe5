#ifndef SELECTSTAFF_H
#define SELECTSTAFF_H

#include <QDialog>

namespace Ui {
class SelectStaff;
}

class SelectStaff : public QDialog
{
    Q_OBJECT

public:
    explicit SelectStaff(QWidget *parent = nullptr);
    ~SelectStaff();

private:
    Ui::SelectStaff *ui;
};

#endif // SELECTSTAFF_H
