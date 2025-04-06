#ifndef DLGSERVERCONNECTION_H
#define DLGSERVERCONNECTION_H

#include <QDialog>

namespace Ui
{
class DlgServerConnection;
}

class DlgServerConnection : public QDialog
{
    Q_OBJECT

public:
    explicit DlgServerConnection(QWidget *parent = nullptr);

    ~DlgServerConnection();

    static void showSettings(QWidget *parent);

private slots:
    void on_btnShowPassword_clicked(bool checked);

    void on_btnSave_clicked();

    void on_btnCancel_clicked();

    void on_btnGetDatabases_clicked();

private:
    Ui::DlgServerConnection *ui;
};

#endif // DLGSERVERCONNECTION_H
