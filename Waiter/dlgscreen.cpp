#include "dlgscreen.h"
#include "c5waiterserver.h"
#include "ui_dlgscreen.h"
#include "c5user.h"
#include "dlgface.h"
#include "c5waiterserver.h"
#include "c5cafecommon.h"
#include "c5serverhandler.h"
#include "dlgsystem.h"
#include "c5userauth.h"
#include <QTimer>

#define mode_hall 1
#define mode_system 2
#define mode_inout 3

DlgScreen::DlgScreen() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgScreen)
{
    ui->setupUi(this);
    connect(&fTcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    fTcpServer.listen(QHostAddress::Any, 1000);
    colorize(mode_hall);
    ui->lbDateOfClose->setText(QString("%1: %2").arg(tr("Date of close")).arg(__c5config.getValue(param_date_cash)));
    QTimer *t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &DlgScreen::timerTimeout);
    t->start(1000);

    C5Database db(__c5config.dbParams());
    db.exec("select f_settings, f_key, f_value from s_settings_values");
    C5CafeCommon::fHallConfigs.clear();
    while (db.nextRow()) {
        C5CafeCommon::fHallConfigs[db.getInt("f_settings")][db.getInt("f_key")] = db.getString("f_value");
    }
}

DlgScreen::~DlgScreen()
{
    delete ui;
}

void DlgScreen::newConnection()
{
    QTcpSocket *s = fTcpServer.nextPendingConnection();
    C5SocketHandler *sh = new C5SocketHandler(s, this);
    connect(sh, SIGNAL(handleCommand(QJsonObject)), this, SLOT(handleSocket(QJsonObject)));
}

void DlgScreen::timerTimeout()
{
    ui->lbCurrentTime->setText(QString("%1: %2").arg(tr("System datetime")).arg(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR)));
}

void DlgScreen::handleSocket(const QJsonObject &obj)
{
    QJsonObject no(obj);
    C5SocketHandler *sh = static_cast<C5SocketHandler*>(sender());
    C5WaiterServer ws(no, sh->fSocket);
    QJsonObject o;
    ws.reply(o);
    o["cmd"] = ws.cmd();
    sh->send(o);
    sh->close();
    sh->deleteLater();
}

void DlgScreen::on_btnClear_clicked()
{
    ui->lePassword->clear();
}


void DlgScreen::on_btnCancel_clicked()
{
    ui->lePassword->clear();
}

void DlgScreen::on_btn1_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton*>(sender())->text());
}
void DlgScreen::on_btn2_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton*>(sender())->text());
}

void DlgScreen::on_btn3_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton*>(sender())->text());
}

void DlgScreen::on_btn4_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton*>(sender())->text());
}

void DlgScreen::on_btn5_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton*>(sender())->text());
}

void DlgScreen::on_btn6_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton*>(sender())->text());
}

void DlgScreen::on_btn7_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton*>(sender())->text());
}

void DlgScreen::on_btn8_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton*>(sender())->text());
}

void DlgScreen::on_btn9_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton*>(sender())->text());
}

void DlgScreen::on_btn0_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton*>(sender())->text());
}

void DlgScreen::on_btnP_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton*>(sender())->text());
}

void DlgScreen::on_btnAccept_clicked()
{
    QString pass = ui->lePassword->text();
    ui->lePassword->clear();
    switch (fMode) {
    case mode_hall: {
        C5User u(pass);
        if (!u.isValid()) {
            C5Message::error(u.error());
            return;
        }
        if (!u.check(cp_t5_enter_dlgface)) {
            C5Message::error(tr("You have not permission to enter the working area"));
            return;
        }
        DlgFace d(&u);
        d.setup();
        d.exec();
        break;
    }
    case mode_system: {
        if (pass != __c5config.dbParams().at(3)) {
            C5Message::error(tr("Access denied"));
            return;
        }
        go<DlgSystem>(C5Config::dbParams());
        break;
    }
    case mode_inout:
        C5Database db(fDBParams);
        C5UserAuth *ua  = new C5UserAuth(db);
        if (!ua->enterWork(pass)) {
            C5Message::error(ua->error());
        }
        break;
    }
}

void DlgScreen::on_lePassword_returnPressed()
{
    on_btnAccept_clicked();
}

void DlgScreen::on_btnHall_clicked()
{
    colorize(mode_hall);
}

void DlgScreen::tryExit()
{
    if (C5Message::question(tr("Are you sure to close application")) == QDialog::Accepted) {
        qApp->quit();
    }
}

void DlgScreen::colorize(int mode)
{
    fMode = mode;
    ui->btnHall->setProperty("stylesheet_button_selected", fMode == mode_hall ? true : false);
    ui->btnHall->style()->polish(ui->btnHall);
    ui->btnSystem->setProperty("stylesheet_button_selected", fMode == mode_system ? true : false);
    ui->btnSystem->style()->polish(ui->btnSystem);
    ui->btnWorkersInOut->setProperty("stylesheet_button_selected", fMode == mode_inout ? true : false);
    ui->btnWorkersInOut->style()->polish(ui->btnWorkersInOut);
}

void DlgScreen::on_btnSystem_clicked()
{
    colorize(mode_system);
}

void DlgScreen::on_btnWorkersInOut_clicked()
{
    colorize(mode_inout);
}
