#include "dlgreservation.h"
#include "ui_dlgreservation.h"
#include "dlgpreorderw.h"
#include "c5orderdriver.h"
#include "c5logtoserverthread.h"
#include "dlglistofreservation.h"
#include "c5user.h"
#include <QPainter>
#include <QItemDelegate>
#include <QScrollBar>

#define chart_days 360

class TblItemDelegate : public QItemDelegate
{

public:
    TblItemDelegate(QTableWidget *w)
    {
        fTbl = w;
    }

protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
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

void DlgReservation::openReservationResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
    DlgPreorder d(jdoc);
    d.setHotelMode(true);
    d.exec();
    loadTable();
}

void DlgReservation::on_btnCreateReservation_clicked()
{
    int row = ui->tblRoom->currentRow();
    if (row < 0) {
        C5Message::error(tr("Select room"));
        return;
    }
    if (DlgPreorder(QJsonObject{
    {"f_table", fRoomId.at(row)},
        {"f_hall", ui->tblRoom->item(row, 0)->data(Qt::UserRole).toInt()}
    })
    .setHotelMode(true).exec() == QDialog::Accepted) {
        loadTable();
    } else {
        loadTable();
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
    int colCount = chart_days;
    ui->tbl->setColumnCount(colCount);
    ui->tblDate->setColumnCount(colCount);
    db.exec("select h.f_name, h.f_id "
            "from h_halls h "
            "inner join s_settings_values v ON v.f_settings=h.f_settings "
            "WHERE v.f_key=112 AND v.f_value=\"1\" "
            "order by f_id");
    fHallFilter.clear();
    while (ui->halls->itemAt(0)) {
        ui->halls->itemAt(0)->widget()->deleteLater();
        ui->halls->removeItem(ui->halls->itemAt(0));
    }
    while (db.nextRow()) {
        QPushButton *btn = new QPushButton(this);
        btn->setText(db.getString("f_name"));
        btn->setCheckable(true);
        btn->setChecked(true);
        btn->setProperty("id", db.getInt("f_id"));
        fHallFilter[db.getInt("f_id")] = true;
        connect(btn, &QPushButton::clicked, [btn, this](bool checked) {
            fHallFilter[btn->property("id").toInt()] = checked;
            filterHall();
        });
        ui->halls->addWidget(btn);
    }
    QStringList roomNames;
    db.exec("SELECT  t.f_id, t.f_name, v.f_value, t.f_hall "
            "from h_tables t "
            "inner join h_halls h on h.f_id=t.f_hall "
            "inner join s_settings_values v ON v.f_settings=h.f_settings "
            "WHERE v.f_key=112 AND v.f_value=\"1\" "
            "order by f_id ");
    ui->tbl->setRowCount(db.rowCount());
    ui->tblRoom->setRowCount(db.rowCount());
    int i = 0;
    while (db.nextRow()) {
        roomNames.append(db.getString("f_name"));
        fRoomId.append(db.getInt("f_id"));
        QTableWidgetItem *item = new QTableWidgetItem(roomNames.at(i));
        item->setData(Qt::UserRole, db.getInt("f_hall"));
        ui->tblRoom->setItem(i, 0, item);
        i++;
    }
    QDate d1 = QDate::fromString(__c5config.getValue(param_date_cash), FORMAT_DATE_TO_STR_MYSQL);
    QDate d2 = d1;
    int colSpan = 0;
    for (int i = 0; i < chart_days; i++) {
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
    ui->tblDate->setSpan(0, colSpan, 1, chart_days);
    ui->tblDate->setItem(0, colSpan, new QTableWidgetItem(d2.toString("MMMM")));
    ui->tblDate->item(0, colSpan)->setTextAlignment(Qt::AlignCenter);
    d1 = QDate::fromString(__c5config.getValue(param_date_cash), FORMAT_DATE_TO_STR_MYSQL);
    db.exec("select o.f_id, o.f_state, o.f_table, ohd.f_date, ohd.f_1, ohd.f_2 "
            "from o_header_hotel_date ohd "
            "left join o_header o on o.f_id=ohd.f_header "
            "where o.f_state in (1, 5, 6) "
            "order by o.f_table, o.f_id, ohd.f_date, ohd.f_1, ohd.f_2 ");
    while (db.nextRow()) {
        int table = db.getInt("f_table");
        int r = fRoomId.indexOf(table);
        if (r < 0) {
            continue;
        }
        int c = d1.daysTo(db.getDate("f_date"));
        if (c < 0) {
            continue;
        }
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

void DlgReservation::filterHall()
{
    for (int i = 0; i < ui->tblRoom->rowCount(); i++) {
        auto *item = ui->tblRoom->item(i, 0);
        int hallid = item->data(Qt::UserRole).toInt();
        bool rowHidden = fHallFilter[hallid] == 0;
        ui->tblRoom->setRowHidden(i, rowHidden);
        ui->tbl->setRowHidden(i, rowHidden);
    }
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
    fHttp->createHttpQuery("/engine/waiter/reservation-open.php", QJsonObject{{"id", id}}, SLOT(openReservationResponse(
                QJsonObject)));
}

void DlgReservation::on_btnClearFilter_clicked()
{
    for (int i = 0; i < ui->halls->count(); i++) {
        auto *btn = dynamic_cast<QPushButton *>(ui->halls->itemAt(i)->widget());
        if (btn) {
            btn->setChecked(true);
        }
    }
    for (QMap<int, bool>::iterator it = fHallFilter.begin(); it != fHallFilter.end(); it++) {
        it.value() = true;
    }
    filterHall();
}

void DlgReservation::on_btnList_clicked()
{
    DlgListOfReservation(this).exec();
    loadTable();
}
