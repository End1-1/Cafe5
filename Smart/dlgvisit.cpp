#include "dlgvisit.h"
#include "ui_dlgvisit.h"
#include "c5config.h"
#include "c5utils.h"
#include "c5message.h"
#include "c5printing.h"

dlgvisit::dlgvisit(const QString &code, QWidget *parent) : C5Dialog(parent), ui(new Ui::dlgvisit),  fCode(code)
{
    ui->setupUi(this);
    fHttp->createHttpQueryLambda("engine/smart/visitcard.php", QJsonObject{{"method", "get"}, {"code", code}, {"config_id", __c5config.fSettingsName}},
    [this](const QJsonObject & jdoc) {
        qDebug() << jdoc;
        const QJsonObject &jcard = jdoc["card"].toObject();
        ui->lbCardNumber->setText(jcard["f_number"].toString());
        fBalance = jdoc["f_balance"].toDouble();
        ui->lbBalance->setText(float_str(fBalance, 2));
        ui->lbCustomer->setText(jdoc["f_customer"].toString());
        fPrice = jcard["f_value"].toDouble();
        ui->lbVisitPrice->setText(float_str(fPrice, 2));
    },
    [](const QJsonObject & je) {
        qDebug() << je;
    });
}

dlgvisit::~dlgvisit() { delete ui; }

void dlgvisit::on_btnClose_clicked()
{
    reject();
}

void dlgvisit::on_btnVisit_clicked()
{
    if(fBalance < fPrice) {
        C5Message::error(tr("Recharge the card"));
        return;
    }

    fHttp->createHttpQueryLambda("engine/smart/visitcard.php", QJsonObject{{"method", "visit"}, {"code", fCode}, {"config_id", __c5config.fSettingsName}},
    [this](const QJsonObject & jdoc) {
        QFont font;
        font.setFamily(__c5config.getValue(param_service_print_font_family));
        int basesize = __c5config.getValue(param_service_print_font_size).toInt() + 8;
        font.setPointSize(basesize);
        C5Printing p;
        p.setSceneParams(650, 2800, QPageLayout::Portrait);
        p.setFont(font);
        p.setFontBold(true);
        p.ctext(tr("Date") + " " + QDate::currentDate().toString(FORMAT_DATE_TO_STR));
        p.br();
        p.ctext(tr("Time") + " " + QTime::currentTime().toString(FORMAT_TIME_TO_STR));
        p.br();
        p.ctext(ui->lbCustomer->text());
        p.br();
        //p.setFontBold(false);
        p.line(3);
        p.br(3);
        p.br();
        p.lrtext(tr("Remain"), QString::number(jdoc["f_remain"].toInt()));
        p.br();
        p.setFontStrike(false);
        p.setFontSize(basesize - 4);
        // p.setFontBold(false);
        p.ltext(QString("%1 %2").arg(tr("[1] Printed:"), QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR)), 0);
        p.br();
        p.br();
        p.ltext("_", 0);
        p.br();
        QString finalPrinter = C5Config::localReceiptPrinter();
        p.print(finalPrinter, QPageSize::Custom);
        accept();
        return;
    },
    [](const QJsonObject & je) {
        qDebug() << je;
    });
}
