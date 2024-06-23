#ifndef DLGSPLASHSCREEN_H
#define DLGSPLASHSCREEN_H

#include <QDialog>

namespace Ui
{
class DlgSplashScreen;
}

class DlgSplashScreen : public QDialog
{
    Q_OBJECT

public:
    explicit DlgSplashScreen();

    ~DlgSplashScreen();

private:
    Ui::DlgSplashScreen *ui;

private slots:
    void message(const QString &text);

signals:
    void messageSignal(const QString &);
};

#endif // DLGSPLASHSCREEN_H
