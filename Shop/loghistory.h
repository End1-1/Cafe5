#ifndef LOGHISTORY_H
#define LOGHISTORY_H

#include <QDialog>
#include <QDate>

namespace Ui {
class LogHistory;
}

class LogHistory : public QDialog
{
    Q_OBJECT

public:
    explicit LogHistory(QWidget *parent = nullptr);

    ~LogHistory();

private slots:
    void on_btnNext_clicked();

    void on_btnPreviouse_clicked();

private:
    Ui::LogHistory *ui;

    QDate mDate;

    void loadHistory();
};

#endif // LOGHISTORY_H
