#ifndef C5CONNECTION_H
#define C5CONNECTION_H

#include "c5dialog.h"

namespace Ui {
class C5Connection;
}

class C5Connection : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5Connection(const QStringList &dbParams, QWidget *parent = 0);

    ~C5Connection();

    static void readParams(QList<QByteArray> &params);

    static void writeParams();

    virtual bool preambule();

private slots:
    void on_btnCancel_clicked();

    void on_btnTest_clicked();

    void on_btnSave_clicked();

    void on_btnInit_clicked();

    void on_btnRefreshSettings_clicked();

private:
    Ui::C5Connection *ui;
};

#endif // C5CONNECTION_H
