#include "dlgsplashscreen.h"
#include <QTimer>
#include "c5user.h"
#include "c5utils.h"
#include "ninterface.h"
#include "struct_workstationitem.h"
#include "ui_dlgsplashscreen.h"

DlgSplashScreen::DlgSplashScreen(int workstationType, C5User *user)
    : C5Dialog(user)
    , ui(new Ui::DlgSplashScreen)
    , mWorkstationType(workstationType)
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

void DlgSplashScreen::prepare()
{
    loadWorkstationConfig([this]() { mOnFinish(mUser); });
}

void DlgSplashScreen::login(const QString &username,
                            const QString &password,
                            std::function<void(C5User *user)> onFinish)
{
    mOnFinish = onFinish;
    fHttp->createHttpQueryLambda(
        "/engine/v2/worker/user-login/login",
        {{"username", username}, {"password", password}, {"nootp", true}},
        [this, onFinish](const QJsonObject &jdoc) {
            auto user = new C5User();
            user->fConfig = jdoc["jsonconfig"].toObject();
            QJsonArray jsettings = jdoc["settings"].toArray();

            for (int i = 0; i < jsettings.count(); i++) {
                const QJsonObject &js = jsettings.at(i).toObject();
                user->fSettings[js["f_key"].toInt()] = js["f_value"].toString();
            }

            QJsonArray jpermissions = jdoc["permissions"].toArray();

            for (int i = 0; i < jpermissions.count(); i++) {
                user->addPermission(jpermissions.at(i).toInt());
            }

            user->fUserData = jdoc["userdata"].toObject().toVariantMap();
            user->mSessionKey = jdoc["token"].toString();
            mUser = new C5User();
            mUser->copySettings(user);
            loadWorkstationConfig([this]() { mOnFinish(mUser); });
        },
        [this](const QJsonObject &jerr) {
            Q_UNUSED(jerr);
            mOnFinish(nullptr);
        },
        QVariant(),
        false);
}

void DlgSplashScreen::loadWorkstationConfig(std::function<void()> nextStep)
{
    NInterface::query(
        "/engine/v2/common/workstation/get-config",
        mUser->mSessionKey,
        this,
        {{"type", mWorkstationType}, {"station_account", hostusername()}, {"workstation", hostinfo}},
        [this](const QJsonObject &jdoc) {
            mWorkStation = JsonParser<WorkstationItem>::fromJson(jdoc);
            mOnFinish(mUser);
        },
        [this](const QJsonObject &jerr) {
            mOnFinish(nullptr);
            return false;
        });
}

void DlgSplashScreen::message(const QString &text)
{
    ui->lbText->setText(text);
}
