#include "dlgstoplistoption.h"
#include "ui_dlgstoplistoption.h"
#include "c5menu.h"
#include "dlgorder.h"
#include "stoplist.h"
#include "dlgviewstoplist.h"
#include "c5printing.h"
#include "c5logtoserverthread.h"
#include "c5user.h"

DlgStopListOption::DlgStopListOption(DlgOrder *o) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgStopListOption),
    fDlgOrder(o)
{
    ui->setupUi(this);
    if (fDlgOrder->stoplistMode()) {
        ui->btnSetStoplist->setText(tr("Stoplist editing finished"));
    } else {
        ui->btnSetStoplist->setText(tr("Set stoplist"));
    }
}

DlgStopListOption::~DlgStopListOption()
{
    delete ui;
}

void DlgStopListOption::handleStopList(const QJsonObject &o)
{
    if (o["reply"].toInt() == 1) {
        C5Menu::fStopList.clear();
        accept();
        C5Message::info(tr("The stoplist was removed"));
    } else {
        C5Message::error(o["msg"].toString());
    }
}

void DlgStopListOption::handlePrintStopList(const QJsonObject &obj)
{
    if (obj["reply"].toInt() == 1) {
        accept();
        C5Menu::fStopList.clear();
        QJsonArray jga = obj["list"].toArray();
        for (int i = 0; i < jga.count(); i++) {
            QJsonObject jgo = jga.at(i).toObject();
            C5Menu::fStopList[jgo["dish"].toInt()] = jgo["qty"].toDouble();
        }
        if (C5Menu::fStopList.isEmpty()) {
            return;
        }
        QFont font(qApp->font());
        font.setPointSize(20);
        C5Printing p;
        p.setSceneParams(__c5config.getValue(param_print_paper_width).toInt(), 2800, QPrinter::Portrait);
        p.setFont(font);
        p.ctext(tr("STOPLIST"));
        p.br();
        p.ctext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
        p.br();
        p.br();
        p.line();
        p.br(2);
        p.line();
        p.br();
        for (QMap<int, double>::const_iterator it = C5Menu::fStopList.begin(); it != C5Menu::fStopList.end(); it++) {
            p.ltext(dbdish->name(it.key()), 0);
            p.rtext(float_str(it.value(), 2));
            p.br();
            p.line();
            p.br();
        }
        p.br();
        p.ltext(".", 0);
        p.br();
        p.print(__c5config.localReceiptPrinter(),  QPrinter::Custom);
    } else {
        C5Message::error(obj["msg"].toString());
    }
}

void DlgStopListOption::on_btnCancel_clicked()
{
    reject();
}

void DlgStopListOption::on_btnClearStopList_clicked()
{
    if (C5Message::question(tr("Are sure to clear stoplist?")) == QDialog::Accepted) {
        C5LogToServerThread::remember(LOG_WAITER, fDlgOrder->fUser->fullName(), "", "", "", "Stop list was removed", "", "");
        auto *sh = createSocketHandler(SLOT(handleStopList(QJsonObject)));
        sh->bind("cmd", sm_stoplist);
        sh->bind("state", sl_remove);
        sh->send();
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
    auto *sh = createSocketHandler(SLOT(handlePrintStopList(QJsonObject)));
    sh->bind("cmd", sm_stoplist);
    sh->bind("state", sl_get);
    sh->send();
}
