#ifndef DLGLISTOFSHIFTS_H
#define DLGLISTOFSHIFTS_H

#include <QDialog>

namespace Ui {
class DlgListOfShifts;
}

class DlgListOfShifts : public QDialog
{
    Q_OBJECT

public:
    explicit DlgListOfShifts(QWidget *parent = nullptr);

    ~DlgListOfShifts();

    static bool getShift(int &id, QString &name);

private slots:
    void on_lst_clicked(const QModelIndex &index);

private:
    Ui::DlgListOfShifts *ui;

    int fShiftId;

    QString fShiftName;
};

#endif // DLGLISTOFSHIFTS_H
