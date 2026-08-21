#include "dlgpin.h"
#include <QAction>
#include <QEvent>
#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>
#include <QKeyEvent>
#include <QMenu>
#include <QPoint>
#include "appwebsocket.h"
#include "c5connectiondialog.h"
#include "c5uilanguage.h"
#include "c5user.h"
#include "ndataprovider.h"
#include "ninterface.h"
#include "ui_dlgpin.h"
#include "working.h"

DlgPin::DlgPin(QWidget *parent) :
    C5ShopDialog(nullptr),
    ui(new Ui::DlgPin)
{
    Q_UNUSED(parent);
    ui->setupUi(this);
    fPinEmpty = true;
    fLastError = false;
    installEventFilter(this);
    fDoNotAuth = false;
    updateLangButton();
}

DlgPin::~DlgPin()
{
    delete ui;
}

void DlgPin::changeEvent(QEvent *e)
{
    C5ShopDialog::changeEvent(e);
    if(e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateLangButton();
    }
}

void DlgPin::updateLangButton()
{
    const QString lang = C5UiLanguage::current();
    ui->btnLang->setIcon(QIcon(C5UiLanguage::flagIcon(lang)));
    ui->btnLang->setToolTip(C5UiLanguage::displayName(lang));
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
    NDataProvider::mProtocol = C5ConnectionDialog().instance()->connectionType() == C5ConnectionDialog::noneSecure ? "http" : "https";
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
            mUser->active = jo.value(QStringLiteral("active")).toBool();

            const QJsonArray jpermissions = jo.value(QStringLiteral("permissions")).toArray();

            for (int i = 0; i < jpermissions.count(); i++) {
                mUser->addPermission(jpermissions.at(i).toInt());
            }

            AppWebSocket::reconnect((C5ConnectionDialog::instance()->connectionType()
                                             == C5ConnectionDialog::instance()->noneSecure
                                         ? "ws://"
                                         : "wss://")
                                        + C5ConnectionDialog::instance()->serverAddress() + "/ws",
                                    C5ConnectionDialog::instance()->serverKey(),
                                    ui->leUser->text(),
                                    ui->lePin->text());
            accept();
        }, [](const QJsonObject & jerr) {
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
    C5ShopDialog::showEvent(e);

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

void DlgPin::on_btnLang_clicked()
{
    const QString current = C5UiLanguage::current();
    QMenu menu(this);

    auto *actAm = menu.addAction(QIcon(C5UiLanguage::flagIcon(C5UiLanguage::kAm)),
                                 C5UiLanguage::displayName(C5UiLanguage::kAm));
    actAm->setData(QString(C5UiLanguage::kAm));
    actAm->setCheckable(true);
    actAm->setChecked(current == QLatin1String(C5UiLanguage::kAm));

    auto *actRu = menu.addAction(QIcon(C5UiLanguage::flagIcon(C5UiLanguage::kRu)),
                                 C5UiLanguage::displayName(C5UiLanguage::kRu));
    actRu->setData(QString(C5UiLanguage::kRu));
    actRu->setCheckable(true);
    actRu->setChecked(current == QLatin1String(C5UiLanguage::kRu));

    QAction *chosen = menu.exec(ui->btnLang->mapToGlobal(QPoint(0, ui->btnLang->height())));
    if(!chosen) {
        return;
    }

    const QString lang = chosen->data().toString();
    if(lang == current) {
        return;
    }

    C5UiLanguage::apply(lang);
    updateLangButton();
}
