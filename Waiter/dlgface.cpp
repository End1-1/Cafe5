#include "dlgface.h"
#include "ui_dlgface.h"
#include "c5connection.h"
#include "c5serverhandler.h"
#include "dlgpassword.h"
#include "c5user.h"
#include "dlgorder.h"
#include "c5menu.h"
#include "c5witerconf.h"
#include "c5waiterserver.h"
#include "c5halltabledelegate.h"
#include <QTcpSocket>
#include <QCloseEvent>

#define HALL_COL_WIDTH 175
#define HALL_ROW_HEIGHT 60

DlgFace::DlgFace(QWidget *parent) :
    C5Dialog(parent),
    ui(new Ui::DlgFace)
{
    ui->setupUi(this);
    fCanClose = false;
}

DlgFace::~DlgFace()
{
    delete ui;
}

void DlgFace::setup()
{
    ui->tblHall->setItemDelegate(new C5HallTableDelegate());
    ui->tblHall->horizontalHeader()->setDefaultSectionSize(HALL_COL_WIDTH);
    ui->tblHall->setColumnCount(ui->tblHall->width() / HALL_COL_WIDTH);
    int delta = ui->tblHall->width() - (ui->tblHall->columnCount() * HALL_COL_WIDTH) - 5;
    delta /= ui->tblHall->columnCount();
    ui->tblHall->horizontalHeader()->setDefaultSectionSize(HALL_COL_WIDTH + delta);
    ui->tblHall->verticalHeader()->setDefaultSectionSize(HALL_ROW_HEIGHT);
    connect(&fTcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    fTcpServer.listen(QHostAddress::Any, 1000);
    C5SocketHandler *sh = createSocketHandler(SLOT(handleHall(QJsonObject)));
    sh->bind("cmd", sm_hall);
    sh->send();
    sh = createSocketHandler(SLOT(handleMenu(QJsonObject)));
    sh->bind("cmd", sm_menu);
    sh->send();
    sh = createSocketHandler(SLOT(handleConf(QJsonObject)));
    sh->bind("cmd", sm_waiterconf);
    sh->send();
    connect(&fTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    fTimer.start(5000);
}

void DlgFace::accept()
{
    if (fCanClose) {
        C5Dialog::accept();
    }
}

void DlgFace::reject()
{

}

void DlgFace::timeout()
{
    C5SocketHandler *sh = createSocketHandler(SLOT(handleHall(QJsonObject)));
    sh->bind("cmd", sm_hall);
    sh->send();
}

void DlgFace::newConnection()
{
    QTcpSocket *s = fTcpServer.nextPendingConnection();
    C5SocketHandler *sh = new C5SocketHandler(s, this);
    connect(sh, SIGNAL(handleCommand(QJsonObject)), this, SLOT(handleSocket(QJsonObject)));
}

void DlgFace::handleHall(const QJsonObject &obj)
{
    sender()->deleteLater();
    fHalls = obj["halls"].toArray();
    fTables = obj["tables"].toArray();
    filterHall();
}

void DlgFace::handleMenu(const QJsonObject &obj)
{
    QJsonArray jMenu = obj["menu"].toArray();
    for (int i = 0, count = jMenu.count(); i < count; i++) {
        QJsonObject o = jMenu.at(i).toObject();
        C5Menu::fMenu[o["menu_name"].toString()]
                [o["part1"].toString()]
                [o["part2"].toString()]
                .append(o);
    }
}

void DlgFace::handleConf(const QJsonObject &obj)
{
    Q_UNUSED(obj)
    C5WaiterConf::fDefaultMenu = "M1";
}

void DlgFace::handleSocket(const QJsonObject &obj)
{
    C5SocketHandler *sh = static_cast<C5SocketHandler*>(sender());
    C5WaiterServer ws(obj);
    QJsonObject o;
    ws.reply(o);
    sh->send(o);
    sh->close();
}

void DlgFace::on_btnConnection_clicked()
{
    go<C5Connection>();
}

void DlgFace::on_btnExit_clicked()
{
    if (C5Message::question(tr("Are you sure to close application")) == QDialog::Accepted) {
        fCanClose = true;
        qApp->quit();
    }
}

void DlgFace::filterHall()
{
    ui->tblHall->clearContents();
    ui->tblHall->setRowCount(0);
    int c = 0, r = 0;
    for (int i = 0; i < fTables.count(); i++)  {
        if (c == ui->tblHall->columnCount()) {
            c = 0;
            r++;
        }
        if (r > ui->tblHall->rowCount() - 1) {
            ui->tblHall->setRowCount(r + 1);
            for (int j = 0; j < ui->tblHall->columnCount(); j++) {
                ui->tblHall->setItem(r, j, new QTableWidgetItem());
            }
        }
        QTableWidgetItem *item = ui->tblHall->item(r, c++);
        item->setData(Qt::UserRole, fTables.at(i).toObject());
    }
    ui->tblHall->setRowCount(r + 1);
}

void DlgFace::on_tblHall_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    QJsonObject o = item->data(Qt::UserRole).toJsonObject();
    if (o["f_id"].toString().toInt() == 0) {
        return;
    }
    C5User user;
    if (!DlgPassword::getUser(o["f_name"].toString(), &user)) {
        return;
    }
    DlgOrder::openTable(o, &user);
    C5SocketHandler *sh = createSocketHandler(SLOT(handleHall(QJsonObject)));
    sh->bind("cmd", sm_hall);
    sh->send();
}
