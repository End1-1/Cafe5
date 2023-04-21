#include "httpquerydialog.h"
#include "ui_httpquerydialog.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>


HttpQueryDialog::HttpQueryDialog(const QStringList &dbParams, const QString &url, const QJsonObject &jo, QWidget *parent) :
    C5Dialog(dbParams, parent),
    ui(new Ui::HttpQueryDialog),
    jRequest(jo),
    fUrl(url)
{
    ui->setupUi(this);
}

HttpQueryDialog::~HttpQueryDialog()
{
    delete ui;
}

int HttpQueryDialog::exec()
{
    QTimer *t = new QTimer(this);
    connect(t, &QTimer::timeout, this, [=]{
        sendRequest();
    });
    t->start(1000);
    return C5Dialog::exec();
}

void HttpQueryDialog::sendRequest()
{
    static_cast<QTimer*>(sender())->stop();
    sender()->deleteLater();
    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    const QUrl url(fUrl);
    QNetworkRequest request(url);
    QSslConfiguration sslConf = request.sslConfiguration();
    sslConf.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConf.setProtocol(QSsl::AnyProtocol);
    request.setSslConfiguration(sslConf);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");


    QJsonDocument doc(jRequest);
    QByteArray data = doc.toJson();
    QNetworkReply *reply = mgr->post(request, data);

    QObject::connect(reply, &QNetworkReply::finished, [=](){
        if(reply->error() == QNetworkReply::NoError){
            QString contents = QString::fromUtf8(reply->readAll());
            QJsonObject jo = QJsonDocument::fromJson(contents.toUtf8()).object();
            if (jo["ok"].toInt() == 1) {
                ui->label->setText(contents);
                accept();
            } else {
                ui->label->setText(contents);
            }
        } else {
            QString err = reply->errorString();
            ui->label->setText(err);
        }
        reply->deleteLater();
    });
}
