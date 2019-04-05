#ifndef DLGEXITBYVERSION_H
#define DLGEXITBYVERSION_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class DlgExitByVersion;
}

class DlgExitByVersion : public QDialog
{
    Q_OBJECT

public:
    explicit DlgExitByVersion(QWidget *parent = nullptr);
    ~DlgExitByVersion();
    static void exit(const QString &appVersion, const QString &dbVersion);
    static void exit(const QString &msg);
private slots:
    void timeout();
    void on_btnClose_clicked();

private:
    Ui::DlgExitByVersion *ui;
    int fCounter;
    QTimer fTimer;
    void setVersions(const QString &appVersion, const QString &dbVersion);
};

#endif // DLGEXITBYVERSION_H
