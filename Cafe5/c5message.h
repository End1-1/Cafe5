#ifndef C5MESSAGE_H
#define C5MESSAGE_H

#include <QDialog>

namespace Ui {
class C5Message;
}

class C5Message : public QDialog
{
    Q_OBJECT

public:
    explicit C5Message(QWidget *parent = nullptr);

    ~C5Message();

    static int error(const QString &errorStr, const QString &yes = QObject::tr("Close"), const QString &no = "");

    static int info(const QString &infoStr, const QString &yes = QObject::tr("OK"), const QString &no = "");

    static int question(const QString &questionStr, const QString &yes = QObject::tr("Yes"), const QString &no = QObject::tr("No"));

private slots:
    void on_btnYes_clicked();

    void on_btnCancel_clicked();

private:
    Ui::C5Message *ui;

    static int showMessage(const QString &text, int tp, const QString &yes, const QString &no);
};

#endif // C5MESSAGE_H
