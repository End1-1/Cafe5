#include "dlgstoplistoption.h"
#include "ui_dlgstoplistoption.h"
#include "dlgorder.h"
#include "c5tabledata.h"
#include "c5message.h"
#include "c5printing.h"
#include "ninterface.h"
#include "c5user.h"
#include "c5utils.h"
#include "c5user.h"

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
    accept();
}

void DlgStopListOption::printStopListResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
    C5TableData::instance()->setStopList(jdoc["list"].toArray());
    accept();

    if(C5TableData::instance()->mStopList.isEmpty()) {
        return;
    }

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

        for(QMap<int, double>::const_iterator it = C5TableData::instance()->mStopList.constBegin();
                it != C5TableData::instance()->mStopList.constEnd(); it++) {
            if(sq.value().contains(it.key())) {
                p.ltext(tds("d_dish", "f_name", it.key()), 0);
                p.rtext(float_str(it.value(), 2));
                p.br();
                p.line();
                p.br();
            }
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
        fHttp->createHttpQuery("/engine/waiter/stoplist.php", QJsonObject{ {"action", "remove"}}, SLOT(removeStopListResponse(
                    QJsonObject)));
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
    fHttp->createHttpQuery("/engine/waiter/stoplist.php", QJsonObject{{"action", "get"}}, SLOT(printStopListResponse(
                QJsonObject)));
}
