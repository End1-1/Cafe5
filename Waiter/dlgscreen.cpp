#include "dlgscreen.h"
#include "ui_dlgscreen.h"
#include "c5user.h"
#include "dlgface.h"
#include "c5cafecommon.h"
#include "c5tabledata.h"
#include "ndataprovider.h"
#include "c5waiterserver.h"
#include "c5config.h"
#include "c5permissions.h"
#include "c5message.h"
#include <QTimer>

DlgScreen::DlgScreen() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgScreen)
{
    ui->setupUi(this);
    QTimer *t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &DlgScreen::timerTimeout);
    t->start(1000);
    C5Database db(__c5config.dbParams());
    db.exec("select f_settings, f_key, f_value from s_settings_values");
    C5CafeCommon::fHallConfigs.clear();
    while (db.nextRow()) {
        C5CafeCommon::fHallConfigs[db.getInt("f_settings")][db.getInt("f_key")] = db.getString("f_value");
    }
    fHttp->createHttpQuery("/engine/waiter/init.php", QJsonObject{{"version", C5TableData::instance()->version()}, {"dick", 1}},
    SLOT(
        initResponse(QJsonObject)), "", false);
    auto *t2 = new QTimer();
    connect(t2, &QTimer::timeout, this, &DlgScreen::serviceTimeout);
    t2->start(10000);
    mWaiterServer = new C5WaiterServer();
}

DlgScreen::~DlgScreen()
{
    delete ui;
    mWaiterServer->deleteLater();
}

void DlgScreen::initResponse(const QJsonObject &jdoc)
{
    QJsonObject jo = jdoc["data"].toObject();
    if (jo["nochanges"].toBool() == false) {
        C5TableData::instance()->setData(jdoc["data"].toObject());
    }
    fHttp->httpQueryFinished(sender());
}

void DlgScreen::serviceTimeout()
{
    auto *timer = static_cast<QTimer *>(sender());
    timer->stop();
    auto np = new NDataProvider(this);
    np->overwriteHost("http", "127.0.0.1", 8080);
    connect(np, &NDataProvider::done, this, [timer](const QJsonObject &jo) {
        timer->start(3000);
    });
    connect(np, &NDataProvider::error, this, [timer](const QString &err) {
        timer->start(3000);
    });
    np->getData("/mobile", {});
}

void DlgScreen::loginResponse(const QJsonObject &jdoc)
{
    QJsonObject jo = jdoc["data"].toObject();
    NDataProvider::sessionKey = jo["sessionkey"].toString();
    QString pass = sender()->property("marks").toString();
    C5User u(pass);
    if (!u.isValid()) {
        fHttp->httpQueryFinished(sender());
        C5Message::error(u.error());
        return;
    }
    if (!u.check(cp_t5_enter_dlgface)) {
        fHttp->httpQueryFinished(sender());
        C5Message::error(tr("You have not permission to enter the working area"));
        return;
    }
    if (u.state() == C5User::usNotAtWork) {
        if (C5Message::question(tr("Worker not registered at the work. Register?")) != QDialog::Accepted) {
            fHttp->httpQueryFinished(sender());
            C5Message::error(tr("Worker must be registered at the work to continue."));
            return;
        }
        if (!u.enterWork()) {
            fHttp->httpQueryFinished(sender());
            C5Message::info(u.error());
            return;
        } else {
            C5Message::info(tr("Welcome") + " " + u.fullName());
        }
    }
    __user = &u;
    fHttp->httpQueryFinished(sender());
    DlgFace d( &u);
    d.exec();
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
    ui->lePassword->appendText(static_cast<QPushButton *>(sender())->text());
}
void DlgScreen::on_btn2_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton *>(sender())->text());
}

void DlgScreen::on_btn3_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton *>(sender())->text());
}

void DlgScreen::on_btn4_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton *>(sender())->text());
}

void DlgScreen::on_btn5_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton *>(sender())->text());
}

void DlgScreen::on_btn6_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton *>(sender())->text());
}

void DlgScreen::on_btn7_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton *>(sender())->text());
}

void DlgScreen::on_btn8_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton *>(sender())->text());
}

void DlgScreen::on_btn9_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton *>(sender())->text());
}

void DlgScreen::on_btn0_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton *>(sender())->text());
}

void DlgScreen::on_btnP_clicked()
{
    ui->lePassword->appendText(static_cast<QPushButton *>(sender())->text());
}

void DlgScreen::on_btnAccept_clicked()
{
    QString pass = ui->lePassword->text().replace(";", "").replace("?", "");
    ui->lePassword->clear();
    fHttp->createHttpQuery("/engine/login.php", QJsonObject{{"method", 2}, {"pin", pass}}, SLOT(loginResponse(QJsonObject)),
    pass);
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
