#ifndef DLGSTARTSTOPHOURLY_H
#define DLGSTARTSTOPHOURLY_H

#include <QDateTime>
#include <QDialog>

namespace Ui {
class DlgStartStopHourly;
}

class DlgStartStopHourly : public QDialog
{
    Q_OBJECT

public:
    explicit DlgStartStopHourly(QWidget *parent = nullptr);

    ~DlgStartStopHourly();

    QDateTime mDateTime;

    int mOption = 0;

private slots:
    void on_btnStop_clicked();

    void on_btnCancel_clicked();

    void on_btnSetEnd_clicked();

private:
    Ui::DlgStartStopHourly *ui;
};

#endif // DLGSTARTSTOPHOURLY_H
