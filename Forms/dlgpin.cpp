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

DlgPin::DlgPin() :
    C5Dialog(nullptr),
    ui(new Ui::DlgPin)
{
    ui->setupUi(this);
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
    QLineEdit *l = ui->leUser;

    if(ui->lePin->hasFocus()) {
        l = ui->lePin;
    }

    l->setText(l->text() + btn->text());
}

void DlgPin::on_btnEnter_clicked()
{
    if(fDoNotAuth) {
        accept();
        return;
    }

    fLastError = false;
    NDataProvider::mProtocol = C5ConnectionDialog::instance()->connectionType() == C5ConnectionDialog::noneSecure ? "http" : "https";
    NDataProvider::mHost = C5ConnectionDialog::instance()->serverAddress();
    fHttp->fErrorObject = this;
    fHttp->fErrorSlot = const_cast<char*>(SLOT(errorResponse(QString)));

    if(ui->lePin->text().length() == 4 && ui->leUser->text().length() == 4) {
        fHttp->createHttpQueryLambda("/engine/login.php", QJsonObject{{"method", 1},
            {"username", ui->leUser->text()},
            {"password", ui->lePin->text()}}, [this](const QJsonObject & jdoc) {
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
            C5Database::fUrl = QString("%1://%2").arg(C5ConnectionDialog::instance()->connectionType() == C5ConnectionDialog::noneSecure ? "http" : "https",
                               C5ConnectionDialog::instance()->serverAddress());
            __c5config.setValues(settings);
            __c5config.fMainJson = mUser->fConfig;
            AppWebSocket::reconnect(QString("%1://%2").arg(C5ConnectionDialog::instance()->connectionType() == C5ConnectionDialog::noneSecure ? "ws" : "wss",
                                    C5ConnectionDialog::instance()->serverAddress() + "/ws"),
                                    C5ConnectionDialog::instance()->serverKey(),
                                    ui->leUser->text(), ui->lePin->text());
            accept();
        }, [](const QJsonObject & jerr) {
            Q_UNUSED(jerr);
        });
    }
}

void DlgPin::on_btnClear_clicked()
{
    ui->lePin->clear();
    ui->leUser->clear();
    ui->leUser->setFocus();
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
    switch(event->key()) {
    case Qt::Key_Backspace:
        if(ui->lePin->isEmpty() && ui->lePin->hasFocus() && fPinEmpty) {
            ui->leUser->setFocus();

            if(ui->leUser->text().length() == 4) {
                ui->leUser->setText(ui->leUser->text().left(3));
            }

            ui->leUser->setCursorPosition(ui->leUser->text().length());
            event->accept();
        }

        break;
    }

    if(ui->lePin->text().length() < 4 || ui->leUser->text().length() < 4) {
        fLastError = false;
    }

    if(ui->leUser->hasFocus() && ui->leUser->text().length() == 4) {
        ui->lePin->setFocus();
        event->accept();
        return;
    }

    if(ui->lePin->hasFocus() && ui->lePin->text().length() == 4 && !fLastError) {
        on_btnEnter_clicked();
        event->accept();
        return;
    }

    if(ui->lePin->isEmpty()) {
        fPinEmpty = true;
    } else {
        fPinEmpty = false;
    }

    QDialog::keyReleaseEvent(event);
}

void DlgPin::showEvent(QShowEvent *e)
{
    C5Dialog::showEvent(e);

    if(!fDoNotAuth && !mDoNotAutoLogin) {
        if(!C5ConnectionDialog::instance()->username().isEmpty()) {
            ui->leUser->setText(C5ConnectionDialog::instance()->username());
            ui->lePin->setText(C5ConnectionDialog::instance()->password());
            on_btnEnter_clicked();
        }
    }
}

void DlgPin::on_btnSettings_clicked()
{
    C5ConnectionDialog::showSettings(this);
    NDataProvider::mProtocol = C5ConnectionDialog::instance()->noneSecure ? "http" : "https";
    NDataProvider::mHost = C5ConnectionDialog::instance()->serverAddress();
}
