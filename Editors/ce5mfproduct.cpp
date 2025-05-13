#include "ce5mfproduct.h"
#include "jsons.h"
#include "ui_ce5mfproduct.h"
#include "ce5mfprocess.h"
#include "c5selector.h"
#include "c5cache.h"
#include "c5printing.h"
#include "c5printpreview.h"
#include "c5database.h"
#include "c5message.h"
#include "mfprocessproductpriceupdate.h"
#include "ninterface.h"
#include "c5utils.h"
#include <QFileDialog>
#include <QDesktopServices>
#include <QXlsx/header/xlsxdocument.h>
#include <QClipboard>
#include <QMimeData>
#include <QBuffer>

CE5MFProduct::CE5MFProduct(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5MFProduct)
{
    ui->setupUi(this);
#ifdef QT_DEBUG
    int processcolwidth = 100;
#else
    int processcolwidth = 0;
#endif
    ui->wt->addColumn(tr("Rownum"), 0)
    .addColumn(tr("Process code"), processcolwidth)
    .addColumn(tr("Process"), 300)
    .addColumn(tr("Duration"), 80)
    .addColumn(tr("Duration, sec"), 80, true)
    .addColumn(tr("7h goal"), 80, true)
    .addColumn(tr("Goal price"), 80, true)
    .addColumn(tr("Price"), 80, true)
    .addColumn(tr("Update price"), 400, true);
    ui->tblMeas->setColumnWidth(0, 50);
}

CE5MFProduct::~CE5MFProduct()
{
    delete ui;
}

QString CE5MFProduct::title()
{
    return tr("Product");
}

QString CE5MFProduct::table()
{
    return "mf_actions_group";
}

void CE5MFProduct::setId(int id)
{
    QJsonObject jo;
    jo["f_id"] = id;
    jo["mode"] = 1;
    fHttp->createHttpQuery("/engine/production/save-process.php", jo, SLOT(openResponse(QJsonObject)));
    // CE5Editor::setId(id);
    // C5Database db(fDBParams);
    // db[":f_product"] = id;
    // db.exec("select p.f_id, p.f_process, ac.f_name as f_processname, p.f_durationsec, p.f_goalprice, p.f_price, p.f_process "
    //         "from mf_process p "
    //         "left join mf_actions ac on ac.f_id=p.f_process "
    //         "where p.f_product=:f_product "
    //         "order by p.f_rowid ");
    // while (db.nextRow()) {
    //     int row;
    //     ui->wt->setData(row, 0, db.getInt("f_id"), true)
    //     .setData(row, 1, db.getInt("f_process"))
    //     .setData(row, 2, db.getString("f_processname"))
    //     .setData(row, 3, durationStr(db.getInt("f_durationsec")))
    //     .createLineEdit(row, 4, db.getInt("f_durationsec"), this, SLOT(durationChanged(QString)))
    //     .setData(row, 5, db.getInt("f_durationsec") == 0 ? 0 : (3600 * 7) / db.getInt("f_durationsec"))
    //     .createLineEdit(row, 6, db.getDouble("f_goalprice"), this, SLOT(goalPriceChanged(QString)))
    //     .setData(row, 7, db.getDouble("f_price"));
    //     auto *pu = new MFProcessProductPriceUpdate();
    //     connect(pu, &MFProcessProductPriceUpdate::startUpdate, this, &CE5MFProduct::startPriceUpdateOnRow);
    //     ui->wt->setWidget(row, 8, pu);
    // }
    // ui->wt->countTotal(-1);
}

void CE5MFProduct::clear()
{
    CE5Editor::clear();
    ui->wt->clearTables();
    ui->tblMeas->setRowCount(0);
    ui->tblMaterials->setRowCount(0);
    ui->lbImage->clear();
    ui->chDryClean->setChecked(false);
    ui->chIron->setChecked(false);
    ui->chNoSpin->setChecked(false);
    ui->chNoWhite->setChecked(false);
    ui->chWashTemp->setChecked(false);
    ui->lsNoWhite->clear();
    ui->lsWash->clear();
    ui->lsDry->clear();
    ui->lsIron->clear();
    ui->lsNoSpin->clear();
}

bool CE5MFProduct::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    // if (CE5Editor::save(err, data)) {
    //     C5Database db(fDBParams);
    //     for (int i = 0; i < ui->wt->rowCount(); i++) {
    //         db[":f_durationsec"] = ui->wt->getData(i, 4);
    //         db[":f_goalprice"] = ui->wt->lineEdit(i, 6)->getDouble();
    //         db[":f_price"] = ui->wt->getData(i, 7);
    //         db[":f_rowid"] = i;
    //         db.update("mf_process", "f_id", ui->wt->getData(i, 0));
    //     }
    //     for (int id : fRemovedRow) {
    //         db[":f_id"] = id;
    //         db.exec("delete from mf_process where f_id=:f_id");
    //     }
    // } else {
    //     return false;
    // }
    QJsonObject jo;
    jo["mode"] = 2;
    QJsonObject j;
    j["f_id"] = ui->leCode->getInteger();
    j["f_name"] = ui->leName->text();
    jo["main"] = j;
    QJsonArray ja;
    for (int i = 0; i < ui->wt->rowCount(); i++) {
        j = QJsonObject();
        j["f_process"] = ui->wt->getData(i, 1).toInt();
        j["f_durationsec"] = ui->wt->getData(i, 4).toInt();
        j["f_goalprice"] = ui->wt->lineEdit(i, 6)->getDouble();
        j["f_price"] = ui->wt->getData(i, 7).toDouble();
        j["f_rowid"] = i;
        ja.append(j);
    }
    jo["process"] = ja;
    QJsonObject body;
    ja = QJsonArray();
    for (int i = 0; i < ui->tblMeas->rowCount(); i++) {
        j = QJsonObject();
        j["f_name"] = ui->tblMeas->item(i, 1)->text();
        j["f_value"] = ui->tblMeas->item(i, 2)->text();
        j["f_add"] = ui->tblMeas->item(i, 3)->text();
        ja.append(j);
    }
    body["meas"] = ja;
    ja = QJsonArray();
    for (int i = 0; i < ui->tblMaterials->rowCount(); i++) {
        j = QJsonObject();
        j["f_name"] = ui->tblMaterials->item(i, 1)->text();
        j["f_value"] = ui->tblMaterials->item(i, 2)->text();
        j["f_add"] = ui->tblMaterials->item(i, 3)->text();
        ja.append(j);
    }
    body["materials"] = ja;
    QByteArray bimage;
    QBuffer buff( &bimage);
    buff.open(QIODevice::WriteOnly);
    QPixmap p = ui->lbImage->pixmap();
    p.save( &buff, "JPG");
    body["image"] = QString(bimage.toBase64());
    j = QJsonObject();
    j["chwash"] = ui->chWashTemp->isChecked();
    j["lewash"] = ui->lsWash->text();
    j["chiron"] = ui->chIron->isChecked();
    j["leiron"] = ui->lsWash->text();
    j["chnowhite"] = ui->chNoWhite->isChecked();
    j["lsnowhite"] = ui->lsNoWhite->text();
    j["chdryclean"] = ui->chDryClean->isChecked();
    j["lsdryclean"] = ui->lsDry->text();
    j["chnospin"] = ui->chNoSpin->isChecked();
    j["lsnospin"] = ui->lsNoSpin->text();
    body["add"] = j;
    jo["body"] = body;
    fHttp->createHttpQuery("/engine/production/save-process.php", jo, SLOT(saveResponse(QJsonObject)));
    return true;
}

void CE5MFProduct::openResponse(const QJsonObject &jdoc)
{
    QJsonObject jo = jdoc["data"].toObject();
    QJsonObject jmain = jo["main"].toObject();
    ui->leCode->setInteger(jmain["f_id"].toInt());
    ui->leName->setText(jmain["f_name"].toString());
    QJsonArray ja =  jo ["process"].toArray();
    for(int i = 0; i < ja.size(); i++) {
        int row;
        const QJsonObject &j = ja.at(i).toObject();
        ui->wt->setData(row, 0, j["f_id"].toInt(), true)
        .setData(row, 1, j["f_process"].toInt())
        .setData(row, 2, j["f_processname"].toString())
        .setData(row, 3, durationStr(j["f_durationsec"].toInt()))
        .createLineEdit(row, 4, j["f_durationsec"].toInt(), this, SLOT(durationChanged(QString)))
        .setData(row, 5, j["f_durationsec"].toInt() == 0 ? 0 : (3600 * 7) / j["f_durationsec"].toInt())
        .createLineEdit(row, 6, j["f_goalprice"].toDouble(), this, SLOT(goalPriceChanged(QString)))
        .setData(row, 7, j["f_price"].toDouble());
        auto *pu = new MFProcessProductPriceUpdate();
        connect(pu, &MFProcessProductPriceUpdate::startUpdate, this, &CE5MFProduct::startPriceUpdateOnRow);
        ui->wt->setWidget(row, 8, pu);
    }
    ui->wt->countTotal(-1);
    jmain = __strjson(jmain["f_data"].toString());
    ja = jmain["meas"].toArray();
    for (int i = 0; i < ja.size(); i++) {
        int r = ui->tblMeas->rowCount();
        ui->tblMeas->setRowCount(r + 1);
        const QJsonObject &j = ja.at(i).toObject();
        for (int c = 0; c < ui->tblMeas->columnCount(); c++) {
            ui->tblMeas->setItem(r, c, new QTableWidgetItem());
        }
        ui->tblMeas->item(r, 0)->setText(QString::number(r + 1));
        ui->tblMeas->item(r, 1)->setText(j["f_name"].toString());
        ui->tblMeas->item(r, 2)->setText(j["f_value"].toString());
        ui->tblMeas->item(r, 3)->setText(j["f_add"].toString());
    }
    ja = jmain["materials"].toArray();
    for (int i = 0; i < ja.size(); i++) {
        int r = ui->tblMaterials->rowCount();
        ui->tblMaterials->setRowCount(r + 1);
        const QJsonObject &j = ja.at(i).toObject();
        for (int c = 0; c < ui->tblMaterials->columnCount(); c++) {
            ui->tblMaterials->setItem(r, c, new QTableWidgetItem());
        }
        ui->tblMaterials->item(r, 0)->setText(QString::number(r + 1));
        ui->tblMaterials->item(r, 1)->setText(j["f_name"].toString());
        ui->tblMaterials->item(r, 2)->setText(j["f_value"].toString());
        ui->tblMaterials->item(r, 3)->setText(j["f_add"].toString());
    }
    QByteArray bimage = QByteArray::fromBase64(jmain["image"].toString().toLatin1());
    QPixmap p;
    p.loadFromData(bimage);
    ui->lbImage->setPixmap(p);
    QJsonObject j = jmain["add"].toObject();
    ui->chWashTemp->setChecked(j["chwash"].toBool());
    ui->lsWash->setText(j["lewash"].toString());
    ui->chIron->setChecked(j["chiron"].toBool());
    ui->lsWash->setText(j["leiron"].toString());
    ui->chNoWhite->setChecked(j["chnowhite"].toBool());
    ui->lsNoWhite->setText(j["lsnowhite"].toString());
    ui->chDryClean->setChecked(j["chdryclean"].toBool());
    ui->lsDry->setText(j["lsdryclean"].toString());
    ui->chNoSpin->setChecked(j["chnospin"].toBool());
    ui->lsNoSpin->setText(j["lsnospin"].toString());
    fHttp->httpQueryFinished(sender());
}

void CE5MFProduct::saveResponse(const QJsonObject &jdoc)
{
    QJsonObject jo = jdoc;
    fHttp->httpQueryFinished(sender());
}

void CE5MFProduct::startPriceUpdateOnRow()
{
    int row, column;
    auto *pu = static_cast<MFProcessProductPriceUpdate *>(sender());
    if (ui->wt->findWidget(row, column, pu) == false) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_product"] = ui->leCode->getInteger();
    db[":f_process"] = ui->wt->getData(row, 1);
    db[":f_price"] = ui->wt->getData(row, 7);
    db[":f_date1"] = pu->date1();
    db[":f_date2"] = pu->date2();
    db.exec("update mf_daily_process set f_price=:f_price where f_product=:f_product and f_process=:f_process and f_date between :f_date1 and :f_date2");
    C5Message::info(tr("Done."));
}

void CE5MFProduct::durationChanged(const QString &arg1)
{
    int row = static_cast<C5LineEdit *>(sender())->property("row").toInt();
    int sec = arg1.toInt();
    if (sec > 0) {
        ui->wt->setData(row, 3, durationStr(sec));
        ui->wt->setData(row, 5, (3600 * 7) / sec);
        ui->wt->setData(row, 7, ui->wt->getData(row, 5).toDouble() / sec);
    } else {
        ui->wt->setData(row, 3, durationStr(sec));
        ui->wt->setData(row, 5, 0);
        ui->wt->setData(row, 7, 0);
    }
    ui->wt->countTotal(-1);
}

void CE5MFProduct::goalPriceChanged(const QString &arg1)
{
    int row = static_cast<C5LineEdit *>(sender())->property("row").toInt();
    ui->wt->setData(row, 7, str_float(arg1) / ui->wt->getData(row, 5).toInt());
}

QString CE5MFProduct::durationStr(int sec)
{
    int h = sec / 3600;
    int m = (sec - (h * 3600)) / 60;
    int s = sec % 60;
    QString result;
    if (h > 0) {
        result += QString(" %1%2").arg(h).arg(tr("H"));
    }
    if (m > 0) {
        result += QString(" %1%2").arg(m).arg(tr("M"));
    }
    if (s > 0) {
        result += QString(" %1%2").arg(s).arg(tr("S"));
    }
    if (result.isEmpty()) {
        result = "-";
    }
    return result;
}

void CE5MFProduct::on_btnAdd_clicked()
{
    if (ui->leCode->getInteger() == 0) {
        C5Message::error(tr("Save first"));
    }
    QJsonArray vals;
    if (!C5Selector::getValue(fDBParams, cache_mf_actions, vals)) {
        return;
    }
    if (vals.at(1).toInt() == 0) {
        C5Message::error(tr("Could not add goods without code"));
        return;
    }
    C5Database db(fDBParams);
    db[":f_rowid"] = ui->wt->rowCount();
    db[":f_product"] = ui->leCode->getInteger();
    db[":f_process"] = vals.at(1);
    db[":f_durationsec"] = 0;
    db[":f_goalprice"] = 0;
    db[":f_price"] = 0;
    int id = db.insert("mf_process");
    int row;
    ui->wt->setData(row, 0, id, true)
    .setData(row, 1, vals.at(1))
    .setData(row, 2, vals.at(2))
    .setData(row, 3, durationStr(0))
    .createLineEdit(row, 4, 0, this, SLOT(durationChanged(QString)))
    .setData(row, 5, 0)
    .createLineEdit(row, 6, 0, this, SLOT(goalPriceChanged(QString)))
    .setData(row, 7, 0);
}

void CE5MFProduct::on_btnMinus_clicked()
{
    int r = ui->wt->currentRow();
    if (r < 0) {
        return;
    }
    if (C5Message::question(tr("Confirm to remove") + "<br>" + ui->wt->getData(r, 2).toString()) != QDialog::Accepted) {
        return;
    }
    if (ui->wt->getData(r, 0).toInt() > 0) {
        fRemovedRow.append(ui->wt->getData(r, 0).toInt());
    }
    ui->wt->removeRow(r);
    ui->wt->countTotal(-1);
}

void CE5MFProduct::on_btnNew_clicked()
{
    if (ui->leCode->getInteger() == 0) {
        C5Message::error(tr("Save first"));
    }
    auto *ep = new CE5MFProcess(fDBParams);
    auto *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        if (data.at(0)["f_id"].toInt() == 0) {
            C5Message::error(tr("Cannot add goods without code"));
            return;
        }
        C5Database db(fDBParams);
        int row;
        ui->wt->setData(row, 0, data.at(0)["f_id"])
        .setData(row, 1, data.at(0)["f_process"])
        .setData(row, 2, data.at(0)["f_name"])
        .setData(row, 3, "")
        .createLineEdit(row, 4, 0, this, SLOT(durationChanged(QString)))
        .setData(row, 5, 0)
        .createLineEdit(row, 6, 0, this, SLOT(goalPriceChanged(QString)))
        .setData(row, 7, 0);
    }
    delete e;
}

void CE5MFProduct::on_btnPrint_clicked()
{
    C5Printing p;
    QList<qreal> points;
    QStringList vals;
    p.setSceneParams(2000, 2700, QPageLayout::Portrait);
    p.setFontSize(25);
    p.setFontBold(true);
    p.ctext(QString("%1: %2").arg(tr("Product"), ui->leName->text()));
    p.br();
    points << 50 << 100 << 500 << 250 << 250 << 250 << 250 << 250;
    vals << "NN" << tr("Process") << tr("Duration") << tr("Duration") << tr("7h goal") << tr("Goal price") << tr("Price");
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.setFontBold(false);
    for (int i = 0; i < ui->wt->rowCount(); i++) {
        vals.clear();
        vals << QString::number(i + 1);
        for (int c = 2; c < ui->wt->columnCount(); c++) {
            vals.append(ui->wt->getData(i, c).toString());
        }
        p.tableText(points, vals, p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
    }
    p.setFontBold(true);
    vals.clear();
    vals << "" << "" << "" << ui->wt->totalStr(4) << "" << ui->wt->totalStr(6) << ui->wt->totalStr(7);
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    C5PrintPreview pp( &p, fDBParams);
    pp.exec();
}

void CE5MFProduct::on_btnClear_clicked()
{
    if (C5Message::question(tr("Confirm to clear whole process")) != QDialog::Accepted) {
        return;
    }
    for (int i = 0; i < ui->wt->rowCount(); i++) {
        fRemovedRow.append(ui->wt->getData(i, 0).toInt());
    }
    ui->wt->clearTables();
    ui->wt->countTotal(-1);
}

void CE5MFProduct::on_btnMoveRowUp_clicked()
{
    int row = ui->wt->currentRow();
    if (row > 0) {
        ui->wt->moveRowUp(row);
        emit ui->wt->lineEdit(row, 4)->textChanged(ui->wt->lineEdit(row, 4)->text());
        emit ui->wt->lineEdit(row, 6)->textChanged(ui->wt->lineEdit(row, 6)->text());
        emit ui->wt->lineEdit(row - 1, 4)->textChanged(ui->wt->lineEdit(row - 1, 4)->text());
        emit ui->wt->lineEdit(row - 1, 6)->textChanged(ui->wt->lineEdit(row - 1, 6)->text());
    }
}

void CE5MFProduct::on_btnMoveRowDown_clicked()
{
    int row = ui->wt->currentRow();
    if (row < ui->wt->rowCount() - 1) {
        ui->wt->moveRowDown(row);
        emit ui->wt->lineEdit(row, 4)->textChanged(ui->wt->lineEdit(row, 4)->text());
        emit ui->wt->lineEdit(row, 6)->textChanged(ui->wt->lineEdit(row, 6)->text());
        emit ui->wt->lineEdit(row + 1, 4)->textChanged(ui->wt->lineEdit(row + 1, 4)->text());
        emit ui->wt->lineEdit(row + 1, 6)->textChanged(ui->wt->lineEdit(row + 1, 6)->text());
    }
}

void CE5MFProduct::on_btnUpdatePrices_clicked()
{
    C5Database db(fDBParams);
    for (int i = 0; i < ui->wt->rowCount(); i++) {
        db[":f_product"] = ui->leCode->getInteger();
        db[":f_process"] = ui->wt->getData(i, 1);
        db[":f_price"] = ui->wt->getData(i, 7);
        db[":f_date1"] = ui->leDate1->date();
        db[":f_date2"] = ui->leDate2->date();
        db.exec("update mf_daily_process set f_price=:f_price where f_product=:f_product and f_process=:f_process and f_date between :f_date1 and :f_date2");
    }
    C5Message::info(tr("Done."));
}

void CE5MFProduct::on_chUpdatePrice_clicked(bool checked)
{
    ui->leDate1->setEnabled(checked);
    ui->leDate2->setEnabled(checked);
    ui->btnUpdatePrices->setEnabled(checked);
}

void CE5MFProduct::on_btnCopy_clicked()
{
    QString clipdata;
    for (int r = 0; r < ui->wt->rowCount(); r++) {
        for (int c = 0; c < ui->wt->columnCount(); c++) {
            clipdata += ui->wt->getData(r, c, Qt::EditRole).toString() + "\t";
        }
        clipdata += "\r";
    }
    qApp->clipboard()->setText(clipdata);
}

void CE5MFProduct::on_btnPaste_clicked()
{
    QString clipdata = qApp->clipboard()->text();
    QStringList rows = clipdata.split("\r", Qt::SkipEmptyParts);
    C5Database db(fDBParams);
    for (int i = 0; i < rows.count(); i++) {
        QStringList cols = rows.at(i).split("\t", Qt::SkipEmptyParts);
        if (cols.count() < 8) {
            return;
        }
        db[":f_rowid"] = ui->wt->rowCount();
        db[":f_product"] = ui->leCode->getInteger();
        db[":f_process"] = cols.at(1);
        db[":f_durationsec"] = cols.at(4);
        db[":f_goalprice"] = cols.at(6).toDouble();
        db[":f_price"] = cols.at(5).toDouble();
        int id = db.insert("mf_process");
        int row;
        ui->wt->setData(row, 0, id, true)
        .setData(row, 1, cols.at(1).toInt())
        .setData(row, 2, cols.at(2))
        .setData(row, 3, durationStr(cols.at(3).toInt()))
        .createLineEdit(row, 4, cols.at(4), this, SLOT(durationChanged(QString)))
        .setData(row, 5, cols.at(5).toDouble())
        .createLineEdit(row, 6, cols.at(6), this, SLOT(goalPriceChanged(QString)))
        .setData(row, 7, cols.at(7).toDouble());
        ui->wt->lineEdit(row, 4)->setDouble(str_float(cols.at(4)));
        emit ui->wt->lineEdit(row, 4)->textChanged(ui->wt->lineEdit(row, 4)->text());
        ui->wt->lineEdit(row, 6)->setDouble(str_float(cols.at(6)));
        emit ui->wt->lineEdit(row, 6)->textChanged(ui->wt->lineEdit(row, 6)->text());
    }
}

void CE5MFProduct::on_btnExportExcel_clicked()
{
    int colCount = 8;
    int rowCount = ui->wt->rowCount();
    if (colCount == 0 || rowCount == 0) {
        C5Message::info(tr("Empty report!"));
        return;
    }
    QXlsx::Document d;
    d.addSheet("Sheet1");
    /* HEADER */
    QColor color = QColor::fromRgb(200, 200, 250);
    QFont headerFont(qApp->font());
    headerFont.setBold(true);
    QXlsx::Format hf;
    hf.setFont(headerFont);
    hf.setBorderStyle(QXlsx::Format::BorderThin);
    hf.setPatternBackgroundColor(color);
    for (int i = 0; i < colCount; i++) {
        d.write(1, i + 1, ui->wt->columnTitle(i), hf);
        d.setColumnWidth(i + 1, ui->wt->columnWidth(i) / 7);
    }
    /* BODY */
    QFont bodyFont(qApp->font());
    QXlsx::Format bf;
    bf.setFont(bodyFont);
    bf.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    bf.setBorderStyle(QXlsx::Format::BorderThin);
    for (int j = 0; j < rowCount; j++) {
        for (int i = 0; i < colCount; i++) {
            if (ui->wt->lineEdit(j, i) == nullptr) {
                d.write(j + 2, i + 1, ui->wt->getData(j, i, Qt::EditRole), bf);
            } else {
                d.write(j + 2, i + 1, ui->wt->lineEdit(j, i)->text(), bf);
            }
        }
    }
    /* TOTALS ROWS */
    d.write(1 + ui->wt->rowCount() + 1, 4 + 1, ui->wt->total(4), hf);
    d.write(1 + ui->wt->rowCount() + 1, 5 + 1, ui->wt->total(5), hf);
    d.write(1 + ui->wt->rowCount() + 1, 6 + 1, ui->wt->total(6), hf);
    d.write(1 + ui->wt->rowCount() + 1, 7 + 1, ui->wt->total(7), hf);
    QString filename = QFileDialog::getSaveFileName(nullptr, "", "", "*.xlsx");
    if (filename.isEmpty()) {
        return;
    }
    d.saveAs(filename);
    QDesktopServices::openUrl(filename);
}

void CE5MFProduct::on_btnAdd_2_clicked()
{
    int row = ui->tblMeas->rowCount();
    ui->tblMeas->setRowCount(row + 1);
    for (int i = 0; i < ui->tblMeas->columnCount(); i++) {
        ui->tblMeas->setItem(row, i, new QTableWidgetItem());
    }
    for (int i = 0; i < ui->tblMeas->rowCount(); i++) {
        ui->tblMeas->item(i, 0)->setText(QString::number(i + 1));
    }
}

void CE5MFProduct::on_btnAddImage_clicked()
{
    // if (ui->leCode->getInteger() == 0) {
    //     C5Message::info(tr("You should to save before")) ;
    //     return;
    // }
    QString fn = QFileDialog::getOpenFileName(this, tr("Image"), "", "*.jpg;*.png;*.bmp");
    if (fn.isEmpty()) {
        return;
    }
    QPixmap pm;
    if (!pm.load(fn)) {
        C5Message::error(tr("Could not load image"));
        return;
    }
    ui->lbImage->setPixmap(pm.scaled(ui->lbImage->size(), Qt::KeepAspectRatio));
    QByteArray ba;
    do {
        pm = pm.scaled(pm.width() * 0.8,  pm.height() * 0.8);
        ba.clear();
        QBuffer buff( &ba);
        buff.open(QIODevice::WriteOnly);
        pm.save( &buff, "JPG");
    } while (ba.size() > 100000);
}

void CE5MFProduct::on_btnAdd_3_clicked()
{
    int row = ui->tblMaterials->rowCount();
    ui->tblMaterials->setRowCount(row + 1);
    for (int i = 0; i < ui->tblMaterials->columnCount(); i++) {
        ui->tblMaterials->setItem(row, i, new QTableWidgetItem());
    }
    for (int i = 0; i < ui->tblMaterials->rowCount(); i++) {
        ui->tblMaterials->item(i, 0)->setText(QString::number(i + 1));
    }
}

void CE5MFProduct::on_btnMinus_2_clicked()
{
    int r = ui->tblMeas->currentRow();
    if (r > -1) {
        ui->tblMeas->removeRow(r);
    }
}

void CE5MFProduct::on_btnClear_2_clicked()
{
    ui->tblMeas->setRowCount(0);
}

void CE5MFProduct::on_btnMinus_3_clicked()
{
    ui->tblMaterials->removeRow(ui->tblMaterials->currentRow());
}

void CE5MFProduct::on_btnClearTblMaterials_clicked()
{
    ui->tblMaterials->setRowCount(0);
}

void CE5MFProduct::on_btnCopy_2_clicked()
{
    QStringList brd;
    for (int i = 0; i < ui->tblMeas->rowCount(); i++) {
        QStringList r;
        for (int j = 0; j < ui->tblMeas->columnCount(); j++) {
            r.append(ui->tblMeas->item(i, j)->text());
        }
        brd.append(r.join("\t"));
    }
    QMimeData *m = new QMimeData();
    m->setData("copy2",  brd.join("\n").toUtf8());
    qApp->clipboard()->setMimeData(m);
}

void CE5MFProduct::on_btnPaste_2_clicked()
{
    QStringList rows = QString(qApp->clipboard()->mimeData()->data("copy2")).split("\n", Qt::SkipEmptyParts);
    for (const QString &r : rows) {
        QStringList rr = r.split("\t", Qt::SkipEmptyParts);
        int ir = ui->tblMeas->rowCount();
        ui->tblMeas->setRowCount(ir + 1);
        for (int j = 1; j < ui->tblMeas->columnCount(); j++) {
            ui->tblMeas->setItem(ir, j, new QTableWidgetItem(rr.at(j)));
        }
        ui->tblMeas->setItem(ir, 0, new QTableWidgetItem(QString::number(ir + 1)));
    }
}

void CE5MFProduct::on_btnCopy_3_clicked()
{
    QStringList brd;
    for (int i = 0; i < ui->tblMaterials->rowCount(); i++) {
        QStringList r;
        for (int j = 0; j < ui->tblMaterials->columnCount(); j++) {
            r.append(ui->tblMaterials->item(i, j)->text());
        }
        brd.append(r.join("\t"));
    }
    QMimeData *m = new QMimeData();
    m->setData("copy3",  brd.join("\n").toUtf8());
    qApp->clipboard()->setMimeData(m);
}

void CE5MFProduct::on_btnPaste_3_clicked()
{
    QStringList rows = QString(qApp->clipboard()->mimeData()->data("copy3")).split("\n", Qt::SkipEmptyParts);
    for (const QString &r : rows) {
        QStringList rr = r.split("\t", Qt::SkipEmptyParts);
        int ir = ui->tblMaterials->rowCount();
        ui->tblMaterials->setRowCount(ir + 1);
        for (int j = 1; j < ui->tblMaterials->columnCount(); j++) {
            ui->tblMaterials->setItem(ir, j, new QTableWidgetItem(rr.at(j)));
        }
        ui->tblMaterials->setItem(ir, 0, new QTableWidgetItem(QString::number(ir + 1)));
    }
}
