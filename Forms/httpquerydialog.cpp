#include "httpquerydialog.h"
#include "ui_httpquerydialog.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QWebSocket>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QTimer>

HttpQueryDialog::HttpQueryDialog(const QString &url, const QJsonObject &jo,
                                 QWidget *parent) :
    C5Dialog(parent),
    ui(new Ui::HttpQueryDialog),
    fUrl(url),
    jRequest(jo)
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
    connect(t, &QTimer::timeout, this, [ = ] {
        sendRequest();
    });
    t->start(1000);
    return C5Dialog::exec();
}

void HttpQueryDialog::sendRequest()
{
    static_cast<QTimer *>(sender())->stop();
    sender()->deleteLater();
    QWebSocket *s = new QWebSocket();
    QUrl url(QString("%1").arg(fUrl));
    QEventLoop l1;
    connect( &fTimer, &QTimer::timeout, this, [this]() {
        fTimer.stop();
        ui->label->setText(tr("Operation timeout"));
    });
    fTimer.start(12000);
    connect(s, &QWebSocket::connected, &l1, &QEventLoop::quit);
    connect(s, &QWebSocket::disconnected, &l1, &QEventLoop::quit);
    connect(this, &HttpQueryDialog::exitLoop, &l1, &QEventLoop::quit);
    connect(this, &HttpQueryDialog::messageReceived, &l1, &QEventLoop::quit);
    connect( &fTimer, &QTimer::timeout, &l1, &QEventLoop::quit);
    s->open(url);
    l1.exec();
    QEventLoop l2;
    connect(s, &QWebSocket::textMessageReceived, this, [this](const QString &s) {
        qDebug() << s;
        QJsonObject jrep = QJsonDocument::fromJson(s.toUtf8()).object();
        if (jrep["errorCode"].toInt() > 0) {
            ui->label->setText(jrep["errorMessage"].toString());
            return;
        }
        emit messageReceived();
        accept();
    });
    connect(this, &HttpQueryDialog::messageReceived, &l2, &QEventLoop::quit);
    connect(s, &QWebSocket::disconnected, &l2, &QEventLoop::quit);
    connect(this, &HttpQueryDialog::messageReceived, &l2, &QEventLoop::quit);
    connect(s, &QWebSocket::disconnected, &l2, &QEventLoop::quit);
    connect( &fTimer, &QTimer::timeout, &l2, &QEventLoop::quit);
    qDebug() << "Send data" << jRequest;
    s->sendTextMessage(QJsonDocument(jRequest).toJson(QJsonDocument::Compact));
    l2.exec();
}

void HttpQueryDialog::connectError(QAbstractSocket::SocketError error)
{
    emit exitLoop();
}
