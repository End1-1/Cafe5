#ifndef C5LOGIN_H
#define C5LOGIN_H

#include "c5dialog.h"
#include <QJsonObject>
#include <QJsonArray>

namespace Ui
{
class C5Login;
}

class C5Login : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5Login();

    ~C5Login();

private slots:
    void loginResponse(const QJsonObject &jdoc);

    void on_btnCancel_clicked();

    void on_btnOk_clicked();

    void on_cbDatabases_currentIndexChanged(int index);

    void on_btnConfig_clicked();

private:
    Ui::C5Login *ui;

    QJsonArray fServers;

    void readServers();
};

#endif // C5LOGIN_H
