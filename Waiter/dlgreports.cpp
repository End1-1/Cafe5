#include "dlgreports.h"
#include "ui_dlgreports.h"
#include "dlgreportsfilter.h"
#include "c5user.h"
#include "dlgreceiptlanguage.h"
#include "dlgorder.h"
#include "c5message.h"
#include "c5printing.h"
#include "c5utils.h"
#include "format_date.h"
#include "ninterface.h"
#include <QSettings>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QFile>

class GridDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *p, const QStyleOptionViewItem &opt,
               const QModelIndex &idx) const override
    {
        QStyledItemDelegate::paint(p, opt, idx);
        p->save();
        p->setPen(QPen(QColor("#6b7a84"), 1));
        const QRect r = opt.rect;
        // правая граница
        p->drawLine(r.topRight(), r.bottomRight());
        // нижняя граница
        p->drawLine(r.bottomLeft(), r.bottomRight());
        p->restore();
    }
};

DlgReports::DlgReports(C5User *user) :
    C5WaiterDialog(user),
    ui(new Ui::DlgReports)
{
    ui->setupUi(this);
    ui->tbl->setItemDelegate(new GridDelegate(ui->tbl));
    ui->tblTotal->setItemDelegate(new GridDelegate(ui->tblTotal));
    fCurrentHall = 0;
    setLangIcon();
    mDate1 = QDate::currentDate();
    mDate2 = QDate::currentDate();
}

DlgReports::~DlgReports()
{
    delete ui;
}

void DlgReports::openReports(C5User *user)
{
    DlgReports *d = new DlgReports(user);
    d->exec();
    delete d;
}

void DlgReports::showEvent(QShowEvent *e)
{
    C5WaiterDialog::showEvent(e);
    getDailyCommon();
}

void DlgReports::getDailyCommon()
{
    NInterface::query("/engine/v2/waiter/reports/get-orders", mUser->mSessionKey, this, {
        {"date1", mDate1.toString(FORMAT_DATE_TO_STR_MYSQL)},
        {"date2", mDate2.toString(FORMAT_DATE_TO_STR_MYSQL)},
        {"hall", fCurrentHall}
    }, [ = ](const QJsonObject & jdoc) {
        QJsonArray jorders = jdoc["orders"].toArray();
        ui->tbl->clearContents();
        ui->tbl->setRowCount(0);

        for(int i = 0; i < jorders.count(); i++) {
            QJsonObject o = jorders.at(i).toObject();
            int r = ui->tbl->addEmptyRow();
            int c = 0;
            ui->tbl->setString(r, c++, o["f_id"].toString());
            ui->tbl->setString(r, c++, o["f_prefix"].toString());
            ui->tbl->setString(r, c++, o["f_date_open"].toString());
            ui->tbl->setString(r, c++, o["f_time_open"].toString());
            ui->tbl->setString(r, c++, o["f_date_close"].toString());
            ui->tbl->setString(r, c++, o["f_time_close"].toString());
            ui->tbl->setString(r, c++, o["f_hall_name"].toString());
            ui->tbl->setString(r, c++, o["f_table_name"].toString());
            ui->tbl->setString(r, c++, o["f_staff_name"].toString());
            ui->tbl->setString(r, c++, o["f_amount_total"].toString());
            ui->tbl->setString(r, c++, o["f_fiscal"].toString());
        }

        ui->tbl->resizeColumnsToContents();
        ui->tbl->setColumnWidth(0, 0);
        ui->tblTotal->setColumnCount(ui->tbl->columnCount());

        for(int i = 0; i < ui->tbl->columnCount(); i++) {
            ui->tblTotal->setColumnWidth(i, ui->tbl->columnWidth(i));
        }

        ui->tblTotal->setString(0, 9, jdoc["total"].toString());
        QStringList l;
        l.append(QString::number(ui->tbl->rowCount()));
        ui->tblTotal->setVerticalHeaderLabels(l);
    }, [](const QJsonObject & je) {
        Q_UNUSED(je);
        return false;
    });
}

void DlgReports::setLangIcon()
{
    // switch(ui->btnReceiptLanguage->property("receipt_language").toInt()) {
    // case 0:
    //     ui->btnReceiptLanguage->setIcon(QIcon(":/armenia.png"));
    //     break;
    // case 1:
    //     ui->btnReceiptLanguage->setIcon(QIcon(":/usa.png"));
    //     break;
    // case 2:
    //     ui->btnReceiptLanguage->setIcon(QIcon(":/russia.png"));
    //     break;
    // default:
    //     ui->btnReceiptLanguage->setIcon(QIcon(":/armenia.png"));
    //     break;
    // }
}

void DlgReports::handleDailyCommon(const QJsonObject &obj)
{
    sender()->deleteLater();

    if(obj["reply"].toInt() != 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }

    QJsonArray ja = obj["report"].toArray();
    ui->tbl->clearContents();
    ui->tbl->setRowCount(0);
    double total = 0;

    for(int i = 0; i < ja.count(); i++) {
        QJsonObject o = ja.at(i).toObject();
        int r = ui->tbl->addEmptyRow();
        ui->tbl->setString(r, 0, o["f_id"].toString());
        ui->tbl->setString(r, 1, o["f_order"].toString());
        ui->tbl->setString(r, 2, o["f_datecash"].toString());
        ui->tbl->setString(r, 3, o["f_timeclose"].toString());
        ui->tbl->setString(r, 4, o["f_hall"].toString());
        ui->tbl->setString(r, 5, o["f_table"].toString());
        ui->tbl->setString(r, 6, o["f_staff"].toString());
        ui->tbl->setString(r, 7, float_str(o["f_amounttotal"].toDouble(), 2));
        ui->tbl->setString(r, 8, o["f_receiptnumber"].toString());
        total += o["f_amounttotal"].toDouble();
    }

    ui->tblTotal->setColumnCount(ui->tbl->columnCount());
    ui->tbl->resizeColumnsToContents();
    ui->tbl->setColumnWidth(0, 0);

    for(int i = 0; i < ui->tbl->columnCount(); i++) {
        ui->tblTotal->setColumnWidth(i, ui->tbl->columnWidth(i));
    }

    ui->tblTotal->setString(0, 7, float_str(total, 2));
    QStringList l;
    l.append(QString::number(ui->tbl->rowCount()));
    ui->tblTotal->setVerticalHeaderLabels(l);
}

void DlgReports::on_btnRefresh_clicked()
{
    getDailyCommon();
}

void DlgReports::on_btnExit_clicked()
{
    accept();
}

void DlgReports::on_btnReports_clicked()
{
    fHttp->createHttpQueryLambda("/engine/v2/waiter/reports/get-daily", {
        {"date1", mDate1.toString(FORMAT_DATE_TO_STR_MYSQL)},
        {"date2", mDate2.toString(FORMAT_DATE_TO_STR_MYSQL)},

    }, [ = ](const QJsonObject & jdoc) {
        int bs = 12;
        QFont font(qApp->font());
        font.setPointSize(bs);
        C5Printing p;
        QPrinterInfo pi = QPrinterInfo::printerInfo(mUser->fConfig["receipt_printer"].toString());
        QPrinter printer(pi);
        printer.setPageSize(QPageSize::Custom);
        printer.setFullPage(false);
        QRectF pr = printer.pageRect(QPrinter::DevicePixel);
        constexpr qreal SAFE_RIGHT_MM = 2.0;
        qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
        p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
        p.setFont(font);
        p.setFontBold(true);
        QString logoFile = qApp->applicationDirPath() + "/logo_receipt.png";

        if(QFile::exists(logoFile)) {
            p.image(logoFile, Qt::AlignHCenter);
            p.br();
        }

        p.br();
        p.ctext(tr("Daily report"));
        p.br();
        p.setFontBold(true);
        p.ctext(mDate1.toString(FORMAT_DATE_TO_STR));
        p.br();
        p.ctext(mDate2.toString(FORMAT_DATE_TO_STR));
        p.br();
        p.ltext(tr("Orders count"));
        p.rtext(float_str((double)jdoc["f_count_id"].toInt(), 2));
        p.br();
        p.ltext(tr("Daily revenue"));
        p.rtext(float_str((double)jdoc["f_amount_total"].toInt(), 2));
        p.br();
        p.br();

        if(jdoc["f_amount_cash"].toDouble() > 0.01) {
            p.line();
            p.br();
            p.ltext(tr("Cash"));
            p.rtext(float_str(jdoc["f_amount_cash"].toDouble(), 2));
            p.br();
        }

        if(jdoc["f_amount_card"].toDouble() > 0.01) {
            p.line();
            p.br();
            p.ltext(tr("Card"));
            p.rtext(float_str(jdoc["f_amount_card"].toDouble(), 2));
            p.br();
        }

        if(jdoc["f_service_amount"].toDouble() > 0.01) {
            p.line();
            p.br();
            p.ltext(tr("Service amount"));
            p.rtext(float_str(jdoc["f_service_amount"].toDouble(), 2));
            p.br();
        }

        p.line();
        p.br();
        p.br();
        p.br();
        p.br();
        p.setFontSize(bs - 4);
        p.setFontBold(false);
        p.ltext(QString("%1 %2").arg(tr("Printed"), QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR)));
        p.br();
        p.ltext(".");
        p.print(printer);
    }, [](const QJsonObject & jerr) {
    });
}

void DlgReports::on_btnParams_clicked()
{
    DlgReportsFilter d(mDate1, mDate2, mUser);

    if(d.exec() == QDialog::Accepted) {
        mDate1 = d.date1();
        mDate2 = d.date2();
        on_btnRefresh_clicked();
    }
}

void DlgReports::on_btnOpenReport_clicked()
{
    int row = ui->tbl->currentRow();

    if(row < 0) {
        return;
    }

    QString orderId = ui->tbl->getString(row, 0);
    DlgOrder d(mUser, {}, {}, {}, {});
    d.setOrderId(orderId);
    d.exec();
}
