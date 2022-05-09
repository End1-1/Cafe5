#include "storeinput.h"
#include "ui_storeinput.h"
#include "c5utils.h"
#include "c5database.h"
#include "c5config.h"
#include "c5storedraftwriter.h"
#include "c5checkbox.h"
#include "c5message.h"
#include "c5user.h"
#include "viewinputitem.h"
#include <QSet>

#define VM_ACCEPT 0
#define VM_STORE 1
#define VM_STORE_GROUP 2
#define VM_STORE_ITEMS 3

StoreInput::StoreInput(C5User *user, QWidget *parent) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::StoreInput)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 30, 100, 0, 300, 140, 100, 100);
    fViewMode = VM_ACCEPT;
    fUser = user;
    refresh();
}

StoreInput::~StoreInput()
{
    delete ui;
}

void StoreInput::checkboxCheck(bool v)
{
    C5CheckBox *c = static_cast<C5CheckBox*>(sender());
    if (v) {
        switch (fViewMode) {
        case VM_ACCEPT:
        case VM_STORE:
            ui->leRows->setInteger(ui->leRows->getInteger() + 1);
            ui->leQty->setDouble(ui->leQty->getDouble() + ui->tbl->getDouble(c->property("row").toInt(), 6));
            break;
        case VM_STORE_GROUP:
            ui->leRows->setInteger(ui->leRows->getInteger() + 1);
            ui->leQty->setDouble(ui->leQty->getDouble() + ui->tbl->getDouble(c->property("row").toInt(), 2));
            break;
        case VM_STORE_ITEMS:
            ui->leRows->setInteger(ui->leRows->getInteger() + 1);
            ui->leQty->setDouble(ui->leQty->getDouble() + ui->tbl->getDouble(c->property("row").toInt(), 4));
            break;
        }

    } else {
        switch (fViewMode) {
        case VM_ACCEPT:
        case VM_STORE:
            ui->leRows->setInteger(ui->leRows->getInteger() - 1);
            ui->leQty->setDouble(ui->leQty->getDouble() - ui->tbl->getDouble(c->property("row").toInt(), 6));
            break;
        case VM_STORE_GROUP:
            ui->leRows->setInteger(ui->leRows->getInteger() - 1);
            ui->leQty->setDouble(ui->leQty->getDouble() + ui->tbl->getDouble(c->property("row").toInt(), 2));
            break;
        case VM_STORE_ITEMS:
            ui->leRows->setInteger(ui->leRows->getInteger() - 1);
            ui->leQty->setDouble(ui->leQty->getDouble() - ui->tbl->getDouble(c->property("row").toInt(), 4));
            break;
        }
    }
}

void StoreInput::on_btnView_clicked()
{
    QModelIndexList l = ui->tbl->selectionModel()->selectedRows();
    if (l.count() == 0) {
        return;
    }
    ViewInputItem *vi = new ViewInputItem(ui->tbl->getString(l.at(0).row(), 0));
    vi->exec();
    vi->deleteLater();
}

void StoreInput::on_btnAccept_clicked()
{
    if (fViewMode != VM_ACCEPT) {
        C5Message::error(tr("Set checking view mode"));
        return;
    }
    QList<int> rows;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (ui->tbl->checkBox(i, 1)->isChecked()) {
            rows.append(i);
        }
    }
    if (rows.isEmpty()) {
        C5Message::error(tr("Nothing is selected"));
        return;
    }
    C5Database db(__c5config.replicaDbParams());
    db.startTransaction();
    QSet<QString> ids;
    foreach (int r, rows) {
        QString id = ui->tbl->getString(r, 0);
        ids.insert(QString("'%1'").arg(id));
        db[":f_id"] = id;
        db[":f_datetime"] = QDateTime::currentDateTime();
        if (!db.insert("a_header_shop2partneraccept", false)) {
            C5Message::error(db.fLastError);
            getList();
            return;
        }
    }
    QString idlist = ids.toList().join(",");
    db.exec(QString("select f_document from a_store_draft where f_id in (%1)").arg(idlist));
    ids.clear();
    while (db.nextRow()) {
        ids.insert(db.getString("f_document"));
    }
    for (const QString &s: ids) {
        db[":f_id"] = s;
        db.exec("select count(a1.f_id) as c1, count(a2.f_id) as c2 "
                "from a_header_shop2partner p "
                "left join a_store_draft a1 on a1.f_document=p.f_id "
                "left join a_header_shop2partneraccept a2 on a2.f_id=a1.f_id "
                "where p.f_id=:f_id");
        if (db.nextRow()){
            if (db.getInt("c1") == db.getInt("c2")) {
                C5StoreDraftWriter dw(db);
                QString err;
                dw.updateField("a_header", "f_date", QDate::currentDate(), "f_id", s);
                dw.updateField("a_header", "f_state", DOC_STATE_SAVED, "f_id", s);
                if (!dw.writeInput(s, err)) {
                    C5Message::error(err);
                    db.rollback();
                    return;
                }
            }
        }
    }
    db.commit();
    getList();
}

void StoreInput::getList()
{
    ui->leQty->clear();
    ui->leRows->clear();
    ui->tbl->clearContents();
    ui->tbl->setRowCount(0);
    QStringList header;
    header << tr("UUID") << tr("X") << tr("Date") << tr("Store") << tr("Goods") << tr("Scancode") << tr("Qty") << tr("Retail");
    ui->tbl->setColumnCount(header.count());
    ui->tbl->setHorizontalHeaderLabels(header);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 30, 100, 0, 300, 140, 100, 100);
    C5Database db(__c5config.replicaDbParams());
    db[":f_store"] = __c5config.defaultStore();
    if (!db.exec("select ad.f_id, spa.f_id as f_acc, ah.f_date, g.f_name, g.f_scancode, ad.f_qty, g.f_saleprice "
                "from a_store_draft ad "
                "left join a_header_shop2partneraccept spa on spa.f_id=ad.f_id "
                "inner join c_goods g on g.f_id=ad.f_goods "
                "inner join a_header ah on ah.f_id=ad.f_document "
                "inner join a_header_shop2partner sp on sp.f_id=ah.f_id "
                 "where sp.f_store=:f_store and sp.f_accept=0 "
                 "order by g.f_scancode ")) {
        C5Message::error(db.fLastError);
        return;
    }
    ui->leTotal->setDouble(0);
    while (db.nextRow()) {
        if (!db.getString("f_acc").isEmpty()) {
            continue;
        }
        int r = ui->tbl->addEmptyRow();
        ui->tbl->setString(r, 0, db.getString("f_id"));
        C5CheckBox *c = ui->tbl->createCheckbox(r, 1);
        connect(c, SIGNAL(clicked(bool)), this, SLOT(checkboxCheck(bool)));
        ui->tbl->setString(r, 2, db.getDate("f_date").toString(FORMAT_DATE_TO_STR));
        ui->tbl->setString(r, 3, "Store");
        ui->tbl->setString(r, 4, db.getString("f_name"));
        ui->tbl->setString(r, 5, db.getString("f_scancode"));
        ui->tbl->setString(r, 6, db.getString("f_qty"));
        ui->tbl->setString(r, 7, db.getString("f_saleprice"));
        ui->leTotal->setDouble(ui->leTotal->getDouble() + (db.getDouble("f_qty") * db.getDouble("f_saleprice")));
    }
    ui->tbl->fitColumnsToWidth();
}

void StoreInput::history()
{
    ui->leQty->clear();
    ui->leRows->clear();
    ui->tbl->clearContents();
    ui->tbl->setRowCount(0);
    QStringList header;
    header << tr("UUID") << tr("X") << tr("Date") << tr("Store") << tr("Goods") << tr("Scancode") << tr("Qty") << tr("Retail");
    ui->tbl->setColumnCount(header.count());
    ui->tbl->setHorizontalHeaderLabels(header);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 30, 100, 0, 300, 140, 100, 100);
    C5Database db(__c5config.replicaDbParams());
    db[":f_store"] = __c5config.defaultStore();
    db[":f_date1"] = ui->deStart->date();
    db[":f_date2"] = ui->deEnd->date();
    db[":f_type"] = 1;
    db[":f_state"] = DOC_STATE_SAVED;
    if (!db.exec("select ad.f_id, '', ah.f_date, g.f_name, g.f_scancode, ad.f_qty, g.f_saleprice, "
            "spa.f_id as f_spa "
            "from a_store_draft ad "
            "inner join c_goods g on g.f_id=ad.f_goods "
            "inner join a_header ah on ah.f_id=ad.f_document "
            "left join a_header_shop2partneraccept spa on spa.f_id=ad.f_id "
            "where ad.f_store=:f_store and ah.f_date between :f_date1 and :f_date2 and ad.f_type=1 "
                 "and ad.f_type=:f_type and ah.f_state=:f_state ")) {
        C5Message::error(db.fLastError);
        return;
    }
    ui->leTotal->setDouble(0);
    while (db.nextRow()) {
        int r = ui->tbl->addEmptyRow();
        ui->tbl->setString(r, 0, db.getString("f_id"));
        C5CheckBox *c = ui->tbl->createCheckbox(r, 1);
        connect(c, SIGNAL(clicked(bool)), this, SLOT(checkboxCheck(bool)));
        ui->tbl->setString(r, 2, db.getDate("f_date").toString(FORMAT_DATE_TO_STR));
        ui->tbl->setString(r, 3, "Store");
        ui->tbl->setString(r, 4, db.getString("f_name"));
        ui->tbl->setString(r, 5, db.getString("f_scancode"));
        ui->tbl->setString(r, 6, db.getString("f_qty"));
        ui->tbl->setString(r, 7, db.getString("f_saleprice"));
        ui->leTotal->setDouble(ui->leTotal->getDouble() + (db.getDouble("f_qty") * db.getDouble("f_saleprice")));
        if (db.getString("f_id") == db.getString("f_spa")) {
            for (int i = 0; i < ui->tbl->columnCount(); i++) {
                ui->tbl->item(r, i)->setData(Qt::BackgroundColorRole, QColor(Qt::magenta));
            }
        }
    }
    ui->tbl->fitColumnsToWidth();
    ui->chAll->clicked(true);
}

void StoreInput::storeByGroup()
{
    ui->leQty->clear();
    ui->leRows->clear();
    ui->leTotal->setDouble(0);
    ui->tbl->clearContents();
    ui->tbl->setRowCount(0);
    QStringList header;
    header << tr("X") << tr("Group") << tr("Qty") << tr("Retail");
    ui->tbl->setColumnCount(header.count());
    ui->tbl->setHorizontalHeaderLabels(header);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 30, 400, 100, 100);
    C5Database db(__c5config.replicaDbParams());
    db[":f_store"] = __c5config.defaultStore();
    db[":f_date"] = QDate::currentDate();
    if (!db.exec("select gg.f_name as f_group,sum(s.f_qty*s.f_type) as f_qty, "
            "sum(s.f_qty*s.f_type)*g.f_saleprice as f_totalsale "
            "from a_store s "
            "inner join c_goods g on g.f_id=s.f_goods "
            "inner join c_groups gg on gg.f_id=g.f_group "
            "inner join a_header h on h.f_id=s.f_document  "
            "where h.f_date<=:f_date and h.f_state=1  and s.f_store=:f_store  "
            "group by gg.f_name "
            "having sum(s.f_qty*s.f_type) > 0 "
                 "order by gg.f_name ")) {
        C5Message::error(db.fLastError);
        return;
    }
    while (db.nextRow()) {
        int r = ui->tbl->addEmptyRow();
        C5CheckBox *c = ui->tbl->createCheckbox(r, 0);
        connect(c, SIGNAL(clicked(bool)), this, SLOT(checkboxCheck(bool)));
        ui->tbl->setString(r, 1, db.getString("f_group"));
        ui->tbl->setDouble(r, 2, db.getDouble("f_qty"));
        ui->tbl->setDouble(r, 3, db.getDouble("f_totalsale"));
        ui->leTotal->setDouble(ui->leTotal->getDouble() + db.getDouble("f_totalsale"));
    }
    ui->chAll->clicked(true);
}

void StoreInput::storeByItems()
{
    ui->leQty->clear();
    ui->leRows->clear();
    ui->leTotal->setDouble(0);
    ui->tbl->clearContents();
    ui->tbl->setRowCount(0);
    QStringList header;
    header << tr("X") << tr("Group") << tr("Goods") << tr("Scancode") << tr("Qty") << tr("Retail");
    ui->tbl->setColumnCount(header.count());
    ui->tbl->setHorizontalHeaderLabels(header);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 30, 300, 200, 200, 100, 100);
    C5Database db(__c5config.replicaDbParams());
    db[":f_store"] = __c5config.defaultStore();
    db[":f_date"] = QDate::currentDate();
    if (!db.exec("select gg.f_name as f_group, g.f_name, g.f_scancode, sum(s.f_qty*s.f_type) as f_qty, "
            "sum(s.f_qty*s.f_type)*g.f_saleprice as f_totalsale "
            "from a_store s "
            "inner join c_goods g on g.f_id=s.f_goods "
            "inner join c_groups gg on gg.f_id=g.f_group "
            "inner join a_header h on h.f_id=s.f_document  "
            "where h.f_date<=:f_date and h.f_state=1 and s.f_store=:f_store  "
            "group by gg.f_name "
            "having sum(s.f_qty*s.f_type) > 0 "
                 "order by gg.f_name ")) {
        C5Message::error(db.fLastError);
        return;
    }
    while (db.nextRow()) {
        int r = ui->tbl->addEmptyRow();
        C5CheckBox *c = ui->tbl->createCheckbox(r, 0);
        connect(c, SIGNAL(clicked(bool)), this, SLOT(checkboxCheck(bool)));
        ui->tbl->setString(r, 1, db.getString("f_group"));
        ui->tbl->setString(r, 2, db.getString("f_name"));
        ui->tbl->setString(r, 3, db.getString("f_scancode"));
        ui->tbl->setDouble(r, 4, db.getDouble("f_qty"));
        ui->tbl->setDouble(r, 5, db.getDouble("f_totalsale"));
        ui->leTotal->setDouble(ui->leTotal->getDouble() + db.getDouble("f_totalsale"));
    }
    ui->chAll->clicked(true);
    ui->tbl->fitColumnsToWidth();
}

void StoreInput::changeDate(int d)
{
    ui->deStart->setDate(ui->deStart->date().addDays(d));
    ui->deEnd->setDate(ui->deEnd->date().addDays(d));
    refresh();
}

void StoreInput::refresh()
{
    switch (fViewMode) {
    case VM_ACCEPT:
        getList();
        break;
    case VM_STORE:
        history();
        break;
    case VM_STORE_GROUP:
        storeByGroup();
        break;
    case VM_STORE_ITEMS:
        storeByItems();
        break;
    }
}

void StoreInput::disableMenuButtons(QPushButton *b)
{
    ui->btnHistoryMode->setChecked(false);
    ui->btnStore->setChecked(false);
    ui->btnAcceptMode->setChecked(false);
    ui->btnStoreGoods->setChecked(false);
    b->setChecked(true);
}

void StoreInput::on_btnDateLeft_clicked()
{
    changeDate(-1);
}

void StoreInput::on_btnDateRight_clicked()
{
    changeDate(1);
}

void StoreInput::on_btnRefresh_clicked()
{
    refresh();
}

void StoreInput::on_btnAcceptMode_clicked()
{
    disableMenuButtons(static_cast<QPushButton*>(sender()));
    fViewMode = VM_ACCEPT;
    refresh();
}

void StoreInput::on_btnHistoryMode_clicked()
{
    disableMenuButtons(static_cast<QPushButton*>(sender()));
    fViewMode = VM_STORE;
    refresh();
}

void StoreInput::on_leFilter_textChanged(const QString &arg1)
{
    for (int r = 0; r < ui->tbl->rowCount(); r++) {
        bool h = true;
        for (int c = 0; c < ui->tbl->columnCount(); c++) {
            if (ui->tbl->getString(r, c).contains(arg1, Qt::CaseInsensitive)) {
                h = false;
                break;
            }
        }
        ui->tbl->setRowHidden(r, h);
    }
}

void StoreInput::on_chAll_clicked(bool checked)
{
    int col = 0;
    int colsum = 0;
    switch (fViewMode) {
    case VM_ACCEPT:
    case VM_STORE:
        col = 1;
        colsum = 6;
        break;
    case VM_STORE_GROUP:
        col = 0;
        colsum = 1;
        break;
    }
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        ui->tbl->checkBox(i, col)->setChecked(checked);
    }
    if (checked) {
        ui->leQty->setDouble(ui->tbl->sumOfColumn(colsum));
        ui->leRows->setInteger(ui->tbl->visibleRows());
    } else {
        ui->leQty->setDouble(0);
        ui->leRows->setInteger(0);
    }
}

void StoreInput::on_btnStore_clicked()
{
    disableMenuButtons(static_cast<QPushButton*>(sender()));
    fViewMode = VM_STORE_GROUP;
    refresh();
}

void StoreInput::on_btnStoreGoods_clicked()
{
    disableMenuButtons(static_cast<QPushButton*>(sender()));
    fViewMode = VM_STORE_ITEMS;
    refresh();
}

void StoreInput::on_btnExit_clicked()
{
    accept();
}

void StoreInput::on_btnMinimize_clicked()
{
    showMinimized();
}
