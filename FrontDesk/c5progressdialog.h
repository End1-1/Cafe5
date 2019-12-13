#ifndef C5PROGRESSDIALOG_H
#define C5PROGRESSDIALOG_H

#include <QDialog>

namespace Ui {
class C5ProgressDialog;
}

class C5ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit C5ProgressDialog(QWidget *parent = nullptr);

    ~C5ProgressDialog();

    void setMax(int max);

    virtual int exec() override;

public slots:
    void updateProgressValue(int value);

private:
    Ui::C5ProgressDialog *ui;

    int fMax;

};

#endif // C5PROGRESSDIALOG_H
