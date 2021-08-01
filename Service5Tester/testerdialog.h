#ifndef TESTERDIALOG_H
#define TESTERDIALOG_H

#include <QDialog>
#include <QSslSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class TesterDialog; }
QT_END_NAMESPACE

class TesterDialog : public QDialog
{
    Q_OBJECT

public:
    TesterDialog(QWidget *parent = nullptr);
    ~TesterDialog();

private slots:
    void n1Finished();
    void n1Error(int code, const QString &msg);
    void data(int code, const QVariant &d);
    void on_btnTestN1_clicked();
    void on_btnTestN2_clicked();

    void on_pushButton_clicked();

private:
    Ui::TesterDialog *ui;
    int fTextN1Count;

};
#endif // TESTERDIALOG_H
