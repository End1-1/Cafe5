#include "c5route.h"
#include "ui_c5route.h"
#include "c5message.h"
#include "c5checkbox.h"
#include "ce5partner.h"

C5Route::C5Route(const QStringList &dbParams, QWidget *parent) :
    C5Document(dbParams, parent),
    ui(new Ui::C5Route)
{
    ui->setupUi(this);
    fLabel = tr("Route");
    fIcon = ":/route.png";
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 0, 200, 300, 300, 100, 40, 40, 40, 40, 40, 40, 40);
    ui->cbDriver->disconnect(this, SLOT(on_cbDriver_currentIndexChanged(int)));
    ui->cbPartnerStatus->disconnect(this, SLOT(on_cbPartnerStatus_currentIndexChanged(int)));
    ui->cbPartnerStatus->setDBValues(fDBParams, "select f_id, f_name from c_partners_state", 0);
    ui->cbDriver->setDBValues(fDBParams, "select f_id, concat_ws(' ', f_last, f_first) as f_name from s_user", 0);
    connect(ui->cbDriver, SIGNAL(currentIndexChanged(int)), this, SLOT(on_cbDriver_currentIndexChanged(int)));
    connect(ui->cbPartnerStatus, SIGNAL(currentIndexChanged(int)), this, SLOT(on_cbDriver_currentIndexChanged(int)));
}

C5Route::~C5Route()
{
    delete ui;
}

QToolBar *C5Route::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        //btn << ToolBarButtons::tbFilter;
        btn << ToolBarButtons::tbSave;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}

void C5Route::loadPartners()
{
    ui->tbl->clearContents();
    ui->tbl->setRowCount(0);
    C5Database db(fDBParams);
    QString where = " where f_id>0 ";
    if (ui->cbPartnerStatus->currentData().toInt() > 0) {
        where += " and f_state=" + ui->cbPartnerStatus->currentData().toString();
    }
    db.exec(QString("select f_id, f_name, f_address, f_taxcode, f_taxname from c_partners %1 order by 2").arg(where));
    QMap<int, int> rowParner;
    while (db.nextRow()) {
        int r = ui->tbl->addEmptyRow();
        for (int i = 0; i < db.columnCount(); i++) {
            ui->tbl->setString(r, i + 1, db.getString(i));
        }
        for (int i = 6; i < 13; i++) {
            ui->tbl->setItem(r, i, new C5TableWidgetItem());
            ui->tbl->item(r, i)->setCheckState(Qt::Unchecked);
        }
        rowParner[db.getInt(0)] = r;
    }
    db[":f_driver"] = ui->cbDriver->currentData().toInt();
    db.exec("select * from o_route where f_driver=:f_driver");
    while (db.nextRow()) {
        if (!rowParner.contains(db.getInt("f_partner"))) {
            continue;
        }
        int r = rowParner[db.getInt("f_partner")];
        ui->tbl->item(r, 6)->setCheckState(db.getInt("f_1") == 1 ? Qt::Checked : Qt::Unchecked);
        ui->tbl->item(r, 7)->setCheckState(db.getInt("f_2") == 1 ? Qt::Checked : Qt::Unchecked);
        ui->tbl->item(r, 8)->setCheckState(db.getInt("f_3") == 1 ? Qt::Checked : Qt::Unchecked);
        ui->tbl->item(r, 9)->setCheckState(db.getInt("f_4") == 1 ? Qt::Checked : Qt::Unchecked);
        ui->tbl->item(r, 10)->setCheckState(db.getInt("f_5") == 1 ? Qt::Checked : Qt::Unchecked);
        ui->tbl->item(r, 11)->setCheckState(db.getInt("f_6") == 1 ? Qt::Checked : Qt::Unchecked);
        ui->tbl->item(r, 12)->setCheckState(db.getInt("f_7") == 1 ? Qt::Checked : Qt::Unchecked);
        ui->tbl->item(r, 13)->setData(Qt::EditRole, db.getInt("f_q1"));
        ui->tbl->item(r, 14)->setData(Qt::EditRole, db.getInt("f_q2"));
        ui->tbl->item(r, 15)->setData(Qt::EditRole, db.getInt("f_q3"));
        ui->tbl->item(r, 16)->setData(Qt::EditRole, db.getInt("f_q4"));
        ui->tbl->item(r, 17)->setData(Qt::EditRole, db.getInt("f_q5"));
        ui->tbl->item(r, 18)->setData(Qt::EditRole, db.getInt("f_q6"));
        ui->tbl->item(r, 19)->setData(Qt::EditRole, db.getInt("f_q7"));

    }
}

void C5Route::setSearchParameters()
{

}

void C5Route::on_leFilter_textChanged(const QString &arg1)
{
    for (int r = 0; r < ui->tbl->rowCount(); r++) {
        bool rowHidden = true;
        for (int c = 2; c < 6; c++) {
            if (ui->tbl->getString(r, c).contains(arg1, Qt::CaseInsensitive)) {
                rowHidden = false;
                break;
            }
        }
        ui->tbl->setRowHidden(r, rowHidden);
    }
}

void C5Route::saveDataChanges()
{
    QString sql;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (!sql.isEmpty()) {
            sql += ",";
        }
        sql += QString("(%1, %2, %3, %4, %5, %6, %7, %8, %9, "
                        "%10, %11, %12, %13, %14, %15, %16)")
                .arg(ui->cbDriver->currentData().toString(),
                    ui->tbl->getString(i, 1),
                     ui->tbl->item(i, 6)->checkState() == Qt::Checked ? "1" : "0",
                     ui->tbl->item(i, 7)->checkState() == Qt::Checked ? "1" : "0",
                     ui->tbl->item(i, 8)->checkState() == Qt::Checked ? "1" : "0",
                     ui->tbl->item(i, 9)->checkState() == Qt::Checked ? "1" : "0",
                     ui->tbl->item(i, 10)->checkState() == Qt::Checked ? "1" : "0",
                     ui->tbl->item(i, 11)->checkState() == Qt::Checked ? "1" : "0",
                     ui->tbl->item(i, 12)->checkState() == Qt::Checked ? "1" : "0",
                     QString::number(ui->tbl->item(i, 13)->data(Qt::EditRole).toString().toInt()),
                     QString::number(ui->tbl->item(i, 14)->data(Qt::EditRole).toString().toInt()),
                     QString::number(ui->tbl->item(i, 15)->data(Qt::EditRole).toString().toInt()),
                     QString::number(ui->tbl->item(i, 16)->data(Qt::EditRole).toString().toInt()),
                     QString::number(ui->tbl->item(i, 17)->data(Qt::EditRole).toString().toInt()),
                     QString::number(ui->tbl->item(i, 18)->data(Qt::EditRole).toString().toInt()),
                     QString::number(ui->tbl->item(i, 19)->data(Qt::EditRole).toString().toInt())
                     );
    }
    if (!sql.isEmpty()) {
    sql = QString("insert into o_route (f_driver, f_partner, f_1, f_2, f_3, f_4, f_5, f_6, f_7, f_q1, f_q2, f_q3, f_q4, f_q5, f_q6, f_q7) values %1")
            .arg(sql);
    }
    C5Database db(fDBParams);
    db.startTransaction();
    QString where = " where f_driver=" + ui->cbDriver->currentData().toString();
    if (ui->cbPartnerStatus->currentData().toInt() > 0) {
        where += " and f_partner in (select f_id from c_partners where f_state=" + ui->cbPartnerStatus->currentData().toString() + ") ";
    }
    db.exec("delete from o_route " + where);
    if (!sql.isEmpty()) {
        db.exec(sql);
    }
    db.commit();
    C5Message::info(tr("Saved"));
}

void C5Route::on_tbl_doubleClicked(const QModelIndex &index)
{
    int id = ui->tbl->getInteger(index.row(), 1);
    if (id < 1) {
        return;
    }
    CE5Partner *ep = new CE5Partner(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    ep->setId(id);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {

    }
    delete e;
}

void C5Route::on_cbPartnerStatus_currentIndexChanged(int index)
{
    if (ui->tbl->rowCount() > 0) {
        if (C5Message::question(tr("Save before change?")) == QDialog::Accepted) {
            saveDataChanges();
        }
    }
    if (ui->cbDriver->currentData().toInt() > 0) {
        loadPartners();
    }

}

void C5Route::on_cbDriver_currentIndexChanged(int index)
{
    if (ui->tbl->rowCount() > 0) {
        if (C5Message::question(tr("Save before change?")) == QDialog::Accepted) {
            saveDataChanges();
        }
    }
    if (ui->cbDriver->currentData().toInt() > 0) {
        loadPartners();
    }
}

void C5Route::on_tbl_cellChanged(int row, int column)
{
    ui->tbl->setEditTriggers(column > 12 ? QTableWidget::EditTrigger::AllEditTriggers : QTableWidget::EditTrigger::NoEditTriggers);
}
