#ifndef C5CONNECTION_H
#define C5CONNECTION_H

#include "c5dialog.h"
#include <QJsonObject>

namespace Ui {
class C5Connection;
}

class C5Connection : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5Connection(const QJsonObject &params);

    ~C5Connection();
    virtual bool preambule();
    QJsonObject fParams;

private slots:
    void on_btnCancel_clicked();

    void on_btnTest_clicked();

    void on_btnSave_clicked();

    void on_btnRefreshSettings_clicked();

private:
    Ui::C5Connection *ui;
};

#endif // C5CONNECTION_H
