#include "c5route.h"
#include "ui_c5route.h"
#include "c5message.h"
#include "c5database.h"
#include "ce5partner.h"

C5Route::C5Route(const QStringList &dbParams, QWidget *parent) :
    C5Document(dbParams, parent),
    ui(new Ui::C5Route)
{
    ui->setupUi(this);
    fLabel = tr("Route");
    fIcon = ":/route.png";
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 0, 80, 200, 300, 300, 100, 40, 40, 40, 40, 40, 40, 40);
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
    db.exec("select f_rounds from o_route_round where f_id=1");
    db.nextRow();
    int round = db.getInt(0);
    ui->tbl->setColumnCount(7 + (round * 8));
    for (int r = 1; r < round + 1; r++) {
        for (int w = 1; w < 9; w++) {
            int c = 6 + ((r - 1) * 8) + w;
            qDebug() << c;
            if (w < 8) {
                ui->tbl->setHorizontalHeaderItem(c, new QTableWidgetItem(QString("%1-%2").arg(r).arg(w)));
            } else {
                ui->tbl->setHorizontalHeaderItem(c, new QTableWidgetItem(QString("Q%1").arg(r)));
            }
        }
    }
    for (int i = 7; i < ui->tbl->columnCount(); i++) {
        ui->tbl->setColumnWidth(i, 30);
    }
    QString where = " where p.f_id>0 ";
    if (ui->cbPartnerStatus->currentData().toInt() > 0) {
        where += " and f_state=" + ui->cbPartnerStatus->currentData().toString();
    }
    db.exec(QString("select p.f_id, ps.f_name, p.f_name,  p.f_address, p.f_taxcode, p.f_taxname "
                    "from c_partners p "
                    "left join c_partners_state ps on ps.f_id=p.f_state "
                    "%1 order by 2").arg(where));
    QMap<int, int> rowParner;
    while (db.nextRow()) {
        int r = ui->tbl->addEmptyRow();
        for (int i = 0; i < db.columnCount(); i++) {
            ui->tbl->setString(r, i + 1, db.getString(i));
        }
        for (int ro = 0; ro < round; ro++) {
            for (int i = 7; i < 14; i++) {
                int c = i + (ro * 8);
                ui->tbl->setItem(r, c, new C5TableWidgetItem());
                ui->tbl->item(r, c)->setCheckState(Qt::Unchecked);
            }
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
        int coloffcet = (db.getInt("f_round") - 1) * 8;
        ui->tbl->item(r, 7 + coloffcet)->setCheckState(db.getInt("f_1") == 1 ? Qt::Checked : Qt::Unchecked);
        ui->tbl->item(r, 8 + coloffcet)->setCheckState(db.getInt("f_2") == 1 ? Qt::Checked : Qt::Unchecked);
        ui->tbl->item(r, 9 + coloffcet)->setCheckState(db.getInt("f_3") == 1 ? Qt::Checked : Qt::Unchecked);
        ui->tbl->item(r, 10 + coloffcet)->setCheckState(db.getInt("f_4") == 1 ? Qt::Checked : Qt::Unchecked);
        ui->tbl->item(r, 11 + coloffcet)->setCheckState(db.getInt("f_5") == 1 ? Qt::Checked : Qt::Unchecked);
        ui->tbl->item(r, 12 + coloffcet)->setCheckState(db.getInt("f_6") == 1 ? Qt::Checked : Qt::Unchecked);
        ui->tbl->item(r, 13 + coloffcet)->setCheckState(db.getInt("f_7") == 1 ? Qt::Checked : Qt::Unchecked);
        ui->tbl->item(r, 14 + coloffcet)->setData(Qt::EditRole, db.getInt("f_q"));
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
    C5Database db(fDBParams);
    db.exec("select * from o_route_round where f_Id=1");
    db.nextRow();
    int round = db.getInt("f_rounds");
    QString sql;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        for (int r = 0; r < round; r++) {
            if (!sql.isEmpty()) {
                sql += ",";
            }
            int coloffcet = r * 8;
            sql += QString("(%1, %2, %3, %4, %5, %6, %7, %8, %9, "
                           "%10, %11)")
                   .arg(QString::number(r + 1),
                        ui->cbDriver->currentData().toString(),
                        ui->tbl->getString(i, 1),
                        ui->tbl->item(i, 7 + coloffcet)->checkState() == Qt::Checked ? "1" : "0",
                        ui->tbl->item(i, 8 + coloffcet)->checkState() == Qt::Checked ? "1" : "0",
                        ui->tbl->item(i, 9 + coloffcet)->checkState() == Qt::Checked ? "1" : "0",
                        ui->tbl->item(i, 10 + coloffcet)->checkState() == Qt::Checked ? "1" : "0",
                        ui->tbl->item(i, 11 + coloffcet)->checkState() == Qt::Checked ? "1" : "0",
                        ui->tbl->item(i, 12 + coloffcet)->checkState() == Qt::Checked ? "1" : "0",
                        ui->tbl->item(i, 13 + coloffcet)->checkState() == Qt::Checked ? "1" : "0",
                        QString::number(ui->tbl->item(i, 14 + coloffcet)->data(Qt::EditRole).toString().toInt())
                       );
        }
    }
    if (!sql.isEmpty()) {
        sql = QString("insert into o_route (f_round, f_driver, f_partner, f_1, f_2, f_3, f_4, f_5, f_6, f_7, f_q) values %1")
              .arg(sql);
    }
    QString where = " where f_driver=" + ui->cbDriver->currentData().toString();
    if (ui->cbPartnerStatus->currentData().toInt() > 0) {
        where += " and f_partner in (select f_id from c_partners where f_state=" + ui->cbPartnerStatus->currentData().toString()
                 + ") ";
    }
    db.exec("delete from o_route " + where);
    if (!sql.isEmpty()) {
        db.exec(sql);
    }
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

void C5Route::on_btnGo_clicked()
{
    loadPartners();
}

void C5Route::on_tbl_itemSelectionChanged()
{
    int column = -1;
    QModelIndexList mil = ui->tbl->selectionModel()->selectedIndexes();
    if (mil.count() > 0) {
        column = mil.at(0).column();
        if (column == ui->tbl->currentColumn()) {
            if (column > 6) {
                column -= 7;
                column = column % 7;
            }
            ui->tbl->setEditTriggers(column == 0 ? QTableWidget::EditTrigger::AllEditTriggers :
                                     QTableWidget::EditTrigger::NoEditTriggers);
        }
    }
}

void C5Route::on_cI_clicked()
{
    filterRows();
}

void C5Route::on_cII_clicked()
{
    filterRows();
}

void C5Route::on_cIII_clicked()
{
    filterRows();
}

void C5Route::on_cIV_clicked()
{
    filterRows();
}

void C5Route::on_cV_clicked()
{
    filterRows();
}

void C5Route::on_cVI_clicked()
{
    filterRows();
}

void C5Route::on_cVII_clicked()
{
    filterRows();
}

void C5Route::filterRows()
{
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        bool h = true;
        bool s = false;
        for (int c = 7; c < ui->tbl->columnCount(); c += 8) {
            if (ui->cI->isChecked()) {
                s = true;
                if (ui->tbl->item(i, c)->checkState() == Qt::Checked) {
                    h = false;
                }
            }
            if (ui->cII->isChecked()) {
                s = true;
                if (ui->tbl->item(i, c + 1)->checkState() == Qt::Checked) {
                    h = false;
                }
            }
            if (ui->cIII->isChecked()) {
                s = true;
                if (ui->tbl->item(i, c + 2)->checkState() == Qt::Checked) {
                    h = false;
                }
            }
            if (ui->cIV->isChecked()) {
                s = true;
                if (ui->tbl->item(i, c + 3)->checkState() == Qt::Checked) {
                    h = false;
                }
            }
            if (ui->cV->isChecked()) {
                s = true;
                if (ui->tbl->item(i, c + 4)->checkState() == Qt::Checked) {
                    h = false;
                }
            }
            if (ui->cVI->isChecked()) {
                s = true;
                if (ui->tbl->item(i, c + 5)->checkState() == Qt::Checked) {
                    h = false;
                }
            }
            if (ui->cVII->isChecked()) {
                s = true;
                if (ui->tbl->item(i, c + 6)->checkState() == Qt::Checked) {
                    h = false;
                }
            }
        }
        if (s) {
            ui->tbl->setRowHidden(i, h);
        } else {
            ui->tbl->setRowHidden(i, false);
        }
    }
}
