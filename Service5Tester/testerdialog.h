#ifndef TESTERDIALOG_H
#define TESTERDIALOG_H

#include <QDialog>
#include <QSslSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class TesterDialog; }
QT_END_NAMESPACE

class SocketConnection;

class TesterDialog : public QDialog
{
    Q_OBJECT

public:
    TesterDialog(QWidget *parent = nullptr);
    ~TesterDialog();


private slots:
    void on_btnSelectClientFile_clicked();

    void on_btnClientLogin_clicked();

    void on_btnDriverRoute_clicked();

private:
    Ui::TesterDialog *ui;

signals:
    void driveRoute(SocketConnection*, const QString&);

};
#endif // TESTERDIALOG_H
