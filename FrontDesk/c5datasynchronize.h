#ifndef C5DATASYNCHRONIZE_H
#define C5DATASYNCHRONIZE_H

#include "c5dialog.h"

namespace Ui {
class C5DataSynchronize;
}

class C5DataSynchronize : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5DataSynchronize(const QStringList &dbParams, QWidget *parent = 0);

    ~C5DataSynchronize();

private slots:
    void on_btnStartUpload_clicked();

    void statusUpdate(const QString &name, int row, const QString &msg);

    void progressBarMax(int max);

    void progressBarValue(int value);

    void dstFinished();

private:
    Ui::C5DataSynchronize *ui;

    int fThreadsCount;
};

#endif // C5DATASYNCHRONIZE_H
