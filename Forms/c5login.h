#ifndef C5LOGIN_H
#define C5LOGIN_H

#include "c5officedialog.h"
#include <QJsonObject>
#include <QJsonArray>

class QEvent;

namespace Ui
{
class C5Login;
}

class C5Login : public C5OfficeDialog
{
    Q_OBJECT

public:
    explicit C5Login(C5User *user);

    ~C5Login();

protected:
    void changeEvent(QEvent *e) override;

private slots:
    void on_btnCancel_clicked();

    void on_btnOk_clicked();

    void on_cbDatabases_currentIndexChanged(int index);

    void on_btnConfig_clicked();

    void on_btnLang_clicked();

private:
    Ui::C5Login* ui;

    QJsonArray fServers;

    void readServers();

    void updateLangButton();
};

#endif // C5LOGIN_H
