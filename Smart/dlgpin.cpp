#include "dlgpin.h"
#include "ui_dlgpin.h"
#include "ndataprovider.h"
#include "c5config.h"
#include "ninterface.h"
#include "c5user.h"
#include "c5database.h"
#include "appwebsocket.h"
#include "c5connectiondialog.h"
#include <QKeyEvent>
#include <QJsonObject>

namespace {

void applyHttpEndpoint(const QString &protocol, const QString &address)
{
    QString proto = protocol.trimmed().toLower();
    QString host = address.trimmed();

    while(host.endsWith(QLatin1Char('/'))) {
        host.chop(1);
    }

    const int scheme = host.indexOf(QStringLiteral("://"));

    if(scheme > 0) {
        const QString embedded = host.left(scheme).toLower();
        host = host.mid(scheme + 3);

        while(host.endsWith(QLatin1Char('/'))) {
            host.chop(1);
        }

        if(embedded == QLatin1String("wss") || embedded == QLatin1String("https")) {
            proto = QStringLiteral("https");
        } else if(embedded == QLatin1String("ws") || embedded == QLatin1String("http")) {
            proto = QStringLiteral("http");
        } else {
            proto = embedded;
        }
    }

    if(proto == QLatin1String("wss")) {
        proto = QStringLiteral("https");
    } else if(proto == QLatin1String("ws") || proto.isEmpty()) {
        proto = QStringLiteral("http");
    }

    NDataProvider::mProtocol = proto;
    NDataProvider::mHost = host;
}

}

QString DlgPin::mPresetProtocol;
QString DlgPin::mPresetHost;
QString DlgPin::mPresetWsUrl;
QString DlgPin::mPresetServerKey;

void DlgPin::setConnection(const QString &httpProtocol, const QString &httpHost, const QString &wsUrl, const QString &serverKey)
{
    mPresetProtocol = httpProtocol;
    mPresetHost = httpHost;
    mPresetWsUrl = wsUrl;
    mPresetServerKey = serverKey;
}

DlgPin::DlgPin() :
    C5Dialog(nullptr),
    ui(new Ui::DlgPin)
{
    ui->setupUi(this);
    ui->leUser->hide();
    ui->lePin->setFocus();
    fPinEmpty = true;
    fLastError = false;
    installEventFilter(this);
    fDoNotAuth = false;
}

DlgPin::~DlgPin()
{
    delete ui;
}

bool DlgPin::getPin(QString &pin, QString &pass, bool donotauth)
{
    bool result = false;
    DlgPin *d = new DlgPin();
    d->fDoNotAuth = donotauth;

    if(!pin.isEmpty()) {
        d->ui->leUser->setText(pin);
        d->ui->lePin->setText(pass);
        result = true;
    }

    if(!result) {
        if(d->exec() == QDialog::Accepted) {
            pin = d->ui->leUser->text();
            pass = d->ui->lePin->text();
            result = true;
        }
    }

    delete d;
    return result;
}

void DlgPin::btnNumPressed()
{
    QPushButton *btn = static_cast<QPushButton*>(sender());
    ui->lePin->setText(ui->lePin->text() + btn->text());

    if(ui->lePin->text().length() == 4 && !fLastError) {
        on_btnEnter_clicked();
    }
}

void DlgPin::on_btnEnter_clicked()
{
    if(fDoNotAuth) {
        accept();
        return;
    }

    fLastError = false;

    if(!mPresetHost.isEmpty()) {
        applyHttpEndpoint(mPresetProtocol, mPresetHost);
    } else {
        const bool secure = C5ConnectionDialog::instance()->connectionType() != C5ConnectionDialog::noneSecure;
        applyHttpEndpoint(secure ? QStringLiteral("https") : QStringLiteral("http"),
                          C5ConnectionDialog::instance()->serverAddress());
    }
    fHttp->fErrorObject = this;
    fHttp->fErrorSlot = const_cast<char*>(SLOT(errorResponse(QString)));

    if(ui->lePin->text().length() == 4) {
        fHttp->createHttpQueryLambda("/engine/login.php", QJsonObject{{"method", 2},
            {"pin", ui->lePin->text()}}, [this](const QJsonObject & jdoc) {
            QJsonObject jo = jdoc["data"].toObject();
            NDataProvider::sessionKey = jo["sessionkey"].toString();
            QMap<int, QString> settings;
            QJsonObject juser = jo["user"].toObject();
            QJsonArray jsettings = jo["settings"].toArray();

            for(int i = 0; i < jsettings.count(); i++) {
                const QJsonObject &js = jsettings.at(i).toObject();
                settings[js["f_key"].toInt()] = js["f_value"].toString();
            }

            mUser = new C5User(juser.toVariantMap());
            mUser->fSettings = settings;
            mUser->fConfig = jo["config"].toObject()["f_config"].toObject();
            mUser->mSessionKey = NDataProvider::sessionKey;
            C5Database::fDbParams = {"", "", "", ""};
            C5Database::fUrl = NDataProvider::mProtocol + QStringLiteral("://") + NDataProvider::mHost;
            __c5config.setValues(settings);
            __c5config.fMainJson = mUser->fConfig;
            QString wsUrl = mPresetWsUrl;
            QString serverKey = mPresetServerKey;

            if(wsUrl.isEmpty()) {
                const bool secure = C5ConnectionDialog::instance()->connectionType() != C5ConnectionDialog::noneSecure;
                wsUrl = (secure ? QStringLiteral("wss://") : QStringLiteral("ws://"))
                        + NDataProvider::mHost + QStringLiteral("/ws");
                serverKey = C5ConnectionDialog::instance()->serverKey();
            }

            AppWebSocket::reconnect(wsUrl, serverKey, mUser->id());
            accept();
        }, [this](const QJsonObject & jerr) {
            Q_UNUSED(jerr);
            fLastError = true;
        });
    }
}

void DlgPin::on_btnClear_clicked()
{
    ui->lePin->clear();
    ui->lePin->setFocus();
    fLastError = false;
    fPinEmpty = true;
}

void DlgPin::on_btnClose_clicked()
{
    reject();

    if(!fDoNotAuth) {
        qApp->quit();
    }
}

void DlgPin::on_btn1_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn2_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn3_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn4_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn5_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn6_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn7_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn8_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn9_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn0_clicked()
{
    btnNumPressed();
}

void DlgPin::keyReleaseEvent(QKeyEvent *event)
{
    if(ui->lePin->text().length() < 4) {
        fLastError = false;
    }

    if(ui->lePin->text().length() == 4 && !fLastError) {
        on_btnEnter_clicked();
        event->accept();
        return;
    }

    fPinEmpty = ui->lePin->isEmpty();
    QDialog::keyReleaseEvent(event);
}

void DlgPin::showEvent(QShowEvent *e)
{
    C5Dialog::showEvent(e);

    ui->lePin->setFocus();
}

void DlgPin::on_btnSettings_clicked()
{
    C5ConnectionDialog::showSettings(this);
    NDataProvider::mProtocol = C5ConnectionDialog::instance()->noneSecure ? "http" : "https";
    NDataProvider::mHost = C5ConnectionDialog::instance()->serverAddress();
}
