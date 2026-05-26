#include "dlgstoplistoption.h"
#include <QPrinterInfo>
#include "c5message.h"
#include "c5printing.h"
#include "c5user.h"
#include "c5utils.h"
#include "dlgorder.h"
#include "format_date.h"
#include "ninterface.h"
#include "ui_dlgstoplistoption.h"

DlgStopListOption::DlgStopListOption(DlgOrder *o, C5User *u) :
    C5Dialog(u),
    ui(new Ui::DlgStopListOption),
    fDlgOrder(o)
{
    ui->setupUi(this);

    if(fDlgOrder->stoplistMode()) {
        ui->btnSetStoplist->setText(tr("Stoplist editing finished"));
    } else {
        ui->btnSetStoplist->setText(tr("Set stoplist"));
    }
}

DlgStopListOption::~DlgStopListOption()
{
    delete ui;
}

void DlgStopListOption::removeStopListResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
    C5Message::info(tr("The stoplist was removed"));
    fDlgOrder->updateStopList(jdoc.value("stoplist").toArray());
    accept();
}

void DlgStopListOption::printStopListResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
    accept();

    QList<int> menu; //QList<int> menu = dbmenu->list();
    QMap<QString, QList<int> > printList;

    // for (int id: menu) {
    //     // if (C5Menu::fStopList.contains(dbmenu->dishid(id))) {
    //     //     printList[dbmenu->print1(id)].append(dbmenu->dishid(id));
    //     // }
    // }
    for(QMap<QString, QList<int> >::const_iterator sq = printList.constBegin(); sq != printList.constEnd(); sq++) {
        QFont font(qApp->font());
        font.setPointSize(20);
        C5Printing p;
        QPrinterInfo pinfo = QPrinterInfo::printerInfo(mUser->fConfig["receipt_printer"].toString());
        QPrinter printer(pinfo);
        QRectF pr = printer.pageRect(QPrinter::DevicePixel);
        p.setSceneParams(pr.width(), pr.height(), printer.logicalDpiX());
        p.setFont(font);
        p.ctext(tr("STOPLIST"));
        p.br();
        p.ctext(sq.key());
        p.br();
        p.ctext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
        p.br();
        p.br();
        p.line();
        p.br(2);
        p.line();
        p.br();

        const QJsonArray &ja = jdoc.value("stoplist").toArray();
        for (int i = 0; i < ja.size(); i++) {
            auto const &d = ja.at(i).toObject();
            p.lrtext(d.value("f_dish_name").toString(), d.value("f_qty").toString());

            p.br();
            p.line();
            p.br();
        }

        p.br();
        p.ltext(".", 0);
        p.br();
        p.print(printer);
    }
}

void DlgStopListOption::on_btnCancel_clicked()
{
    reject();
}

void DlgStopListOption::on_btnClearStopList_clicked()
{
    if(C5Message::question(tr("Are sure to clear stoplist?")) == QDialog::Accepted) {
        fHttp->createHttpQuery("/engine/v2/waiter/stoplist/remove-stoplist", {}, SLOT(removeStopListResponse(QJsonObject)));
    }
}

void DlgStopListOption::on_btnSetStoplist_clicked()
{
    accept();
    fDlgOrder->setStoplistmode();
}

void DlgStopListOption::on_btnViewStopList_clicked()
{
    accept();
    fDlgOrder->viewStoplist();
}

void DlgStopListOption::on_btnPrintStoplist_clicked()
{
    fHttp->createHttpQuery("/engine/v2/waiter/stoplist/get", {}, SLOT(printStopListResponse(QJsonObject)));
}
