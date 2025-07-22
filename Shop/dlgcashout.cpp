#include "dlgcashout.h"
#include "ui_dlgcashout.h"
#include "c5user.h"
#include "c5config.h"
#include "c5message.h"
#include "c5utils.h"
#include <QValidator>

DlgCashout::DlgCashout()
    : C5Dialog()
    , ui(new Ui::DlgCashout)
{
    ui->setupUi(this);
    ui->leAmount->setValidator(new QDoubleValidator(0, 99999999, 0));
}

DlgCashout::~DlgCashout()
{
    delete ui;
}

void DlgCashout::on_btnCancel_clicked()
{
    reject();
}

void DlgCashout::on_btnSave_clicked()
{
    if (ui->leAmount->getDouble() < 1) {
        C5Message::error(tr("What did you try to save?"));
        return;
    }
    if (ui->tePurpose->toPlainText().isEmpty()) {
        C5Message::error(tr("Please, explain your spend"));
        return;
    }
    fHttp->createHttpQuery("/engine/cashdesk/create.php",
    QJsonObject{
        {"date", QDate::currentDate().toString(FORMAT_DATE_TO_STR_MYSQL)},
        {"operator", __user->id() },
        {"cashin", 0},
        {"cashout", __c5config.cashId()},
        {"remarks", ui->tePurpose->toPlainText()},
        {"amount", ui->leAmount->text()}
    }, SLOT(responseOfCreate(QJsonObject)));
}

void DlgCashout::responseOfCreate(const QJsonObject &jdoc)
{
    Q_UNUSED(jdoc);
    fHttp->httpQueryFinished(sender());
    accept();
}
