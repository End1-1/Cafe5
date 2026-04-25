#include "vieworder.h"
#include <QClipboard>
#include <QJsonObject>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrinterInfo>
#include "c5checkbox.h"
#include "c5message.h"
#include "c5printing.h"
#include "c5printrecipta4.h"
#include "c5user.h"
#include "c5utils.h"
#include "dlgdate.h"
#include "dqty.h"
#include "goodsreturnreason.h"
#include "jsons.h"
#include "ninterface.h"
#include "printreceiptgroup.h"
#include "printtaxn.h"
#include "selectprinters.h"
#include "struct_workstationitem.h"
#include "ui_vieworder.h"
#include "worder.h"
#include "working.h"

using std::function;

ViewOrder::ViewOrder(Working *w, const QString &order, C5User *user)
    : C5ShopDialog(user)
    , ui(new Ui::ViewOrder)
    , fWorking(w)
{
    ui->setupUi(this);
    showMaximized();
    fUuid = order;
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 30, 300, 100, 100, 100, 0, 200);
    //TOPDO
    // todo db;
    // db[":f_id"] = order;
    // db.exec("select o.*, concat(' ', u.f_last, u.f_first) as f_saler, "
    //         "concat_ws(' ', p.f_taxcode, p.f_taxname, p.f_contact, p.f_phone) as "
    //         "f_buyer "
    //         "from o_header o "
    //         "left join s_user u on u.f_id=o.f_staff "
    //         "left join c_partners p on p.f_id=o.f_partner "
    //         "where o.f_id=:f_id");

    // if (db.nextRow()) {
    //     ui->leOrderNum->setText(
    //         QString("%1%2").arg(db.getString("f_prefix")).arg(db.getInt("f_hallid")));
    //     ui->leAmount->setDouble(db.getDouble("f_amounttotal"));

    //     fSaleDoc = QString("%1%2, %3")
    //                    .arg(db.getString("f_prefix"),
    //                         db.getString("f_hallid"),
    //                         db.getDate("f_datecash").toString(FORMAT_DATE_TO_STR));
    //     ui->leDate->setDate(db.getDate("f_datecash"));
    //     ui->leTime->setText(db.getTime("f_timeclose").toString("HH:mm"));
    //     ui->leUUID->setText(db.getString("f_id"));
    //     ui->leCash->setDouble(db.getDouble("f_amountcash"));
    //     ui->leCard->setDouble(db.getDouble("f_amountcard"));
    //     ui->leIdram->setDouble(db.getDouble("f_amountidram"));
    //     ui->leDebt->setDouble(db.getDouble("f_amountdebt"));
    //     ui->leBank->setDouble(db.getDouble("f_amountbank"));
    //     ui->leTelcell->setDouble(db.getDouble("f_amounttelcell"));
    //     ui->leSaler->setText(db.getString("f_saler"));
    //     ui->leBuyer->setText(db.getString("f_buyer"));
    // } else {
    //     C5Message::error(tr("Document is not exists"));
    //     return;
    // }

    // db[":f_id"] = order;
    // db.exec("select concat(' ', u.f_last, u.f_first) as f_deliveryman "
    //         "from o_header_options o "
    //         "left join s_user u on u.f_id=o.f_deliveryman "
    //         "where o.f_id=:f_id");

    // if (db.nextRow()) {
    //     ui->leDeliveryMan->setText(db.getString("f_deliveryman"));
    // }

    // db[":f_header"] = order;
    // db.exec("select b.f_id, g.f_name, g.f_id as f_goodsid, b.f_qty, b.f_price, "
    //         "b.f_total, f_scancode,  "
    //         "g.f_service, b.f_return, t.f_receiptnumber as f_tax, b.f_store "
    //         "from o_goods b "
    //         "left join o_header h on h.f_id=b.f_header "
    //         "left join o_tax t on t.f_id=h.f_id "
    //         "inner join c_goods g on g.f_id=b.f_goods "
    //         "where b.f_header=:f_header "
    //         "order by b.f_row ");

    // while (db.nextRow()) {
    //     int r = ui->tbl->addEmptyRow();
    //     ui->tbl->setString(r, 0, db.getString("f_id"));
    //     ui->tbl->createCheckbox(r, 1);
    //     ui->tbl->setString(r, 2, db.getString("f_name"));
    //     ui->tbl->setDouble(r, 3, db.getDouble("f_qty"));
    //     ui->tbl->setDouble(r, 4, db.getDouble("f_price"));
    //     ui->tbl->setDouble(r, 5, db.getDouble("f_total"));
    //     ui->tbl->setInteger(r, 6, db.getInt("f_goodsid"));
    //     ui->tbl->setString(r, 7, db.getString("f_scancode"));
    //     ui->tbl->setString(r, 8, db.getString("f_service"));
    //     ui->tbl->setInteger(r, 9, db.getInt("f_return"));
    //     ui->tbl->setInteger(r, 10, db.getInt("f_store"));

    //     if (db.getInt("f_return") > 0 || db.getDouble("f_price") < 0) {
    //         ui->tbl->checkBox(r, 1)->setEnabled(false);
    //     }

    //     ui->leTaxNumber->setText(db.getString("f_tax"));
    //     ui->btnPrintFiscal->setVisible(ui->leTaxNumber->getInteger() == 0);
    //     ui->btnTaxReturn->setVisible(!ui->btnPrintFiscal->isVisible());
    // }

    if (ui->leAmount->getDouble() < 0) {
        ui->btnReturn->setVisible(false);
    }

    ui->btnSaveReturn->setVisible(false);
}

ViewOrder::~ViewOrder()
{
    delete ui;
}

void ViewOrder::on_btnReturn_clicked()
{
    if(ui->leDate->date().daysTo(QDate::currentDate()) > 14) {
        C5Message::error(tr("You cannot return this item"));
        return;
    }

    GoodsReturnReason r(mUser);
    r.exec();
    int reason = r.fReason;
    ui->leReturnReason->setProperty("reason", r.fReason);
    ui->leReturnReason->setText(r.fReasonName);

    if(reason == 0) {
        return;
    }

    setProperty("return", true);
    setStyleSheet("background:rgb(255, 210, 217);");
    style()->polish(this);
    ui->btnSaveReturn->setVisible(true);
    ui->leUUID->setProperty("returnfrom", ui->leUUID->text());
    ui->leUUID->clear();
    ui->leDate->setDate(QDate::currentDate());
    ui->btnReturn->setEnabled(false);

    for(int i = 0; i < ui->tbl->rowCount(); i++) {
        ui->tbl->item(i, 3)->setData(Qt::UserRole, ui->tbl->getDouble(i, 3));
    }
}

void ViewOrder::countOrder()
{
    double total = 0;

    for(int i = 0; i < ui->tbl->rowCount(); i++) {
        ui->tbl->setDouble(i, 5, ui->tbl->getDouble(i, 3) *ui->tbl->getDouble(i, 4));
        total += ui->tbl->getDouble(i, 5);
    }

    ui->leAmount->setDouble(total);
}

void ViewOrder::on_btnTaxReturn_clicked()
{
    //TODO
    // todo db;
    // db[":f_id"] = ui->leUUID->text();
    // db.exec("select * from o_tax_log where f_order=:f_id and f_state=1");

    // if(!db.nextRow()) {
    //     C5Message::error(tr("No fiscal exists for this order"));
    //     return;
    // }

    // QJsonObject jout = __strjson(db.getString("f_out"));
    // QString crn = jout["crn"].toString();
    // QString rseq = ui->leTaxNumber->text();
    // FiscalMachine fm = getFiscalMachine(mWorkStation.fiscalMachineId());
    // PrintTaxN pt(fm.ip, fm.port, fm.machinePassword, fm.externalPosString(), fm.opPin, fm.opPassword, this);
    // QString jsnin, jsnout, err;
    // int result;
    // result = pt.printTaxback(rseq.toInt(), crn, jsnin, jsnout, err);
    // db[":f_id"] = db.uuid();
    // db[":f_order"] = fUuid;
    // db[":f_date"] = QDate::currentDate();
    // db[":f_time"] = QTime::currentTime();
    // db[":f_in"] = jsnin;
    // db[":f_out"] = jsnout;
    // db[":f_err"] = err;
    // db[":f_result"] = result;
    // db.insert("o_tax_log", false);

    // if(result != pt_err_ok) {
    //     C5Message::error(err);
    // } else {
    //     db[":f_fiscal"] = QVariant();
    //     db[":f_receiptnumber"] = QVariant();
    //     db[":f_time"] = QVariant();
    //     db.update("o_tax", "f_id", fUuid);
    //     C5Message::info(tr("Taxback complete"));
    // }
}

void ViewOrder::on_btnClose_clicked()
{
    accept();
}

void ViewOrder::on_btnCopyUUID_clicked()
{
    qApp->clipboard()->setText(ui->leUUID->text());
}

void ViewOrder::on_btnEditDeliveryMan_clicked()
{
    //todo
    // QString id, name;

    // if(DlgGetIDName::get(mUser,  id, name, idname_users_fullname) == false) {
    //     return;
    // }

    // if(C5Message::question(tr("Confirm to change the deliveryman")) != QDialog::Accepted) {
    //     return;
    // }

    // db[":f_deliveryman"] = id;

    // if(db.update("o_header_options", "f_id", ui->leUUID->text())) {
    //     ui->leDeliveryMan->setText(name);
    // }
}

void ViewOrder::on_btnEditSaler_clicked()
{
    //TODO
    // QString id, name;

    // if(DlgGetIDName::get(mUser, id, name, idname_users_fullname) == false) {
    //     return;
    // }

    // if(C5Message::question(tr("Confirm to change the saler")) != QDialog::Accepted) {
    //     return;
    // }

    // db[":f_staff"] = id;

    // if(db.update("o_header", "f_id", ui->leUUID->text())) {
    //     ui->leSaler->setText(name);
    // }
}

void ViewOrder::on_btnEditBuyer_clicked()
{
    //todo
    // QString id, name;

    // if(DlgGetIDName::get(mUser, id, name, idname_partners_full) == false) {
    //     return;
    // }

    // if(C5Message::question(tr("Confirm to change the buyer")) != QDialog::Accepted) {
    //     return;
    // }

    // db[":f_partner"] = id;

    // if(db.update("o_header", "f_id", ui->leUUID->text())) {
    //     ui->leBuyer->setText(name);
    // }
}

void ViewOrder::on_btnPrintReceipt_clicked()
{
    if (!mWorkStation.defaultPrinter().isEmpty()) {
        PrintReceiptGroup p;

        p.print2(ui->leUUID->text());
    }
}

void ViewOrder::on_btnPrintFiscal_clicked()
{
    if(C5Message::question(tr("Confirm to print fiscal")) != QDialog::Accepted) {
        return;
    }

    printCheckWithTax(ui->leUUID->text(), [this](auto rseq) { ui->leTaxNumber->setText(rseq); });
}

void ViewOrder::printCheckWithTax(const QString &id, std::function<void(const QString &)> funcSuccess)
{
    NInterface::query1("/engine/v2/common/fiscal/get", mUser->mSessionKey, this, {{"id", id}}, [this, id, funcSuccess](const QJsonObject &jo) {
        if (jo.value("fiscal").toInt() > 0) {
            C5Message::error(tr("Cannot print tax twice"));
            return;
        }

        QJsonObject jheader = jo.value("header").toObject();
        QJsonObject jpartner = jo.value("partner").toObject();
        QJsonArray jgoods = jo.value("goods").toArray();
        double card = jheader.value("f_amountcard").toDouble();
        double idram = jheader.value("f_idram").toDouble();

        FiscalMachine fm = getFiscalMachine(mWorkStation.fiscalMachineId());
        QString useExtPos = idram > 0.01 ? "true" : fm.externalPosString();
        QString partnerTIN = jpartner.value("f_taxcode").toString();

        // Создаем pt в куче (new), так как функция закончится раньше, чем придет ответ от фискалки
        // Указываем 'this' как родителя для автоматической очистки при закрытии окна
        PrintTaxN *pt = new PrintTaxN(fm.ip, fm.port, fm.machinePassword, useExtPos, fm.opPin, fm.opPassword, this);

        if (partnerTIN.length() == 8 && ui->btnPrintPartnerTIN->isChecked()) {
            pt->fPartnerTin = partnerTIN;
        }

        for (int i = 0; i < jgoods.size(); i++) {
            const QJsonObject &jg = jgoods.at(i).toObject();
            pt->addGoods(jg.value("f_taxdept").toInt(),
                         jg.value("f_adgcode").toString(),
                         jg.value("f_goods").toString(),
                         jg.value("f_name").toString(),
                         jg.value("f_price").toDouble(),
                         jg.value("f_qty").toDouble(),
                         jg.value("f_discountfactor").toDouble() * 100);
        }

        // Подписываемся на результат
        connect(pt,
                &PrintTaxN::finished,
                this,
                [this, id, funcSuccess, pt](const QString &jsonIn, const QString &jsonOut, const QString &err, int result) {
                    QJsonObject reply{{"f_id", QUuid::createUuid().toString(QUuid::WithoutBraces)},
                                      {"f_order", id},
                                      {"in", QJsonDocument::fromJson(jsonIn.toUtf8()).object()},
                                      {"out", QJsonDocument::fromJson(jsonOut.toUtf8()).object()},
                                      {"error", err},
                                      {"result", result}};

                    // Логируем результат на сервер
                    NInterface::query(
                        "/engine/v2/common/fiscal/log",
                        mUser->mSessionKey,
                        this,
                        reply,
                        [](const QJsonObject &) {},
                        [](const QJsonObject &) { return true; });

                    if (result == pt_err_ok) {
                        QJsonObject joutObj = QJsonDocument::fromJson(jsonOut.toUtf8()).object();
                        funcSuccess(QString::number(joutObj["rseq"].toInt()));
                        C5Message::info(tr("Printed"));
                    } else {
                        C5Message::error(err.isEmpty() ? tr("Fiscal error") : err);
                    }

                    // Удаляем объект pt после завершения работы
                    pt->deleteLater();
                });

        // Запускаем печать
        pt->makeJsonAndPrint(card, 0);
    });
}

void ViewOrder::on_btnPrintReceiptA4_clicked()
{
    C5PrintReciptA4 p(ui->leUUID->text(), mUser, this);
    QString err;

    if(!p.print(err)) {
        C5Message::error(err);
    }
}

void ViewOrder::on_btnMakeDraft_clicked()
{
    if(ui->leDate->date().daysTo(QDate::currentDate()) > 14) {
        C5Message::error(tr("You cannot return this item"));
        return;
    }

    if(ui->leDate->date().daysTo(QDate::currentDate()) > 14) {
        C5Message::error(tr("You cannot return this item"));
        return;
    }

    if(C5Message::question(tr("Confirm to make draft")) != QDialog::Accepted) {
        return;
    }

    fHttp->createHttpQuery("/engine/shop/make-draft.php", QJsonObject{{"id", ui->leUUID->text()}}, SLOT(
        removeOrderResponse(QJsonObject)));
}

void ViewOrder::on_btnSaveReturn_clicked()
{
    if(ui->leReturnReason->property("reason").toInt() == 0) {
        C5Message::error(tr("he return reason must be specified."));
        return;
    }

    C5Message::info(tr("Return completed"));
}

void ViewOrder::on_tbl_cellClicked(int row, int column)
{
    if(column < 2) {
        return;
    }

    if(!property("return").toBool()) {
        return;
    }

    double maxqty = ui->tbl->item(row, 3)->data(Qt::UserRole).toDouble();
    double newqty = DQty::getQty(tr("Quantity"), maxqty, this);

    if(newqty < 0) {
        return;
    }

    ui->tbl->setDouble(row, 3, newqty);
    countOrder();
}

void ViewOrder::on_btnEditReason_clicked()
{
}

void ViewOrder::on_btnPrintPrices_clicked()
{
    QString ppp = "f_price1";

    if(C5Message::question("Զեղչված՞") == QDialog::Accepted) {
        ppp = "f_price1disc";
    }

    QPrintDialog pd;
    QString printerName;

    if(pd.exec() == QDialog::Accepted) {
        printerName = pd.printer()->printerName();
    } else {
        return;
    }

    //todo
    // db[":f_header"] = ui->leUUID->text();
    // QString sql = R"(
    // SELECT og.f_goods, gc.f_goods as gg,
    // cast(gc.f_qty as unsigned) as f_qty,
    // cast(og.f_qty as  unsigned) as f_qty2,
    // coalesce(gp.%1,0) as fp1,
    // coalesce(gp2.%1,0) as fp2
    // FROM o_goods og
    // LEFT JOIN c_goods g ON g.f_id=og.f_goods
    // LEFT JOIN c_goods_complectation gc ON gc.f_base=og.f_goods and g.f_unit=10
    // LEFT JOIN c_goods_prices gp ON gp.f_goods=gc.f_goods
    // LEFT JOIN c_goods_prices gp2 ON gp2.f_goods=og.f_goods
    // WHERE f_header=:f_header
    // ORDER BY og.f_row desc
    //           )";
    // sql.replace("%1", ppp);
    // db.exec(sql);

    // while(db.nextRow()) {
    //     QFont font(qApp->font());
    //     font.setPointSize(44);
    //     font.setBold(true);
    //     C5Printing p;
    //     QPrinterInfo pi = QPrinterInfo::printerInfo(printerName);
    //     QPrinter printer(pi);
    //     printer.setPageSize(QPageSize::Custom);
    //     printer.setFullPage(false);
    //     QRectF pr = printer.pageRect(QPrinter::DevicePixel);
    //     constexpr qreal SAFE_RIGHT_MM = 2.0;
    //     qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
    //     p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());

    //     if(ppp == "f_price1") {
    //         p.setSceneParams(350, 100, QPageLayout::Portrait);
    //     } else {
    //         p.setSceneParams(350, 100, QPageLayout::Portrait);
    //     }

    //     p.setFont(font);
    //     p.br(2);
    //     p.br(6);
    //     bool print = false;
    //     int qty =  db.getInt("f_qty");
    //     //p.line(4);
    //     p.line();

    //     if(db.getInt("gg") == 0) {
    //         if(db.getDouble("fp2") > 0) {
    //             print = true;
    //             p.ctext(float_str(db.getDouble("fp2"), 1) + "֏");
    //             qty =  db.getInt("f_qty2");
    //         }
    //     } else {
    //         if(db.getDouble("fp1") > 0) {
    //             print = true;
    //             p.ctext(float_str(db.getDouble("fp1"), 1) + "֏");
    //         }
    //     }

    //     if(print) {
    //         for(int i = 0; i < qty; i++) {
    //             p.print(printer);
    //         }
    //     }
    // }
}
