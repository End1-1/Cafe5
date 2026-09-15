#include "printreceiptgroup.h"
#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QPageSize>
#include <QPrinter>
#include <QPrinterInfo>
#include "c5message.h"
#include "c5printing.h"
#include "c5user.h"
#include "c5utils.h"
#include "format_date.h"
#include "httplite.h"
#include "ninterface.h"
#include "struct_workstationitem.h"

PrintReceiptGroup::PrintReceiptGroup(QObject *parent) :
    QObject(parent)
{
}

void PrintReceiptGroup::print(const QString &id, int rw)
{
    Q_UNUSED(id);
    Q_UNUSED(rw);
}

void PrintReceiptGroup::print2(const QString &id, C5User *user, QObject *context)
{
    if (id.isEmpty()) {
        C5Message::error(QObject::tr("Document is not exists"));
        return;
    }
    if (!user) {
        C5Message::error(QObject::tr("Unauthorized"));
        return;
    }
    QObject *ctx = context ? context : user;
    NInterface::query1(QStringLiteral("/engine/v2/shop/view-order/get"),
                       user->mSessionKey,
                       ctx,
                       {{QStringLiteral("id"), id}},
                       [](const QJsonObject &jo) { PrintReceiptGroup::printOrder(jo); });
}

void PrintReceiptGroup::printOrder(const QJsonObject &jo)
{
    if (!mWorkStation.isReceiptPrintingConfigured()) {
        return;
    }
    const QJsonObject header = jo.value(QStringLiteral("header")).toObject();
    const QJsonArray goods = jo.value(QStringLiteral("goods")).toArray();
    if (header.isEmpty()) {
        C5Message::error(QObject::tr("Document is not exists"));
        return;
    }

    const QString printerName = mWorkStation.receiptPrinter();
    QPrinterInfo pi;
    if (mWorkStation.hasReceiptPrinter()) {
        pi = QPrinterInfo::printerInfo(printerName);
    }

    const int bs = 20;
    QFont font(qApp->font());
    font.setPointSize(bs);
    C5Printing p;
    QPrinter printer(pi.isNull() ? QPrinterInfo() : pi);
    if (!pi.isNull()) {
        p.setSceneFromPrinter(printer);
    } else {
        p.setSceneParams(650, 2800, 96);
    }
    p.setFont(font);
    p.setFontSize(bs);

    const int marginMm = mWorkStation.receiptMarginsMm();
    p.setRightMarginMm(marginMm);
    const int colQty = 33 + marginMm;
    const int colPrice = 41 + marginMm;
    const int nameWidthMm = qMax(10, 35 - 2 * marginMm);

    const QString logoFile = qApp->applicationDirPath() + QStringLiteral("/logo_receipt.png");
    if (QFile::exists(logoFile)) {
        p.image(logoFile, Qt::AlignHCenter);
        p.br();
    }

    const double amountTotal = header.value(QStringLiteral("f_amounttotal")).toDouble();
    if (amountTotal < 0) {
        p.ctext(QObject::tr("Return"));
        p.br();
    }

    const QJsonObject jtax = header.value(QStringLiteral("f_fiscal")).toObject();
    if (jtax.value(QStringLiteral("rseq")).toInt() > 0) {
        p.ltext(jtax.value(QStringLiteral("taxpayer")).toString(), marginMm);
        p.br();
        p.ltext(jtax.value(QStringLiteral("address")).toString(), marginMm);
        p.br();
        p.ltext(QObject::tr("TIN"), marginMm);
        p.rtext(jtax.value(QStringLiteral("tin")).toString());
        p.br();
        p.ltext(QObject::tr("Device number"), marginMm);
        p.rtext(jtax.value(QStringLiteral("crn")).toString());
        p.br();
        p.ltext(QObject::tr("Serial"), marginMm);
        p.rtext(jtax.value(QStringLiteral("sn")).toString());
        p.br();
        p.ltext(QObject::tr("Fiscal"), marginMm);
        p.rtext(jtax.value(QStringLiteral("fiscal")).toString());
        p.br();
        p.ltext(QObject::tr("Receipt number"), marginMm);
        p.rtext(QString::number(jtax.value(QStringLiteral("rseq")).toInt()));
        p.br();
        p.ltext(QObject::tr("Date"), marginMm);
        if (jtax.contains(QStringLiteral("time"))) {
            p.rtext(QDateTime::fromMSecsSinceEpoch(jtax.value(QStringLiteral("time")).toVariant().toLongLong())
                        .toString(FORMAT_DATETIME_TO_STR));
        } else {
            const QString dateCash = header.value(QStringLiteral("f_datecash")).toString();
            const QString timeClose = header.value(QStringLiteral("f_time_close")).toString();
            p.rtext(QStringLiteral("%1 %2").arg(dateCash, timeClose));
        }
        p.br();
        p.ltext(QObject::tr("(F)"), marginMm);
        p.br();
    }

    const QString buyer = header.value(QStringLiteral("f_buyer")).toString().trimmed();
    if (!buyer.isEmpty()) {
        p.ltext(QObject::tr("Partner"), marginMm);
        p.br();
        p.ltext(buyer, marginMm);
        p.br();
    }

    p.setFontBold(true);
    p.ctext(QStringLiteral("#%1").arg(header.value(QStringLiteral("f_number")).toString()));
    p.br();
    p.setFontBold(false);

    p.setFontSize(bs - 2);
    p.ltext(QObject::tr("Name"), marginMm);
    p.ltext(QObject::tr("Qty"), colQty);
    p.ltext(QObject::tr("Price"), colPrice);
    p.rtext(QObject::tr("Amount"));
    p.br();
    p.line(2);
    p.br(2);

    double linesTotal = 0;
    for (const QJsonValue &jv : goods) {
        const QJsonObject g = jv.toObject();
        const double qty = g.value(QStringLiteral("f_qty")).toDouble();
        const double price = g.value(QStringLiteral("f_price")).toDouble();
        const double total = g.value(QStringLiteral("f_total")).toDouble();
        linesTotal += total;

        QString name = g.value(QStringLiteral("f_name")).toString();
        const QString scancode = g.value(QStringLiteral("f_scancode")).toString();
        if (!scancode.isEmpty()) {
            name = QStringLiteral("%1 %2").arg(name, scancode);
        }
        p.ltext(name, marginMm, nameWidthMm);
        p.ltext(float_str(qty, 3), colQty, 8);
        p.ltext(float_str(price, 2), colPrice, 12);
        p.rtext(float_str(total, 2));
        p.br();
        p.line();
        p.br(1);
    }

    p.setFontBold(true);
    p.ltext(QObject::tr("Need to pay"), marginMm);
    p.rtext(float_str(amountTotal != 0 ? amountTotal : linesTotal, 2));
    p.br();
    p.setFontBold(false);
    p.line();
    p.br();

    auto printPay = [&p, marginMm](const QString &title, double amount) {
        if (qAbs(amount) > 0.001) {
            p.ltext(title, marginMm);
            p.rtext(float_str(amount, 2));
            p.br();
        }
    };
    printPay(QObject::tr("Payment, cash"), header.value(QStringLiteral("f_amount_cash")).toDouble());
    printPay(QObject::tr("Payment, card"), header.value(QStringLiteral("f_amount_card")).toDouble());
    printPay(QObject::tr("Payment, idram"), header.value(QStringLiteral("f_amount_idram")).toDouble());
    printPay(QObject::tr("Payment, telcell"), header.value(QStringLiteral("f_amount_telcell")).toDouble());
    printPay(QObject::tr("Payment, bank"), header.value(QStringLiteral("f_amount_bank")).toDouble());
    printPay(QObject::tr("Payment, debt"), header.value(QStringLiteral("f_amount_debt")).toDouble());
    printPay(QObject::tr("Prepaid"), header.value(QStringLiteral("f_amount_prepaid")).toDouble());

    const QString comment = header.value(QStringLiteral("f_comment")).toString().trimmed();
    if (!comment.isEmpty()) {
        p.br();
        p.ltext(comment, marginMm);
        p.br();
    }

    p.br();
    p.ctext(QObject::tr("Thank you for visit!"));
    p.br();
    p.ltext(QObject::tr("Printed"), marginMm);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();

    // 1) print_server set → HTTP; 2) receipt_printer set → local Qt; neither → already skipped above
    if (mWorkStation.usePrintServer()) {
        auto *http = new HttpLite(qApp);
        QJsonObject json;
        json.insert(QStringLiteral("print_data"), p.jsonData());
        json.insert(QStringLiteral("printer_name"), printerName);
        http->post(mWorkStation.printServer(), json);
    }
    if (mWorkStation.hasReceiptPrinter()) {
        if (!pi.isNull()) {
            p.print(printer);
        } else {
            C5Message::error(QObject::tr("Printer not found") + ": " + printerName);
        }
    }
}

void PrintReceiptGroup::print3(const QString &id)
{
    Q_UNUSED(id);
}
