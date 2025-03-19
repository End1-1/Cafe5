#include "dlgpin.h"
#include "ui_dlgpin.h"
#include "nloadingdlg.h"
#include "c5message.h"
#include "ndataprovider.h"
#include <QKeyEvent>
#include <QJsonObject>

DlgPin::DlgPin(QWidget *parent) :
    C5Dialog(QStringList()),
    ui(new Ui::DlgPin)
{
    Q_UNUSED(parent);
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
    if (!pin.isEmpty()) {
        d->ui->leUser->setText(pin);
        d->ui->lePin->setText(pass);
        result = true;
    }
    if (!result) {
        if (d->exec() == QDialog::Accepted) {
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
    QPushButton *btn = static_cast<QPushButton *>(sender());
    QLineEdit *l = ui->leUser;
    if (ui->lePin->hasFocus()) {
        l = ui->lePin;
    }
    l->setText(l->text() + btn->text());
}

void DlgPin::on_btnEnter_clicked()
{
    if (fDoNotAuth) {
        accept();
        return;
    }
    fLastError = false;
    NDataProvider::mHost = __c5config.dbParams().at(1);
    fHttp->fErrorObject = this;
    fHttp->fErrorSlot = const_cast<char *>(SLOT(errorResponse(QString)));
    if (ui->lePin->text().length() == 4 && ui->leUser->text().length() == 4) {
        fHttp->createHttpQuery("/engine/login.php", QJsonObject{{"method", 1},
            {"username", ui->leUser->text()},
            {"password", ui->lePin->text()}},
        SLOT(loginResponse(QJsonObject)));
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
    switch (event->key()) {
        case Qt::Key_Backspace:
            if (ui->lePin->isEmpty() && ui->lePin->hasFocus() && fPinEmpty) {
                ui->leUser->setFocus();
                if (ui->leUser->text().length() == 4) {
                    ui->leUser->setText(ui->leUser->text().left(3));
                }
                ui->leUser->setCursorPosition(ui->leUser->text().length());
                event->accept();
            }
            break;
    }
    if (ui->lePin->text().length() < 4 || ui->leUser->text().length() < 4) {
        fLastError = false;
    }
    if (ui->leUser->hasFocus() && ui->leUser->text().length() == 4) {
        ui->lePin->setFocus();
        event->accept();
        return;
    }
    if (ui->lePin->hasFocus() && ui->lePin->text().length() == 4 && !fLastError) {
        on_btnEnter_clicked();
        event->accept();
        return;
    }
    if (ui->lePin->isEmpty()) {
        fPinEmpty = true;
    } else {
        fPinEmpty = false;
    }
    QDialog::keyReleaseEvent(event);
}

void DlgPin::loginResponse(const QJsonObject &jdoc)
{
    QJsonObject jo = jdoc["data"].toObject();
    NDataProvider::sessionKey = jo["sessionkey"].toString();
    fHttp->httpQueryFinished(sender());
    accept();
}

void DlgPin::errorResponse(const QString &err)
{
    fLastError = true;
    fHttp->httpQueryFinished(sender());
    C5Message::error(err);
}

void DlgPin::queryLoading()
{
    fLoadingDlg = new NLoadingDlg(this);
    fLoadingDlg->open();
}

void DlgPin::queryStopped(QObject *sender)
{
    fLoadingDlg->hide();
    fLoadingDlg->deleteLater();
    sender->deleteLater();
}

void DlgPin::queryFinished(const QJsonObject &json)
{
    QJsonObject jo = json["data"].toObject();
    Q_UNUSED(jo);
    queryStopped(sender());
}

void DlgPin::queryError(const QString &err)
{
    C5Message::error(err);
    queryStopped(sender());
}
