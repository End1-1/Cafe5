#include "dlgsplashscreen.h"
#include "ui_dlgsplashscreen.h"
#include "c5user.h"
#include "ninterface.h"
#include <QTimer>

DlgSplashScreen::DlgSplashScreen(C5User *user) :
    C5Dialog(user),
    ui(new Ui::DlgSplashScreen)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    ui->lbText->setText(tr("Prepare to work"));
    connect(this, &DlgSplashScreen::messageSignal, this, &DlgSplashScreen::message);
}

DlgSplashScreen::~DlgSplashScreen()
{
    delete ui;
}

void DlgSplashScreen::login(const QString &username, const QString &password, std::function<void (C5User *user)> callback)
{
    fHttp->createHttpQueryLambda("/engine/v2/worker/user-login/login", {
        {"username", username},
        {"password", password},
        {"nootp", true}
    },
    [callback](const QJsonObject & jdoc) {
        auto user = new C5User();
        user->fConfig = jdoc["jsonconfig"].toObject();
        QJsonArray jsettings = jdoc["settings"].toArray();

        for(int i = 0; i < jsettings.count(); i++) {
            const QJsonObject &js = jsettings.at(i).toObject();
            user->fSettings[js["f_key"].toInt()] = js["f_value"].toString();
        }

        QJsonArray jpermissions = jdoc["permissions"].toArray();

        for(int i = 0; i < jpermissions.count(); i++) {
            user->addPermission(jpermissions.at(i).toInt());
        }

        user->fUserData = jdoc["userdata"].toObject().toVariantMap();
        callback(user);
    }, [callback](const QJsonObject & jerr) {
        Q_UNUSED(jerr);
        callback(nullptr);
    }, QVariant(), false);
}

void DlgSplashScreen::message(const QString &text)
{
    ui->lbText->setText(text);
}
