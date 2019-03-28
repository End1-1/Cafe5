#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>

namespace Ui {
class MainDialog;
}

class UpdateThread;

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MainDialog(QWidget *parent = nullptr);

    ~MainDialog();

private slots:
    void message(const QString &str);

    void download(bool d);

    void downloadProgress(int p);

    void allDone(const QString &str);

    void on_btnCancel_clicked();

    void on_btnUpdate_clicked();

private:
    Ui::MainDialog *ui;

    UpdateThread *fUpdateThread;

signals:
    void exitThread();

};

#endif // MAINDIALOG_H
