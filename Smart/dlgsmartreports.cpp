#include "dlgsmartreports.h"
#include "ui_dlgsmartreports.h"
#include "c5printing.h"
#include "sessionorders.h"
#include <QFile>

DlgSmartReports::DlgSmartReports() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgSmartReports)
{
    ui->setupUi(this);
    QListWidgetItem *item = new QListWidgetItem(ui->list);
    item->setText(tr("Sales by dishes"));
    item->setData(Qt::UserRole, 1);
    item->setTextAlignment(Qt::AlignCenter);
    ui->list->addItem(item);

    item = new QListWidgetItem(ui->list);
    item->setText(tr("Sales by dishes with time"));
    item->setData(Qt::UserRole, 2);
    item->setTextAlignment(Qt::AlignCenter);
    ui->list->addItem(item);

    item = new QListWidgetItem(ui->list);
    item->setText(tr("Session orders"));
    item->setData(Qt::UserRole, 3);
    item->setTextAlignment(Qt::AlignCenter);
    ui->list->addItem(item);

    item = new QListWidgetItem(ui->list);
    item->setText(tr("Delivery"));
    item->setData(Qt::UserRole, 4);
    item->setTextAlignment(Qt::AlignCenter);
    ui->list->addItem(item);

    item = new QListWidgetItem(ui->list);
    item->setText(tr("Cancel"));
    item->setTextAlignment(Qt::AlignCenter);
    ui->list->addItem(item);
}

DlgSmartReports::~DlgSmartReports()
{
    delete ui;
}
void DlgSmartReports::on_list_itemClicked(QListWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }
    if (item->data(Qt::UserRole).toInt() == 0) {
        reject();
        return;
    }
    switch (item->data(Qt::UserRole).toInt()) {
    case 1:
        reportCommonDishes();
        break;
    case 2:
        reportCommonDishesWithTime();
        break;
    case 3:
        SessionOrders().exec();
        break;
    case 4:
        printDeliveryReport();
        break;
    }
}

void DlgSmartReports::reportCommonDishes()
{
    accept();
    QFont font(qApp->font());
    font.setPointSize(28);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);

    if (QFile::exists("./logo_receipt.png")) {
        p.image("./logo_receipt.png", Qt::AlignHCenter);
        p.br();
    }
    p.setFontBold(true);
    p.ctext(tr("End of day"));
    p.br();
    p.ctext(ui->leDateStart->text());
    p.br();
    p.ctext("-");
    p.br();
    p.ctext(ui->leDateEnd->text());
    p.br();
    double total = 0;

    C5Database dd(fDBParams);
    dd[":f_datecash1"] = ui->leDateStart->date();
    dd[":f_datecash2"] = ui->leDateEnd->date();
    dd[":f_stateh"] = ORDER_STATE_CLOSE;
    dd.exec("select count(f_id) from o_header h "
            "where h.f_state=:f_stateh  and h.f_datecash between :f_datecash1 and :f_datecash2 ");
    dd.nextRow();
    int totalQty = dd.getInt(0);
    dd[":f_datecash1"] = ui->leDateStart->date();
    dd[":f_datecash2"] = ui->leDateEnd->date();
    dd[":f_stateh"] = ORDER_STATE_CLOSE;
    dd[":f_stated"] = DISH_STATE_OK;
    dd.exec("select d.f_name, sum(b.f_qty1) as f_qty, b.f_price, sum(b.f_total) as f_total "
            "from o_body b "
            "inner join o_header h on h.f_id=b.f_header "
            "left join d_dish d on d.f_id=b.f_dish "
            "where h.f_state=:f_stateh and b.f_state=:f_stated and h.f_datecash between :f_datecash1 and :f_datecash2 "
            "group by 1, 3 ");
    p.setFontBold(false);
    p.setFontSize(22);
    while (dd.nextRow()) {
        if (p.checkBr(p.fLineHeight + 2)) {
            p.br();
        }
        total += dd.getDouble("f_total");
        p.ltext(dd.getString("f_name"), 0);
        p.br();
        p.ltext(QString("%1 X %2 = %3").arg(float_str(dd.getDouble("f_qty"), 2)).arg(dd.getDouble("f_price"), 2).arg(float_str(dd.getDouble("f_total"), 2)), 0);
        p.br();
        p.line();
        p.br(2);
    }
    p.line(4);
    p.br(3);
    p.setFontBold(true);
    p.setFontSize(28);
    p.br();
    p.br();
    p.ltext(tr("Quantity of orders"), 0);
    p.rtext(QString::number(totalQty));
    p.br();
    p.br();
    p.ltext(tr("Total today"), 0);
    p.rtext(float_str(total, 2));
    p.br();
    p.br();

    p.line();
    p.br();

    p.setFontSize(18);
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);
}

void DlgSmartReports::reportCommonDishesWithTime()
{
    accept();
    QFont font(qApp->font());
    font.setPointSize(28);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);

    if (QFile::exists("./logo_receipt.png")) {
        p.image("./logo_receipt.png", Qt::AlignHCenter);
        p.br();
    }
    p.setFontBold(true);
    p.ctext(tr("End of day"));
    p.br();
    p.ctext(ui->leDateStart->text() + " " + ui->leTimeStart->text());
    p.br();
    p.ctext("-");
    p.br();
    p.ctext(ui->leDateEnd->text() + " " + ui->leTimeEnd->text());
    p.br();
    double total = 0;

    C5Database dd(fDBParams);
    dd[":f_date1"] = QDateTime::fromString(ui->leDateStart->text() + " " + ui->leTimeStart->text(), "dd/MM/yyyy HH:mm");
    dd[":f_date2"] = QDateTime::fromString(ui->leDateEnd->text() + " " + ui->leTimeEnd->text(), "dd/MM/yyyy HH:mm");
    dd[":f_stateh"] = ORDER_STATE_CLOSE;
    dd.exec("select count(f_id) from o_header h "
            "where h.f_state=:f_stateh  and cast(concat(h.f_dateclose, ' ', h.f_timeclose) as datetime) between :f_date1 and :f_date2 ");
    dd.nextRow();
    int totalQty = dd.getInt(0);
    dd[":f_date1"] = QDateTime::fromString(ui->leDateStart->text() + " " + ui->leTimeStart->text(), "dd/MM/yyyy HH:mm");
    dd[":f_date2"] = QDateTime::fromString(ui->leDateEnd->text() + " " + ui->leTimeEnd->text(), "dd/MM/yyyy HH:mm");
    dd[":f_stateh"] = ORDER_STATE_CLOSE;
    dd[":f_stated"] = DISH_STATE_OK;
    dd.exec("select d.f_name, sum(b.f_qty1) as f_qty, b.f_price, sum(b.f_total) as f_total "
            "from o_body b "
            "inner join o_header h on h.f_id=b.f_header "
            "left join d_dish d on d.f_id=b.f_dish "
            "where h.f_state=:f_stateh and b.f_state=:f_stated "
            "and cast(concat(h.f_dateclose, ' ', h.f_timeclose) as datetime) between :f_date1 and :f_date2 "
            "group by 1, 3 ");
    p.setFontBold(false);
    p.setFontSize(22);
    while (dd.nextRow()) {
        if (p.checkBr(p.fLineHeight + 2)) {
            p.br();
        }
        total += dd.getDouble("f_total");
        p.ltext(dd.getString("f_name"), 0);
        p.br();
        p.ltext(QString("%1 X %2 = %3").arg(float_str(dd.getDouble("f_qty"), 2)).arg(dd.getDouble("f_price"), 2).arg(float_str(dd.getDouble("f_total"), 2)), 0);
        p.br();
        p.line();
        p.br(2);
    }
    p.line(4);
    p.br(3);
    p.setFontBold(true);
    p.setFontSize(28);
    p.br();
    p.br();
    p.ltext(tr("Quantity of orders"), 0);
    p.rtext(QString::number(totalQty));
    p.br();
    p.br();
    p.ltext(tr("Total today"), 0);
    p.rtext(float_str(total, 2));
    p.br();
    p.br();

    p.line();
    p.br();

    p.setFontSize(18);
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);
}

void DlgSmartReports::printDeliveryReport()
{
    accept();
    QFont font(qApp->font());
    font.setPointSize(28);
    C5Printing p;
    p.setSceneParams(650, 2800, QPrinter::Portrait);
    p.setFont(font);

    if (QFile::exists("./logo_receipt.png")) {
        p.image("./logo_receipt.png", Qt::AlignHCenter);
        p.br();
    }
    p.setFontBold(true);
    p.ctext(tr("Delivery report"));
    p.br();
    p.ctext(ui->leDateStart->text() + " " + ui->leTimeStart->text());
    p.br();
    p.ctext("-");
    p.br();
    p.ctext(ui->leDateEnd->text() + " " + ui->leTimeEnd->text());
    p.br();

    QString sql ("SELECT h.f_id, concat(h.f_prefix, h.f_hallid) as f_hallid, "
                    "date_format(cast(concat(f_dateclose, ' ', f_timeclose) as datetime),'%d/%m/%Y %H:%i' ) as f_date, h.f_amounttotal "
                    "from o_header h "
                    "left join o_header_flags f on f.f_id=h.f_id "
                    "where f.f_1=1 and h.f_state=:f_state and cast(concat(h.f_dateclose, ' ', h.f_timeclose) as datetime) between :f_date1 and :f_date2 ");
    C5Database db(fDBParams);
    db[":f_date1"] = QDateTime::fromString(ui->leDateStart->text() + " " + ui->leTimeStart->text(), "dd/MM/yyyy HH:mm");
    db[":f_date2"] = QDateTime::fromString(ui->leDateEnd->text() + " " + ui->leTimeEnd->text(), "dd/MM/yyyy HH:mm");
    db[":f_state"] = ORDER_STATE_CLOSE;
    db.exec(sql);
    double total = 0;
    int count = db.rowCount();
    while (db.nextRow()) {
        total += db.getDouble("f_amounttotal");
    }
    p.setFontBold(false);
    p.setFontSize(22);
    p.ltext(tr("Delivery total count"), 0);
    p.rtext(QString::number(count));
    p.br();
    p.ltext(tr("Delivery total amount"), 0);
    p.rtext(float_str(total, 2));
    p.br();
    p.br();

    p.line();
    p.br();

    p.setFontSize(18);
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);
}
