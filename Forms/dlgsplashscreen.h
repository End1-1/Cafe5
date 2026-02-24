#ifndef DLGSPLASHSCREEN_H
#define DLGSPLASHSCREEN_H

#include "c5dialog.h"

namespace Ui
{
class DlgSplashScreen;
}

class C5User;

class DlgSplashScreen : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgSplashScreen(int workstationType, C5User *user);

    ~DlgSplashScreen();

    void prepare();

    std::function<void(C5User *user)> mOnFinish;

    void login(const QString &username,
               const QString &password,
               std::function<void(C5User *user)> onFinish);

    void loadWorkstationConfig(std::function<void()> nextStep);

private:
    Ui::DlgSplashScreen* ui;

    int mWorkstationType;

private slots:
    void message(const QString &text);

signals:
    void messageSignal(const QString&);
};

#endif // DLGSPLASHSCREEN_H
