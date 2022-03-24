#ifndef MONITOR_H
#define MONITOR_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Monitor; }
QT_END_NAMESPACE

class Monitor : public QDialog
{
    Q_OBJECT

public:
    Monitor(QWidget *parent = nullptr);
    ~Monitor();

private slots:
    void on_btnSave_clicked();
    void on_btnCheckDBConnection_clicked();
    void on_btnCreateDatabases_clicked();
    void on_btnSetMariaDBInstallationPath_clicked();
    void on_btnRegister_clicked();

private:
    Ui::Monitor *ui;
    void readLicense();
};
#endif // MONITOR_H
