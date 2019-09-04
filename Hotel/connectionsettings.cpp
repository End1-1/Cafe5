#include "connectionsettings.h"
#include "ui_connectionsettings.h"
#include "message.h"
#include "widgetcontainer.h"
#include "settings.h"
#include <QInputDialog>

ConnectionSettings::ConnectionSettings() :
    Dialog(),
    ui(new Ui::ConnectionSettings)
{
    ui->setupUi(this);
}

ConnectionSettings::~ConnectionSettings()
{
    delete ui;
}

void ConnectionSettings::configure()
{
    ConnectionSettings *cs = new ConnectionSettings();
    cs->setWidgetContainer();
    cs->setStrw("host", __s.s("host"));
    cs->setStrw("port", __s.s("port"));
    cs->setStrw("hostpassword", __s.s("hostpassword"));
    if (!cs->strw("hostpassword").isEmpty()) {
        bool ok = false;
        QString pwd = QInputDialog::getText(cs, tr("Database password"), tr("Database password"), QLineEdit::Password, "", &ok, Qt::FramelessWindowHint);
        if (ok && pwd == cs->strw("hostpassword")) {

        } else {
            if (ok) {
                Message::showMessage(tr("Access denied"));
            }
            delete cs;
            return;
        }
    }
    cs->exec();
    delete cs;
}

void ConnectionSettings::setWidgetContainer()
{
    fCtrl = ui->wc;
    fCtrl->getWatchList(fCtrl);
}

void ConnectionSettings::on_btnClose_clicked()
{
    accept();
}

void ConnectionSettings::on_btnSave_clicked()
{
    QByteArray settings;
    settings += "host=" + strw("host") + ";";
    settings += "port=" + strw("port") + ";";
    settings += "hostpassword=" + strw("hostpassword") + ";";
    for (int i = 0; i < settings.length(); i++) {
        settings[i] = settings[i] ^ ((i % 2) + (i % 3) + (i % 4) + (i % 5) + (i % 6) + (i % 7) + (i % 8) + (i % 9));
    }
    __s.set("con", settings);
    __s.set("host", strw("host"));
    __s.set("port", strw("port"));
    __s.set("hostpassword", strw("hostpassword"));
    Message::showMessage(tr("Saved"));
}
