#include "ce5mfproduct.h"
#include "ui_ce5mfproduct.h"
#include "ce5mfprocess.h"
#include "c5selector.h"
#include "c5cache.h"
#include "c5printing.h"
#include "c5printpreview.h"

CE5MFProduct::CE5MFProduct(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5MFProduct)
{
    ui->setupUi(this);
    ui->wt->addColumn(tr("Rownum"), 0)
            .addColumn(tr("Process"), 300)
            .addColumn(tr("Duration"), 80)
            .addColumn(tr("Duration, sec"), 80, true)
            .addColumn(tr("7h goal"), 80, true)
            .addColumn(tr("Goal price"), 80, true)
            .addColumn(tr("Price"), 80, true);
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
    CE5Editor::setId(id);
    C5Database db(fDBParams);
    db[":f_product"] = id;
    db.exec("select p.f_id, ac.f_name as f_processname, p.f_durationsec, p.f_goalprice, p.f_price "
            "from mf_process p "
            "left join mf_actions ac on ac.f_id=p.f_process "
            "where p.f_product=:f_product "
            "order by p.f_rowid ");
    while (db.nextRow()) {
        int row;
        ui->wt->setData(row, 0, db.getInt("f_id"), true)
                .setData(row, 1, db.getString("f_processname"))
                .setData(row, 2, durationStr(db.getInt("f_durationsec")))
                .createLineEdit(row, 3, db.getInt("f_durationsec"), this, SLOT(durationChanged(QString)))
                .setData(row, 4, db.getInt("f_durationsec") == 0 ? 0 : (3600 * 7) / db.getInt("f_durationsec"))
                .createLineEdit(row, 5, db.getDouble("f_goalprice"), this, SLOT(goalPriceChanged(QString)))
                .setData(row, 6, db.getDouble("f_price"));
    }
    ui->wt->countTotal(-1);
}

void CE5MFProduct::clear()
{
    CE5Editor::clear();
    ui->wt->clearTables();
}

bool CE5MFProduct::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    if (CE5Editor::save(err, data)) {
        C5Database db(fDBParams);
        for (int i = 0; i < ui->wt->rowCount(); i++) {
            db[":f_durationsec"] = ui->wt->getData(i, 3);
            db[":f_goalprice"] = ui->wt->lineEdit(i, 5)->getDouble();
            db[":f_price"] = ui->wt->getData(i, 6);
            db[":f_rowid"] = i;
            db.update("mf_process", "f_id", ui->wt->getData(i, 0));
        }
        for (int id: fRemovedRow) {
            db[":f_id"] = id;
            db.exec("delete from mf_process where f_id=:f_id");
        }
    } else {
        return false;
    }
    return true;
}

void CE5MFProduct::durationChanged(const QString &arg1)
{
    int row = static_cast<C5LineEdit*>(sender())->property("row").toInt();
    int sec = arg1.toInt();
    if (sec > 0) {
        ui->wt->setData(row, 2, durationStr(sec));
        ui->wt->setData(row, 4, (3600 * 7) / sec);
        ui->wt->setData(row, 6, ui->wt->getData(row, 4).toDouble() / sec);
    } else {
        ui->wt->setData(row, 2, durationStr(sec));
        ui->wt->setData(row, 4, 0);
        ui->wt->setData(row, 6, 0);
    }
    ui->wt->countTotal(-1);
}

void CE5MFProduct::goalPriceChanged(const QString &arg1)
{
    int row = static_cast<C5LineEdit*>(sender())->property("row").toInt();
    ui->wt->setData(row, 6, str_float(arg1) / ui->wt->getData(row, 4).toInt());
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
    if (ui->leCode->getInteger() == 0){
        C5Message::error(tr("Save first"));
    }
    QList<QVariant> vals;
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
            .setData(row, 1, vals.at(2))
            .setData(row, 2, durationStr(0))
            .createLineEdit(row, 3, 0, this, SLOT(durationChanged(QString)))
            .setData(row, 4, 0)
            .createLineEdit(row, 5, 0, this, SLOT(goalPriceChanged(QString)))
            .setData(row, 6, 0);
}

void CE5MFProduct::on_btnMinus_clicked()
{
    int r = ui->wt->currentRow();
    if (r < 0) {
        return;
    }
    if (C5Message::question(tr("Confirm to remove") + "<br>" + ui->wt->getData(r, 1).toString()) != QDialog::Accepted) {
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
    if (ui->leCode->getInteger() == 0){
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
                .setData(row,1, data.at(0)["f_name"])
                .setData(row, 2, "")
                .createLineEdit(row, 3, 0, this, SLOT(durationChanged(QString)))
                .setData(row, 4, 0)
                .createLineEdit(row, 5, 0, this, SLOT(goalPriceChanged(QString)))
                .setData(row, 6, 0);
    }
    delete e;
}

void CE5MFProduct::on_btnPrint_clicked()
{
    C5Printing p;
    QList<qreal> points;
    QStringList vals;
    p.setSceneParams(2700, 2000, QPrinter::Landscape);
    p.setFontSize(25);
    p.setFontBold(true);
    p.ctext(QString("%1: %2").arg(tr("Product"), ui->leName->text()));
    p.br();
    points << 50 << 100 << 1000 << 250 << 250 << 250 << 250 << 250;
    vals << "NN" << tr("Process") << tr("Duration") << tr("Duration") << tr("7h goal") << tr("Goal price") << tr("Price");
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);

    p.setFontBold(false);
    for (int i = 0; i < ui->wt->rowCount(); i++) {
        vals.clear();
        vals << QString::number(i + 1);
        for (int c = 1; c < ui->wt->columnCount(); c++) {
            vals.append(ui->wt->getData(i, c).toString());
        }
        p.tableText(points, vals, p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
    }

    p.setFontBold(true);
    vals.clear();
    vals << "" << "" << "" << ui->wt->totalStr(3) << "" << ui->wt->totalStr(5) << ui->wt->totalStr(6);
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);

    C5PrintPreview pp(&p, fDBParams);
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
        emit ui->wt->lineEdit(row, 3)->textChanged(ui->wt->lineEdit(row, 3)->text());
        emit ui->wt->lineEdit(row, 5)->textChanged(ui->wt->lineEdit(row, 5)->text());
        emit ui->wt->lineEdit(row - 1, 3)->textChanged(ui->wt->lineEdit(row - 1, 3)->text());
        emit ui->wt->lineEdit(row - 1, 5)->textChanged(ui->wt->lineEdit(row - 1, 5)->text());
    }
}

void CE5MFProduct::on_btnMoveRowDown_clicked()
{
    int row = ui->wt->currentRow();
    if (row < ui->wt->rowCount() - 1) {
        ui->wt->moveRowDown(row);
        emit ui->wt->lineEdit(row, 3)->textChanged(ui->wt->lineEdit(row, 3)->text());
        emit ui->wt->lineEdit(row, 5)->textChanged(ui->wt->lineEdit(row, 5)->text());
        emit ui->wt->lineEdit(row + 1, 3)->textChanged(ui->wt->lineEdit(row + 1, 3)->text());
        emit ui->wt->lineEdit(row + 1, 5)->textChanged(ui->wt->lineEdit(row + 1, 5)->text());
    }
}
