#include "c5printrecipta4.h"
#include "c5database.h"
#include "c5utils.h"
#include "c5config.h"
#include "QRCodeGenerator.h"
#include "logwriter.h"
#include <QFile>
#include <QApplication>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QTextDocument>

C5PrintReciptA4::C5PrintReciptA4(const QString &orderid, C5User *user, QObject *parent) :
    QObject(parent),
    fOrderUUID(orderid),
    mUser(user)
{
}

QString C5PrintReciptA4::loadTemplate(const QString &name)
{
    QString fileName = QCoreApplication::applicationDirPath() + "/templates/" + name;
    QFile f(fileName);

    if(!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LogWriter::write(LogWriterLevel::errors, "Printing template not found", fileName);
        return QString();
    }

    return QString::fromUtf8(f.readAll());
}

QString C5PrintReciptA4::applyTemplate(QString html,
                                       const QMap<QString, QString>& vars)
{
    for(auto it = vars.begin(); it != vars.end(); ++it) {
        html.replace("{{" + it.key() + "}}", it.value());
    }

    return html;
}

QString C5PrintReciptA4::makeGoodsTable(const QList<QMap<QString, QVariant>> &body)
{
    QString h;
    QTextStream s(&h);
    s << "<table>";
    s << "<tr>"
      << "<th>NN</th>"
      << "<th>Կոդ</th>"
      << "<th>Ապրանք</th>"
      << "<th class='right'>Քանակ</th>"
      << "<th>Չափ․մ․</th>"
      << "<th class='right'>Գին</th>"
      << "<th class='right'>Զեղչ %</th>"
      << "<th class='right'>Զեղչված գին</th>"
      << "<th class='right'>Ընդամենը</th>"
      << "</tr>";
    for(int i = 0; i < body.count(); ++i) {
        const auto &m = body[i];
        double price = m["f_price"].toDouble();
        double disc = m["f_discountfactor"].toDouble();
        double discounted = price - price * disc / 100.0;
        s << "<tr>";
        s << "<td class='center'>" << i + 1 << "</td>";
        s << "<td>" << m["f_scancode"].toString() << "</td>";
        s << "<td>" << m["f_goodsname"].toString() << "</td>";
        s << "<td class='right'>" << float_str(m["f_qty"].toDouble(), 2) << "</td>";
        s << "<td>" << m["f_unitname"].toString() << "</td>";
        s << "<td class='right'>" << float_str(price, 2) << "</td>";
        s << "<td class='right'>" << float_str(disc, 2) << "%</td>";
        s << "<td class='right'>" << float_str(discounted, 2) << "</td>";
        s << "<td class='right'>" << float_str(discounted * m["f_qty"].toDouble(), 2) << "</td>";
        s << "</tr>";
    }
    s << "</table>";
    return h;
}

bool C5PrintReciptA4::print(QString &err)
{
    QString html = loadTemplate("receipt_a4.html");
    if(html.isEmpty()) {
        err =  "Template not found";
        return false;
    }
    C5Database db;
    db[":f_id"] = fOrderUUID;
    db.exec("select * from o_draft_sale where f_id=:f_id");
    bool oops = false;
    bool isDraft;
    if(db.nextRow() == false) {
        oops = true;
    }
    if(oops) {
        db[":f_id"] = fOrderUUID;
        db.exec("select * from o_header where f_id=:f_id");
        if(db.nextRow() == false) {
            err = "Order not exists";
            return false;
        }
        isDraft = db.getInt("f_state") != 2;
    } else {
        isDraft = db.getInt("f_state") == 1;
    }
    db[":f_id"] = fOrderUUID;
    if(isDraft) {
        db.exec("select '--' as f_ordernumber, ost.f_name as f_saletypename, "
                "o.f_amount as f_amounttotal, 0 as f_amountcash, 0 as f_amountcard, 0 as f_amountother, o.f_date, "
                "p.f_taxcode, p.f_taxname, p.f_address, o.f_datefor, "
                "concat_ws(' ', u.f_last, u.f_first) as f_staff, "
                "CONCAT(DATE(o.f_date), ' ', TIME(o.f_time)) AS lu, o.f_id, "
                "o.f_partner "
                "from  o_draft_sale o "
                "left join o_sale_type ost on ost.f_id=o.f_saletype "
                "left join s_user u on u.f_id=o.f_staff "
                "left join c_partners p on p.f_id=o.f_partner "
                "where o.f_id=:f_id ");
    } else {
        db.exec("select concat(o.f_prefix, o.f_hallid) as f_ordernumber, ost.f_name as f_saletypename, "
                "o.f_amounttotal, o.f_amountcash, o.f_amountcard, o.f_amountother, o.f_datecash, "
                "p.f_taxcode, p.f_taxname, p.f_address, ds.f_datefor, "
                "concat_ws(' ', u.f_last, u.f_first) as f_staff, "
                "o.lu, o.f_id, o.f_partner  "
                "from o_header o "
                "left join o_draft_sale ds on ds.f_id=o.f_id "
                "left join o_sale_type ost on ost.f_id=o.f_saletype "
                "left join s_user u on u.f_id=o.f_staff "
                "left join c_partners p on p.f_id=o.f_partner "
                "where o.f_id=:f_id ");
    }
    QMap<QString, QVariant> header;

    if(db.nextRow()) {
        db.rowToMap(header);
    } else {
        err = "Order not exists";
        return false;
    }

    QMap<QString, QVariant> debt1, debt2;

    if(header["f_partner"].toInt() > 0) {
        db[":f_costumer"] = header["f_partner"];
        db[":f_order"] = fOrderUUID;
        db.exec("select 0 as a, sum(f_amount) as dd from b_clients_debts "
                "where f_costumer=:f_costumer and (f_order<>:f_order or f_order is null) "
                "union "
                "select 1 as a, sum(f_amount) as dd from b_clients_debts "
                "where f_costumer=:f_costumer and f_order=:f_order "
                "order by 1");
        db.nextRow();
        db.rowToMap(debt1);
        db.nextRow();
        db.rowToMap(debt2);
    }

    QList<QMap<QString, QVariant> > body;
    db[":f_header"] = fOrderUUID;

    if(isDraft) {
        db.exec("select g.f_scancode, g.f_name as f_goodsname, "
                "ob.f_qty, ob.f_price, ob.f_qty*ob.f_price as f_total, "
                "s.f_name as f_storename, gu.f_name as f_unitname, ob.f_discount as f_discountfactor "
                "from o_draft_sale_body ob "
                "left join c_goods g on g.f_id=ob.f_goods "
                "left join c_storages s on s.f_id=ob.f_store "
                "left join c_units gu on gu.f_id=g.f_unit "
                "where ob.f_header=:f_header "
                "order by ob.f_row ");
    } else {
        db.exec("select g.f_scancode, g.f_name as f_goodsname, ob.f_qty, ob.f_price, ob.f_total, "
                "s.f_name as f_storename, gu.f_name as f_unitname, ob.f_discountfactor * 100 as f_discountfactor "
                "from o_goods ob "
                "left join c_goods g on g.f_id=ob.f_goods "
                "left join c_storages s on s.f_id=ob.f_store "
                "left join c_units gu on gu.f_id=g.f_unit "
                "where ob.f_header=:f_header "
                "order by ob.f_row ");
    }

    while(db.nextRow()) {
        QMap<QString, QVariant> b;
        db.rowToMap(b);
        body.append(b);
    }

    int levelIndex = 1;
    int versionIndex = 0;
    bool bExtent = true;
    int maskIndex = -1;
    QString encodeString = QString("doc;%1;%2;%3").arg(header["f_id"].toString(),
                           isDraft ? tr("Draft") : header["f_ordernumber"].toString(),
                           header["lu"].toString());
    CQR_Encode qrEncode;
    bool successfulEncoding = qrEncode.EncodeData(levelIndex, versionIndex, bExtent, maskIndex,
                              encodeString.toUtf8().data());

    if(!successfulEncoding) {
        //            fLog.append("Cannot encode qr image");
    }

    int qrImageSize = qrEncode.m_nSymbleSize;
    int encodeImageSize = qrImageSize + (QR_MARGIN * 2);
    QImage encodeImage(encodeImageSize, encodeImageSize, QImage::Format_Mono);
    encodeImage.fill(1);

    for(int i = 0; i < qrImageSize; i++) {
        for(int j = 0; j < qrImageSize; j++) {
            if(qrEncode.m_byModuleData[i][j]) {
                encodeImage.setPixel(i + QR_MARGIN, j + QR_MARGIN, 0);
            }
        }
    }

    QMap<QString, QString> vars;
    vars["doc_title"] = tr("Sale") + " " + header["f_ordernumber"].toString();
    vars["date"] = QDate::fromString(header["f_datecash"].toString(), FORMAT_DATE_TO_STR_MYSQL).toString("dd/MM/yyyy");
    vars["delivery_date"] = QDate::fromString(header["f_datefor"].toString(), FORMAT_DATE_TO_STR_MYSQL).toString("dd/MM/yyyy");
    vars["buyer"] = QString("%1, %2 %3, %4 %5").arg(header["f_taxname"].toString(), tr("Taxpayer code"),
                    header["f_taxcode"].toString(),
                    tr("Address"), header["f_address"].toString());
    vars["saler"] = __c5config.fMainJson["firmfullinfo"].toString();
    vars["staff"] = header["f_staff"].toString();
    vars["total_amount"] = float_str(header["f_amounttotal"].toDouble(), 2);
    vars["goods_table"] = makeGoodsTable(body);
    vars["qr_image"] = "data:image/png;base64," + encodeString;
    html = applyTemplate(html, vars);
    QTextDocument doc;
    doc.setHtml(html);
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize::A4);
    printer.setFullPage(false);
    QPrintPreviewDialog preview(&printer);
    connect(&preview, &QPrintPreviewDialog::paintRequested,
    [&](QPrinter * p) { doc.print(p); });
    preview.exec();
    return true;
}
