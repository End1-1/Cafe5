#include "dlgscreen.h"
#include "c5waiterserver.h"
#include "ui_dlgscreen.h"
#include "c5user.h"
#include "dlgface.h"
#include "c5waiterserver.h"
#include "c5cafecommon.h"
#include "c5serverhandler.h"
#include "dlgsystem.h"
#include "dlgmanagertools.h"
#include <QTimer>

DlgScreen::DlgScreen() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgScreen)
{
    ui->setupUi(this);
    connect(&fTcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    fTcpServer.listen(QHostAddress::Any, 1000);

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

    QDate d = QDate::currentDate();
    int dateShift = 1;
    if (__c5config.getValue(param_date_cash_auto).toInt() == 1) {
       QTime t = QTime::fromString(__c5config.getValue(param_working_date_change_time), "HH:mm:ss");
       if (t.isValid()) {
           if (QTime::currentTime() < t) {
               d = d.addDays(-1);
               dateShift = 2;
           }
       }
    } else {
        d = QDate::fromString(__c5config.dateCash(), FORMAT_DATE_TO_STR_MYSQL);
        dateShift = __c5config.dateShift();
    }
    ui->lbDateOfClose->setText(QString("%1: %2")
                               .arg(tr("Date of close"))
                               .arg(QDate::fromString(__c5config.getValue(param_date_cash), FORMAT_DATE_TO_STR_MYSQL).toString(FORMAT_DATE_TO_STR)));

    ui->lbCurrentTime->setText(QString("%1: %2")
                               .arg(tr("System datetime"))
                               .arg(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR)));
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
    QString pass = ui->lePassword->text().replace(";", "").replace("?", "");
    ui->lePassword->clear();
    if (ui->btnHall->isChecked()) {
        C5User u(pass);
        if (!u.isValid()) {
            C5Message::error(u.error());
            return;
        }
        if (!u.check(cp_t5_enter_dlgface)) {
            C5Message::error(tr("You have not permission to enter the working area"));
            return;
        }
        if (u.state() == C5User::usNotAtWork) {
            if (C5Message::question(tr("Worker not registered at the work. Register?")) != QDialog::Accepted) {
                C5Message::error(tr("Worker must be registered at the work to continue."));
                return;
            }
            if (!u.enterWork()) {
                C5Message::info(u.error());
                return;
            } else {
                C5Message::info(tr("Welcome") + " " + u.fullName());
            }
        }
        DlgFace d(&u);
        d.setup();
        d.exec();
    } else if (ui->btnSystem->isChecked()) {
        if (pass.isEmpty()) {
            DlgSystem d(C5Config::dbParams());
            if (!__c5config.dbParams().at(3).isEmpty()) {
                d.setupNoPassword();
            }
            d.exec();
            return;
        }
        if (pass != __c5config.dbParams().at(3)) {
            C5Message::error(tr("Access denied"));
            return;
        }
        go<DlgSystem>(C5Config::dbParams());
    } else if (ui->btnWorkersInOut->isChecked()) {
        C5User ua(0);
        if (!ua.authorize(pass)) {
            C5Message::error(ua.error());
            return;
        }
        switch (ua.state()) {
        case C5User::usAtWork:
            if (!ua.leaveWork()) {
                C5Message::error(ua.error());
            } else {
                C5Message::info(tr("Goodbuy") + " " + ua.fullName());
            }
            break;
        case C5User::usNotAtWork:
            if (!ua.enterWork()) {
                C5Message::error(ua.error());
            } else {
                C5Message::info(tr("Welcome") + " " + ua.fullName());
            }
            break;
        }
    } else if (ui->btnManagerTools->isChecked()) {
        C5User u(pass);
        if (!u.isValid()) {
            C5Message::error(u.error());
            return;
        }
        if (!u.check(cp_t5_manager_tools)) {
            C5Message::error(tr("You have not permission to enter the manager tools"));
            return;
        }
        __user = &u;
        DlgManagerTools d(&u);
        d.exec();
    }
}

void DlgScreen::on_lePassword_returnPressed()
{
    on_btnAccept_clicked();
}

void DlgScreen::tryExit()
{
    if (C5Message::question(tr("Are you sure to close application")) == QDialog::Accepted) {
        qApp->quit();
    }
}

void DlgScreen::on_btnHall_clicked()
{
    ui->btnHall->setChecked(true);
    ui->btnWorkersInOut->setChecked(false);
    ui->btnManagerTools->setChecked(false);
    ui->btnSystem->setChecked(false);
}

void DlgScreen::on_btnWorkersInOut_clicked()
{
    ui->btnHall->setChecked(false);
    ui->btnWorkersInOut->setChecked(true);
    ui->btnManagerTools->setChecked(false);
    ui->btnSystem->setChecked(false);
}

void DlgScreen::on_btnManagerTools_clicked()
{
    ui->btnHall->setChecked(false);
    ui->btnWorkersInOut->setChecked(false);
    ui->btnManagerTools->setChecked(true);
    ui->btnSystem->setChecked(false);
}

void DlgScreen::on_btnSystem_clicked()
{
    ui->btnHall->setChecked(false);
    ui->btnWorkersInOut->setChecked(false);
    ui->btnManagerTools->setChecked(false);
    ui->btnSystem->setChecked(true);
}
