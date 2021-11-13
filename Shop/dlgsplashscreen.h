#ifndef DLGSPLASHSCREEN_H
#define DLGSPLASHSCREEN_H

#include "c5dialog.h"

namespace Ui {
class DlgSplashScreen;
}

class DlgSplashScreen : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgSplashScreen();
    ~DlgSplashScreen();

private:
    Ui::DlgSplashScreen *ui;

private slots:
    void timeout();

    void message(const QString &text);

signals:
    void messageSignal(const QString &);
};

#endif // DLGSPLASHSCREEN_H
