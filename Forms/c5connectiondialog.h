#ifndef C5CONNECTIONDIALOG_H
#define C5CONNECTIONDIALOG_H

#include <QDialog>

namespace Ui
{
class C5ConnectionDialog;
}

class C5ConnectionDialog : public QDialog
{
    Q_OBJECT
public:
    enum ConnectionType {noneSecure = 0, Secure };

    explicit C5ConnectionDialog(QWidget *parent = nullptr);

    ~C5ConnectionDialog();

    static void showSettings(QWidget *parent);

    static C5ConnectionDialog* instance();

    QString serverAddress();

    QString serverKey();

    int  connectionType();

private slots:
    void on_btnSave_clicked();

    void on_btnCancel_clicked();

    void on_btnShowPassword_clicked(bool checked);

private:
    Ui::C5ConnectionDialog* ui;

    static C5ConnectionDialog* mInstance;
};

#endif // C5CONNECTIONDIALOG_H
