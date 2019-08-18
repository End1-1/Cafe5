#ifndef CONNECTIONSETTINGS_H
#define CONNECTIONSETTINGS_H

#include "dialog.h"

namespace Ui {
class ConnectionSettings;
}

class ConnectionSettings : public Dialog
{
    Q_OBJECT

public:
    explicit ConnectionSettings();
    ~ConnectionSettings();
    static void configure();

protected:
    virtual void setWidgetContainer();

private slots:
    void on_btnClose_clicked();
    void on_btnSave_clicked();

private:
    Ui::ConnectionSettings *ui;
};

#endif // CONNECTIONSETTINGS_H
