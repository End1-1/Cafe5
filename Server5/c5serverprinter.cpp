#include "c5serverprinter.h"
#include "c5printing.h"
#include "c5utils.h"
#include "notificationwidget.h"
#include <QSettings>

static QSettings ___s(_ORGANIZATION_, _APPLICATION_);

C5ServerPrinter::C5ServerPrinter(const QJsonObject &j, QObject *parent)
    : QObject{parent}
{
    jo = j;
    fHttp = new NInterface(this);
}

C5ServerPrinter::~C5ServerPrinter()
{
    fHttp->deleteLater();
}

void C5ServerPrinter::run()
{
    if (jo.contains("chat")) {
        handleChat();
        return;
    }
    QStringList dishes;
    QJsonObject jcomplete;
    QString originalPrinter = ___s.value("receipprinter").toString();
    QJsonObject jconfig = jo["config"].toObject();
    QString printField = ___s.value("printerside").toString();
    int fontSize = 18;
    QFont font("Arial LatArm Unicode", fontSize);
    font.setPointSize(fontSize);
    C5Printing p;
    p.setSceneParams(600, 2800, QPrinter::Portrait);
    p.setFont(font);
    p.setFontBold(false);
    p.setFontSize(fontSize);
    p.ctext("Նոր պատվեր");
    p.br();
    p.ltext("Սեղան", 0);
    p.setFontSize(fontSize);
    p.rtext(jo["table"].toString());
    p.br();
    p.setFontSize(fontSize);
    p.ltext(tr("Պատվեր"), 0);
    p.rtext(jo["hallid"].toString());
    p.br();
    p.ltext(tr("Ամսաթիվ"), 0);
    p.rtext(QDate::currentDate().toString(FORMAT_DATE_TO_STR));
    p.br();
    p.ltext(tr("Ժամ"), 0);
    p.rtext(QTime::currentTime().toString(FORMAT_TIME_TO_STR));
    p.br();
    p.line(0, p.fTop, p.fNormalWidth, p.fTop);
    p.br(2);
    p.setFontSize(fontSize);
    QSet<QString> storages;
    QString part1;
    p.setFontBold(true);
    p.ltext("Քնկ", 0);
    p.ltext("Անվանում", 50, 400);
    p.rtext("Գին");
    p.br();
    p.line(2);
    p.setFontBold(false);
    for (int i = 0; i < jo["dishes"].toArray().size(); i++) {
        QJsonObject jdish = jo["dishes"].toArray().at(i).toObject();
        if (jdish[printField] != originalPrinter) {
            continue;
        }
        if (part1.isEmpty()) {
            part1 = jdish["part1"].toString();
            dishes.append(part1);
            dishes.append("---------------");
            p.ltext(QString("%1").arg(jdish["part1"].toString()), 0);
            p.br();
            p.line();
        } else {
            if (part1 != jdish["part1"].toString()) {
                p.br();
                part1 = jdish["part1"].toString();
                dishes.append("");
                dishes.append(part1);
                dishes.append("---------------");
                p.ltext(QString("%1").arg(jdish["part1"].toString()), 0);
                p.br();
                p.line();
            }
        }
        jcomplete[jdish["obid"].toString()] = printField;
        dishes.append(float_str(jdish["qty1"].toDouble(), 2) + "   " + jdish["dishname"].toString());
        p.ltext(QString("%1").arg(jdish["dishname"].toString()), 50);
        p.setFontBold(true);
        p.ltext(QString("%1").arg(float_str(jdish["qty1"].toDouble(), 2)), 0);
        p.rtext(float_str(jdish["price"].toDouble(), 2));
        p.setFontBold(false);
        if (jdish["dishcomment"].toString().length() > 0) {
            dishes.append(jdish["dishcomment"].toString());
            p.br();
            p.setFontSize(fontSize + 10);
            p.setFontBold(true);
            p.ltext(jdish["dishcomment"].toString(), 0);
            p.br();
            p.setFontSize(fontSize);
            p.setFontBold(false);
        }
        p.br();
        p.line(0, p.fTop, p.fNormalWidth, p.fTop);
        p.br(1);
    }
    p.line(0, p.fTop, p.fNormalWidth, p.fTop);
    p.br(1);
    p.setFontBold(true);
    p.setFontSize(fontSize + 6);
    p.ltext("Վճարումը՝", 0);
    p.br();
    dishes.append("");
    if (jo["servicefactor"].toDouble() > 0.01) {
        p.ltext(QString("Սպասարկում +%1%").arg(float_str(jo["servicefactor"].toDouble(), 2)), 0);
        p.rtext(QString("%1 Դ").arg(float_str(jo["serviceamount"].toDouble(), 2)));
        p.br();
        dishes.append(QString("Սպասարկում +%1%").arg(float_str(jo["servicefactor"].toDouble(), 2)));
    }
    if (jo["cashamount"].toDouble() > 0) {
        p.ltext("Կանխիկ", 0);
        p.rtext(QString("%1 Դ").arg(float_str(jo["cashamount"].toDouble(), 2)));
        p.br();
        dishes.append(QString("Վճարումը կանխիկ %1Դ").arg(float_str(jo["cashamount"].toDouble(), 2)));
    }
    if (jo["cardamount"].toDouble() > 0) {
        p.ltext("Քարտ", 0);
        p.rtext(QString("%1 Դ").arg(float_str(jo["cardamount"].toDouble(), 2)));
        dishes.append(QString("Վճարումը քարտով՝ %1Դ").arg(float_str(jo["cardamount"].toDouble(), 2)));
        p.br();
    }
    if (jo["idramamount"].toDouble() > 0) {
        p.ltext("Իդրամ", 0);
        p.rtext(QString("%1 Դ").arg(float_str(jo["idramamount"].toDouble(), 2)));
        dishes.append(QString("Վճարումը Իդրամով՝ %1Դ").arg(float_str(jo["idramamount"].toDouble(), 2)));
        p.br();
    }
    p.br();
    p.br();
    p.setFontSize(20);
    if (!jo["headercomment"].toString().isEmpty()) {
        p.ltext("Հաճախորդի հաղորդագրություն", 0);
        p.br();
        p.ltext(jo["headercomment"].toString(), 0);
        p.br();
        dishes.append("<<<<<<<<<<<<<<<<<");
        dishes.append(jo["headercomment"].toString());
    }
    p.ltext(tr("Տպիչ: ") + originalPrinter, 0);
    p.rtext(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"));
    p.br();
    p.setFontBold(true);
    p.rtext(printField == "print1" ? " [1]" : "[2]");
    p.br();
    if (jcomplete.isEmpty()) {
        emit finished();
        return;
    }
    QString final = "OK";
    if (!p.print(originalPrinter, QPrinter::Custom)) {
        final = "FAIL";
    }
    if (!jcomplete.isEmpty()) {
        fHttp->fErrorSlot = (char *) SLOT(httpError(QString));
        fHttp->fErrorObject = this;
        fHttp->createHttpQuery("/engine/kinopark/orders.php", QJsonObject{
            {"action", "complete"},
            {"ids", jcomplete}
        }, SLOT(completeResponse(QJsonObject)), "", false);
    }
    emit showNotificationWidget(QJsonObject{{"message", jo["table"].toString()
            + "       "
            + jo["hallid"].toString()
            + "<br>"
            + dishes.join("<br>")
        },
        {"id", jo["header"].toString()}});
}

void C5ServerPrinter::handleChat()
{
    QStringList dishes;
    QJsonObject jcomplete;
    QString originalPrinter = ___s.value("receipprinter").toString();
    QJsonObject jconfig = jo["config"].toObject();
    QString printField = ___s.value("printerside").toString();
    int fontSize = 22;
    QFont font(jconfig["font_family"].toString(), fontSize);
    font.setPointSize(20);
    C5Printing p;
    p.setSceneParams(600, 2800, QPrinter::Portrait);
    p.setFont(font);
    p.setFontBold(false);
    p.setFontSize(fontSize);
    p.setFontSize(20);
    p.ctext(jo["f_last"].toString()
            + jo["f_first"].toString());
    p.br();
    p.ltext("Հաճախորդի հաղորդագրություն", 0);
    p.br();
    p.ltext(jo["text"].toString(), 0);
    p.br();
    p.br();
    p.br();
    p.br();
    p.br();
    p.br();
    p.br();
    p.ltext(tr("Printer: ") + originalPrinter, 0);
    p.setFontBold(true);
    QString final = "OK";
    if (!p.print(originalPrinter, QPrinter::Custom)) {
        final = "FAIL";
    }
}

void C5ServerPrinter::completeResponse(const QJsonObject &jdoc)
{
    qDebug() << jdoc;
    emit finished();
}

void C5ServerPrinter::httpError(const QString &err)
{
    qDebug() << err;
    emit finished();
}
