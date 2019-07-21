#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTcpSocket>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_btnConnect_clicked();
    void on_teData_textChanged();
    void on_leAddress_textChanged(const QString &arg1);
    void on_lePort_textChanged(const QString &arg1);
    void on_btnSend_clicked();
    void readyRead();

private:
    Ui::Dialog *ui;
    QTcpSocket fSocket;
    int datasize;
};

#endif // DIALOG_H
