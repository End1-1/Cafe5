#include "c5connection.h"
#include "ui_c5connection.h"
#include "c5config.h"
#include <QSettings>
#include <QInputDialog>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QThread>


#define db_ver 1
#define params_count 6

C5Connection::C5Connection(const QJsonObject &params) :
    C5Dialog(QStringList()),
    fParams(params),
    ui(new Ui::C5Connection)
{
    ui->setupUi(this);
    ui->leName->setText(params["name"].toString());
    ui->leServer->setText(params["waiter_server"].toString());
    ui->leHost->setText(params["host"].toString());
    ui->leDatabase->setText(params["database"].toString());
    ui->leUsername->setText(params["username"].toString());
    ui->lePassword->setText(params["password"].toString());
    ui->chFullScreen->setChecked(params["fullscreen"].toBool());
    if (!ui->leHost->text().isEmpty() && !ui->leDatabase->text().isEmpty()) {
        on_btnRefreshSettings_clicked();
        ui->cbSettings->setCurrentText(params["settings"].toString());
    }
    ui->chFullScreen->setChecked(params["fullscreen"].toBool());
    ui->leServer->setText(C5Config::serverIP());
}

C5Connection::~C5Connection()
{
    delete ui;
}

bool C5Connection::preambule()
{
    bool ok = false;
    QString password = QInputDialog::getText(__c5config.fParentWidget, tr("Configuration settings"), tr("Password"), QLineEdit::Password, "", &ok);
    if (ok) {
        if (password != ui->lePassword->text()) {
            C5Message::error(tr("Access denied"));
            return false;
        }
    } else {
        return false;
    }
    return true;
}

void C5Connection::on_btnCancel_clicked()
{
    reject();
}

void C5Connection::on_btnTest_clicked()
{
#ifdef NETWORKDB
    QNetworkAccessManager m;
    QString host = "https://" + ui->leHost->text();
    QNetworkRequest rq(host);
    m.setTransferTimeout(60000);
    rq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QSslConfiguration sslConf = rq.sslConfiguration();
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConf.setProtocol(QSsl::AnyProtocol);
    rq.setSslConfiguration(sslConf);
    QJsonObject jo;
    jo["query"] = 3;
    jo["call"] = "sql";
    jo["sql"] = "select * from s_users where f_id=1";
    jo["sk"] = "5cfafe13-a886-11ee-ac3e-1078d2d2b808";
    auto *r = m.post(rq, QJsonDocument(jo).toJson());
    while (!r->isFinished()) {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        QThread::msleep(10);
    }
    if (r->error() != QNetworkReply::NoError) {
        C5Message::error(r->errorString());
        return;
    }
    QByteArray ba = r->readAll();
    jo = QJsonDocument::fromJson(ba).object();
    if (jo["status"].toInt() == 0) {
        C5Message::error(ba);
        return;
    }
    C5Message::info(tr("Connection successfull"));

#else
    C5Database db(ui->leHost->text(), ui->leDatabase->text(), ui->leUsername->text(), ui->lePassword->text());
    if (!db.open()) {
        C5Message::error(tr("Cannot connect to database") + "<br>" + db.fLastError);
        return;
    }
    db.close();
    C5Message::info(tr("Connection successfull"));
#endif
}

void C5Connection::on_btnSave_clicked()
{
    fParams["name"] = ui->leName->text();
    fParams["waiter_server"] = ui->leServer->text();
    fParams["host"] = ui->leHost->text();
    fParams["database"] = ui->leDatabase->text();
    fParams["username"] = ui->leUsername->text();
    fParams["password"] = ui->lePassword->text();
    fParams["settings"] = ui->cbSettings->currentText();
    fParams["fullscreen"] = ui->chFullScreen->isChecked();
    C5Message::info(tr("Saved"));
    C5Config::setServerIP(ui->leServer->text());
    accept();
}


void C5Connection::on_btnRefreshSettings_clicked()
{
    ui->cbSettings->clear();
    C5Database db(ui->leHost->text(), ui->leDatabase->text(), ui->leUsername->text(), ui->lePassword->text());
    if (db.open()) {
        db.exec("select f_id, f_name, f_description from s_settings_names");
        while (db.nextRow()) {
            ui->cbSettings->addItem(db.getString(1), db.getInt(0));
        }
    }
}
