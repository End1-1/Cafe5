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
    explicit C5Message(QWidget *parent = 0);
    ~C5Message();
    static int error(const QString &errorStr);
    static int info(const QString &infoStr);
    static int question(const QString &questionStr);
private slots:
    void on_btnYes_clicked();

    void on_btnCancel_clicked();

private:
    Ui::C5Message *ui;
    static int showMessage(const QString &text, const QString &color);
};

#endif // C5MESSAGE_H
