#include "dlgrealreports.h"
#include "ui_dlgrealreports.h"
#include "ninterface.h"
#include "c5user.h"
#include "c5printing.h"
#include "c5printjson.h"
#include "struct_workstationitem.h"
#include "format_date.h"
#include "c5message.h"
#include "dlgreportparams.h"
#include <QListWidgetItem>
#include <QScrollBar>
#include <QFile>
#include <QShowEvent>

DlgRealReports::DlgRealReports(C5User *user)
    : C5WaiterDialog(user), ui(new Ui::DlgRealReports)
{
    ui->setupUi(this);
    ui->lbReportName->setText("");
    mPrinting = new C5Printing();
    mPrinting->setSceneParams(380, 3700, 96);
    ui->gv->setScene(mPrinting->fCanvas);
    ui->gv->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    ui->lbDate->setText(QDate::currentDate().toString(FORMAT_DATE_TO_STR));
}

DlgRealReports::~DlgRealReports()
{
    delete mPrinting;
    delete ui;
}

void DlgRealReports::showEvent(QShowEvent *e)
{
    C5WaiterDialog::showEvent(e);

    if(e->spontaneous()) {
        return;
    }

    NInterface::query1("/engine/v2/waiter/reports/get-list", mUser->mSessionKey, this,
                       {},
    [this](const QJsonObject & jdoc) {
        for(const QJsonValue  &v : jdoc["list"].toArray()) {
            const QJsonObject &jo = v.toObject();
            auto *item = new QListWidgetItem(ui->lst);
            item->setSizeHint(QSize(50, 50));
            item->setText(jo.value("title").toString());
            item->setData(Qt::UserRole, jo.value("report_id").toInt());
            item->setData(Qt::UserRole + 1, jo.value("params").toString());
            item->setData(Qt::UserRole + 2, jo.value("default_values").toObject());
            ui->lst->addItem(item);
        }
    });
}

void DlgRealReports::on_btnBack_clicked()
{
    reject();
}

void DlgRealReports::on_lst_itemClicked(QListWidgetItem *item)
{
    mLastClickedItem = item;
    mLastParams = item->data(Qt::UserRole + 2).toJsonObject();
    loadReport(mLastClickedItem);
}

void DlgRealReports::on_btnPrint_clicked()
{
    C5Printing p;
    QPrinterInfo pi = QPrinterInfo::printerInfo(mWorkStation.defaultPrinter());
    QPrinter printer(pi);
    printer.setPageSize(QPageSize::Custom);
    printer.setFullPage(false);
    QRectF pr = printer.pageRect(QPrinter::DevicePixel);
    constexpr qreal SAFE_RIGHT_MM = 4.0;
    qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
    p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
    p.setFont(qApp->font());
    QString logoFile = qApp->applicationDirPath() + "/logo_receipt.png";

    if(QFile::exists(logoFile)) {
        p.image(logoFile, Qt::AlignHCenter);
        p.br();
    }

    C5PrintJson pj;
    pj.parse(p, mLastReport);
    p.print(printer);
}

void DlgRealReports::on_btnParams_clicked()
{
    if(!mLastClickedItem) {
        C5Message::error(tr("Select report"));
        return;
    }

    DlgReportParams d(mUser, mLastClickedItem->data(Qt::UserRole + 1).toString());

    if(d.exec() == QDialog::Accepted) {
        mLastParams = d.getParams();
        loadReport(mLastClickedItem);
    }
}

void DlgRealReports::loadReport(QListWidgetItem *item)
{
    ui->lbReportName->setText(item->text());
    NInterface::query1("/engine/v2/waiter/reports/get-report", mUser->mSessionKey, this, {
        {"report_id", item->data(Qt::UserRole).toInt()},
        {"report_name", item->text()},
        {"params", mLastParams}
    },
    [this](const QJsonObject & jdoc) {
        mLastReport = jdoc["printing"].toArray();
        C5PrintJson pj;
        mPrinting->reset();
        mPrinting->fCanvas->clear();
        pj.parse(*mPrinting, jdoc["printing"].toArray());
        ui->gv->verticalScrollBar()->setValue(0);
    });
}

void DlgRealReports::on_btnReload_clicked()
{
    if(!mLastClickedItem) {
        return;
    }

    loadReport(mLastClickedItem);
}
