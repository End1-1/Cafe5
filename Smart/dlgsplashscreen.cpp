#include "dlgsplashscreen.h"
#include "ui_dlgsplashscreen.h"

DlgSplashScreen::DlgSplashScreen(QWidget *parent) :
    QDialog(parent),
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

void DlgSplashScreen::message(const QString &text)
{
    ui->lbText->setText(text);
}
