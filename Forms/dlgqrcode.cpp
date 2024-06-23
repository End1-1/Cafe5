#include "dlgqrcode.h"
#include "ui_dlgqrcode.h"
#include "dlgqty.h"
#include <QPushButton>

DlgQrCode::DlgQrCode(int dish) :
    C5Dialog(QStringList()),
    ui(new Ui::DlgQrCode)
{
    ui->setupUi(this);
    setProperty("dishid", dish);
    fHttp->createHttpQuery("/engine/waiter/qr-list-list.php", QJsonObject{{"dishid", dish}}, SLOT(openResponse(
                QJsonObject)));
    ui->tbl->setColumnWidth(0, 100);
    ui->tbl->setColumnWidth(1, 100);
    ui->tbl->setColumnWidth(2, 100);
    ui->tbl->setColumnWidth(3, 100);
}

DlgQrCode::~DlgQrCode()
{
    delete ui;
}

void DlgQrCode::openResponse(const QJsonObject &jdoc)
{
    ui->tbl->setRowCount(0);
    ui->lbName->setText(jdoc["dish"].toString());
    for (int i = 0; i < jdoc["qrlist"].toArray().size(); i++) {
        const QJsonObject &j = jdoc["qrlist"].toArray().at(i).toObject();
        int row = ui->tbl->rowCount();
        ui->tbl->setRowCount(row + 1);
        ui->tbl->setItem(row, 0, new QTableWidgetItem(j["f_date"].toString()));
        ui->tbl->item(row, 0)->setData(j["f_id"].toInt(), Qt::UserRole);
        ui->tbl->setItem(row, 1, new QTableWidgetItem(j["f_emarks"].toString()));
        ui->tbl->setItem(row, 2, new QTableWidgetItem(float_str(j["f_qty"].toDouble(), 1)));
        ui->tbl->setItem(row, 3, new QTableWidgetItem(float_str(j["f_remain"].toDouble(), 1)));
        auto *b = new QPushButton(tr("Remove"));
        int id = j["f_id"].toInt();
        connect(b, &QPushButton::clicked, [this, b, id]() {
            for (int c = 0; c < ui->tbl->rowCount(); c++) {
                if (b == ui->tbl->cellWidget(c, 4)) {
                    fHttp->createHttpQuery("/engine/waiter/qr-list-remove.php",
                    QJsonObject{{"id", id}},
                    SLOT(removeResponse(QJsonObject)));
                }
            }
        });
        ui->tbl->setCellWidget(row, 4, b);
    }
    fHttp->httpQueryFinished(sender());
}

void DlgQrCode::appendResponse(const QJsonObject &jdoc)
{
    Q_UNUSED(jdoc);
    fHttp->httpQueryFinished(sender());
    fHttp->createHttpQuery("/engine/waiter/qr-list-list.php", QJsonObject{{"dishid", property("dishid").toInt()}}, SLOT(
        openResponse(QJsonObject)));
}

void DlgQrCode::removeResponse(const QJsonObject &jdoc)
{
    Q_UNUSED(jdoc);
    fHttp->httpQueryFinished(sender());
    fHttp->createHttpQuery("/engine/waiter/qr-list-list.php", QJsonObject{{"dishid", property("dishid").toInt()}}, SLOT(
        openResponse(QJsonObject)));
}

void DlgQrCode::on_btnEdit_clicked()
{
    double max = 999;
    if (!DlgQty::getQty(max, ui->lbName->text())) {
        return;
    }
    ui->leQty->setDouble(max);
}

void DlgQrCode::on_btnClose_clicked()
{
    reject();
}

void DlgQrCode::on_btnSave_clicked()
{
    if (ui->leAppendQr->text().isEmpty()) {
        return;
    }
    if (ui->leAppendQr->text().length() < 15) {
        C5Message::error(tr("Invalid emarks"));
        return;
    }
    if (ui->leQty->getDouble() < 0.01) {
        C5Message::error(tr("Unknown quantity"));
        return;
    }
    fHttp->createHttpQuery("/engine/waiter/qr-list-add.php",
    QJsonObject{{"dishid", property("dishid").toInt()},
        {"emarks", ui->leAppendQr->text()},
        {"qty", ui->leQty->getDouble()}},
    SLOT(appendResponse(QJsonObject)));
    ui->leAppendQr->clear();
    ui->leQty->clear();
}
