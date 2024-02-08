#include "c5dlgconnections.h"
#include "ui_c5dlgconnections.h"
#include "c5connection.h"
#include <QSettings>
#include <QListWidgetItem>
#include <QJsonDocument>
#include <QJsonObject>

C5DlgConnections::C5DlgConnections(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::C5DlgConnections)
{
    ui->setupUi(this);
    readServers();
}

C5DlgConnections::~C5DlgConnections()
{
    delete ui;
}

void C5DlgConnections::on_btnPlus_clicked()
{
    QJsonObject jo ;
    C5Connection c(jo);
    if (c.exec() == QDialog::Accepted) {
        fServers.append(c.fParams);
        QSettings s(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);
        s.setValue("servers", QJsonDocument(fServers).toJson());
        readServers();
    }
}

void C5DlgConnections::readServers()
{
    QSettings s(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);
    QString jsonStr = s.value("servers", "[]").toString();
    fServers = QJsonDocument::fromJson(jsonStr.toUtf8()).array();
    ui->lst->clear();
    for (int i = 0; i < fServers.count(); i++) {
        const QJsonObject &js = fServers.at(i).toObject();
        QListWidgetItem *item = new QListWidgetItem(ui->lst);
        item->setText(js["name"].toString());
        ui->lst->addItem(item);
    }
}


void C5DlgConnections::on_btnEdit_clicked()
{
    int index = ui->lst->currentRow();
    if (index < 0) {
        return;
    }
    QJsonObject jo = fServers.at(index).toObject();
    C5Connection c(jo);
    if (c.exec() == QDialog::Accepted) {
        fServers[index] = c.fParams;
        QSettings s(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);
        s.setValue("servers", QJsonDocument(fServers).toJson());
        readServers();
    }
}


void C5DlgConnections::on_btnDelete_clicked()
{
    int index = ui->lst->currentRow();
    if (index < 0) {
        return;
    }
    fServers.removeAt(index);
    QSettings s(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);
    s.setValue("servers", QJsonDocument(fServers).toJson());
    readServers();
}

