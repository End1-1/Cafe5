#ifndef DLGDISHREMOVEREASON_H
#define DLGDISHREMOVEREASON_H

#include <QDialog>

namespace Ui {
class DlgDishRemoveReason;
}

class DlgDishRemoveReason : public QDialog
{
    Q_OBJECT

public:
    explicit DlgDishRemoveReason(QWidget *parent = nullptr);

    ~DlgDishRemoveReason();

    static bool getReason(QString &reason, int &state);

private slots:
    void kbdAccept();

    void textChanged(const QString &text);

    void on_btnMistake_clicked();

    void on_btnVoid_clicked();

private:
    Ui::DlgDishRemoveReason *ui;

    int fState;

    QString fName;
};

#endif // DLGDISHREMOVEREASON_H
