#include "connectionsettings.h"
#include "ui_connectionsettings.h"
#include "message.h"
#include "widgetcontainer.h"
#include <QSettings>
#include <QInputDialog>

static QSettings __s(_ORGANIZATION_, _APPLICATION_);

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
    QByteArray buf = __s.value("con").toByteArray();
    for (int i = 0; i < buf.length(); i++) {
        buf[i] = buf[i] ^ ((i % 2) + (i % 3) + (i % 4) + (i % 5) + (i % 6) + (i % 7) + (i % 8) + (i % 9));
    }
    QList<QByteArray> s = buf.split(';');
    for (const QString l: s) {
        QStringList kv = l.split('=');
        if (kv.count() != 2) {
            continue;
        }
        cs->setStrw(kv.at(0), kv.at(1));
    }
    if (!cs->strw("password").isEmpty()) {
        bool ok = false;
        QString pwd = QInputDialog::getText(cs, tr("Database password"), tr("Database password"), QLineEdit::Password, "", &ok);
        if (ok && pwd == cs->strw("password")) {

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
    settings += "database=" + strw("database") + ";";
    settings += "username=" + strw("username") + ";";
    settings += "password=" + strw("password") + ";";
    for (int i = 0; i < settings.length(); i++) {
        settings[i] = settings[i] ^ ((i % 2) + (i % 3) + (i % 4) + (i % 5) + (i % 6) + (i % 7) + (i % 8) + (i % 9));
    }
    __s.setValue("con", settings);
}
