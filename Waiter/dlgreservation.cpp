#include "dlgreservation.h"
#include "ui_dlgreservation.h"
#include "dlgpreorderw.h"
#include "c5orderdriver.h"
#include "c5logtoserverthread.h"
#include "c5user.h"
#include <QPainter>
#include <QItemDelegate>
#include <QScrollBar>

class TblItemDelegate : public QItemDelegate {

public:
    TblItemDelegate(QTableWidget *w) {
        fTbl = w;
    }

protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->drawRect(option.rect);
        if (index.isValid() == false) {
            return;
        }
        bool f1 = index.data(Qt::UserRole + 1).toInt();
        bool f2 = index.data(Qt::UserRole + 2).toInt();
        QRect r1 = option.rect;
        QRect r2 = option.rect;
        r1.adjust(0, 0, - r1.width() / 2, 0);
        r2.adjust(r2.width() / 2, 0, 0, 0);
        QColor c1 = f1 ? Qt::red : Qt::white;
        QColor c2 = f2 ? Qt::red : Qt::white;
        if (index.data(Qt::UserRole + 5).toInt() == ORDER_STATE_OPEN) {
            c1 = Qt::green;
        }
        if (index.data(Qt::UserRole + 6).toInt() == ORDER_STATE_OPEN) {
            c2 = Qt::green;
        }
        painter->setBrush(c1);
        painter->drawRect(r1);
        painter->setBrush(c2);
        painter->drawRect(r2);
        painter->restore();
    }

private:
    QTableWidget *fTbl;
};

DlgReservation::DlgReservation(C5User *u) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgReservation)
{
    ui->setupUi(this);
    fUser = u;
    ui->tbl->setItemDelegate(new TblItemDelegate(ui->tbl));
    ui->tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tbl->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    if (!C5Config::isAppFullScreen()) {
        setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);
    } else {
        setWindowState(Qt::WindowFullScreen);
    }
    ui->tblDate->setMaximumHeight(81);
    ui->tblRoom->setMaximumWidth(100);
    ui->tblRoom->setMinimumWidth(100);
    connect(ui->tbl->horizontalScrollBar(), &QScrollBar::valueChanged, this, &DlgReservation::horizontalScroll);
    connect(ui->tbl->verticalScrollBar(), &QScrollBar::valueChanged, this, &DlgReservation::verticalScroll);
    connect(ui->tblRoom->verticalScrollBar(), &QScrollBar::valueChanged, this, &DlgReservation::verticalRoomScroll);
    loadTable();
}

DlgReservation::~DlgReservation()
{
    delete ui;
}
void DlgReservation::on_btnCreateReservation_clicked()
{
    int row = ui->tblRoom->currentRow();
    if (row < 0) {
        C5Message::error(tr("Select room"));
        return;
    }
    C5OrderDriver od(__c5config.dbParams());
    QString uuid;
    od.newOrder(fUser->id(), uuid, fRoomId.at(row));
    od.setHeader("f_state", ORDER_STATE_PREORDER_EMPTY);
    od.setPreorder("f_fortable", fRoomId.at(row));
    if (DlgPreorder(&od, fUser, __c5config.dbParams()).setHotelMode(true).exec() == QDialog::Accepted) {
        loadTable();
    } else {

    }
}

void DlgReservation::on_btnExit_clicked()
{
    accept();
}

void DlgReservation::horizontalScroll(int v)
{
    ui->tblDate->horizontalScrollBar()->setValue(v);
}

void DlgReservation::verticalScroll(int v)
{
    ui->tblRoom->verticalScrollBar()->setValue(v);
}

void DlgReservation::verticalRoomScroll(int v)
{
    ui->tbl->verticalScrollBar()->setValue(v);
}

void DlgReservation::loadTable()
{
    C5Database db(__c5config.dbParams());
    int vpos = ui->tbl->verticalScrollBar()->value();
    int hpos = ui->tbl->horizontalScrollBar()->value();
    ui->tblDate->clearContents();
    ui->tbl->clearContents();
    int colCount = 60;
    ui->tbl->setColumnCount(colCount);
    ui->tblDate->setColumnCount(colCount);

    QStringList roomNames;
    db.exec("SELECT  t.f_id, t.f_name, v.f_value from h_tables t "
            "inner join h_halls h on h.f_id=t.f_hall "
            "inner join s_settings_values v ON v.f_settings=h.f_settings "
            "WHERE v.f_key=112 AND v.f_value=\"1\" "
            "order by f_id ");
    while (db.nextRow()) {
        roomNames.append(db.getString("f_name"));
        fRoomId.append(db.getInt("f_id"));

    }
    ui->tbl->setRowCount(db.rowCount());
    ui->tblRoom->setRowCount(db.rowCount());
    for (int i = 0; i < roomNames.count(); i++) {
        ui->tblRoom->setItem(i, 0, new QTableWidgetItem(roomNames.at(i)));
    }
    QDate d1 = QDate::fromString(__c5config.getValue(param_date_cash), FORMAT_DATE_TO_STR_MYSQL);
    QDate d2 = d1;
    int colSpan = 0;
    for (int i = 0; i < 60; i++) {
        if (d2.month() != d1.month()) {
            ui->tblDate->setSpan(0, colSpan, 1, i - colSpan);
            ui->tblDate->setItem(0, colSpan, new QTableWidgetItem(d2.toString("MMMM, yyyy")));
            ui->tblDate->item(0, colSpan)->setTextAlignment(Qt::AlignCenter);
            colSpan = i;
            d2 = d1;
        }
        ui->tblDate->setItem(1, i, new QTableWidgetItem(QString::number(d1.day())));
        d1 = d1.addDays(1);
    }

    ui->tblDate->setSpan(0, colSpan, 1, 60);
    ui->tblDate->setItem(0, colSpan, new QTableWidgetItem(d2.toString("MMMM")));
    ui->tblDate->item(0, colSpan)->setTextAlignment(Qt::AlignCenter);

    d1 = QDate::fromString(__c5config.getValue(param_date_cash), FORMAT_DATE_TO_STR_MYSQL);
    db.exec("select o.f_id, o.f_state, o.f_table, ohd.f_date, ohd.f_1, ohd.f_2 "
            "from o_header_hotel_date ohd "
            "left join o_header o on o.f_id=ohd.f_header "
            "where o.f_state in (1, 5, 6) "
            "order by o.f_table, o.f_id, ohd.f_date, ohd.f_1, ohd.f_2 ");
    while (db.nextRow()) {
        int r = fRoomId.indexOf(db.getInt("f_table"));
        if (r < 0) {
            return;
        }
        int c = d1.daysTo(db.getDate("f_date"));
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setData(Qt::UserRole + 1, db.getInt("f_1"));
        item->setData(Qt::UserRole + 2, db.getInt("f_2"));
        item->setData(Qt::UserRole + 3, db.getInt("f_1") == 0 ? "" : db.getString("f_id"));
        item->setData(Qt::UserRole + 4, db.getInt("f_2") == 0 ? "" : db.getString("f_id"));
        item->setData(Qt::UserRole + 5, db.getInt("f_1") == 0 ? 0 : db.getInt("f_state"));
        item->setData(Qt::UserRole + 6, db.getInt("f_2") == 0 ? 0 : db.getInt("f_state"));
        ui->tbl->setItem(r, c, item);
    }
    ui->tbl->horizontalScrollBar()->setValue(hpos);
    ui->tbl->verticalScrollBar()->setValue(vpos);
}

void DlgReservation::on_tbl_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    QString id1 = item->data(Qt::UserRole + 3).toString();
    QString id2 = item->data(Qt::UserRole + 4).toString();

    if (id1.isEmpty() && id2.isEmpty()) {
        return;
    }
    QString id;
    if (!id1.isEmpty() && id2.isEmpty()) {
        id = id1;
    }
    if (id1.isEmpty() && !id2.isEmpty()) {
        id = id2;
    }
    if (id1 == id2) {
        id = id1;
    }
    if (id.isEmpty() && id1 != id2) {
        switch (C5Message::question(tr("Which one to open?"), tr("Checkout"), tr("Cancel"), tr("Checkin"))) {
        case C5Message::Rejected:
            return;
        case C5Message::Accepted:
            id = id2;
            break;
        case 2:
            id = id1;
            break;
        }
    }
    C5OrderDriver od(__c5config.dbParams());
    if (od.loadData(id)) {
        switch (DlgPreorder(&od, fUser, __c5config.dbParams()).setHotelMode(true).exec()) {
        case DlgPreorder::RESULT_OK:
            loadTable();
            break;
        case DlgPreorder::RESULT_CANCELED:
            C5Database db(__c5config.dbParams());
            db[":f_state"] = ORDER_STATE_VOID;
            db.update("o_header", "f_id", id);
            C5LogToServerThread::remember(LOG_WAITER, fUser->fullName(), id, id, id, tr("Booking canceled"), "", "");
            loadTable();
            break;
        }
    }
}
