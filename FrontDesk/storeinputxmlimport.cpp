#include "storeinputxmlimport.h"
#include "appwebsocket.h"
#include "c5structtableview.h"
#include "c5user.h"
#include "ninterface.h"
#include "struct_goods_item.h"
#include "struct_partner.h"
#include <QCoreApplication>
#include <QEventLoop>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>
#include <QRegularExpression>
#include <QTimer>
#include <QUuid>
#include <QXmlStreamReader>

namespace {

QString localName(const QXmlStreamReader &xml)
{
    return xml.name().toString();
}

QString readElementText(QXmlStreamReader &xml)
{
    return xml.readElementText(QXmlStreamReader::SkipChildElements).trimmed();
}

double readDouble(const QString &text)
{
    QString normalized = text;
    normalized.replace(QLatin1Char(','), QLatin1Char('.'));
    return normalized.toDouble();
}

QDate parseSupplyDate(const QString &text)
{
    const QString datePart = text.left(10);
    const QDate date = QDate::fromString(datePart, Qt::ISODate);
    return date;
}

void parseTaxpayer(QXmlStreamReader &xml, StoreInputXmlInvoice &invoice)
{
    while (xml.readNextStartElement()) {
        const QString tag = localName(xml);
        if (tag == QLatin1String("TIN")) {
            invoice.tin = readElementText(xml);
        } else if (tag == QLatin1String("Name")) {
            invoice.supplierName = readElementText(xml);
        } else if (tag == QLatin1String("Address")) {
            invoice.supplierAddress = readElementText(xml);
        } else {
            xml.skipCurrentElement();
        }
    }
}

StoreInputXmlGoodLine parseGood(QXmlStreamReader &xml)
{
    StoreInputXmlGoodLine line;
    while (xml.readNextStartElement()) {
        const QString tag = localName(xml);
        if (tag == QLatin1String("Description")) {
            line.description = readElementText(xml);
        } else if (tag == QLatin1String("ClassifierCode")) {
            line.classifierCode = readElementText(xml);
        } else if (tag == QLatin1String("Unit")) {
            line.unit = readElementText(xml);
        } else if (tag == QLatin1String("Amount")) {
            line.qty = readDouble(readElementText(xml));
        } else if (tag == QLatin1String("PricePerUnit")) {
            line.pricePerUnit = readDouble(readElementText(xml));
        } else {
            xml.skipCurrentElement();
        }
    }
    return line;
}

void parseGoodsInfo(QXmlStreamReader &xml, StoreInputXmlInvoice &invoice, QString &error)
{
    while (xml.readNextStartElement()) {
        const QString tag = localName(xml);
        if (tag == QLatin1String("Good")) {
            const StoreInputXmlGoodLine line = parseGood(xml);
            if (line.description.isEmpty()) {
                error = QCoreApplication::translate("StoreInputXmlImport", "Goods line without description");
                return;
            }
            if (line.qty <= 0) {
                error = QCoreApplication::translate("StoreInputXmlImport", "Invalid quantity for goods: %1").arg(line.description);
                return;
            }
            if (line.pricePerUnit <= 0) {
                error = QCoreApplication::translate("StoreInputXmlImport", "Invalid price for goods: %1").arg(line.description);
                return;
            }
            invoice.goods.append(line);
        } else if (tag == QLatin1String("Total")) {
            while (xml.readNextStartElement()) {
                if (localName(xml) == QLatin1String("TotalPrice")) {
                    invoice.totalPrice = readDouble(readElementText(xml));
                } else {
                    xml.skipCurrentElement();
                }
            }
        } else {
            xml.skipCurrentElement();
        }
    }
}

bool parseSignableData(QXmlStreamReader &xml, StoreInputXmlInvoice &invoice, QString &error)
{
    invoice = StoreInputXmlInvoice();
    while (xml.readNextStartElement()) {
        const QString tag = localName(xml);
        if (tag == QLatin1String("GeneralInfo")) {
            while (xml.readNextStartElement()) {
                const QString child = localName(xml);
                if (child == QLatin1String("InvoiceNumber")) {
                    while (xml.readNextStartElement()) {
                        const QString numberTag = localName(xml);
                        if (numberTag == QLatin1String("Number")) {
                            invoice.invoiceNumber = readElementText(xml);
                        } else if (numberTag == QLatin1String("Series")) {
                            invoice.invoiceSeries = readElementText(xml);
                        } else {
                            xml.skipCurrentElement();
                        }
                    }
                } else if (child == QLatin1String("SupplyDate")) {
                    invoice.supplyDate = parseSupplyDate(readElementText(xml));
                } else if (child == QLatin1String("AdditionalData")) {
                    invoice.additionalData = readElementText(xml);
                } else {
                    xml.skipCurrentElement();
                }
            }
        } else if (tag == QLatin1String("SupplierInfo")) {
            while (xml.readNextStartElement()) {
                if (localName(xml) == QLatin1String("Taxpayer")) {
                    parseTaxpayer(xml, invoice);
                } else {
                    xml.skipCurrentElement();
                }
            }
        } else if (tag == QLatin1String("GoodsInfo")) {
            parseGoodsInfo(xml, invoice, error);
            if (!error.isEmpty()) {
                return false;
            }
        } else {
            xml.skipCurrentElement();
        }
    }

    invoice.docNumber = invoice.invoiceSeries + invoice.invoiceNumber;
    if (invoice.tin.isEmpty()) {
        error = QCoreApplication::translate("StoreInputXmlImport", "Supplier TIN is missing");
        return false;
    }
    if (!invoice.supplyDate.isValid()) {
        error = QCoreApplication::translate("StoreInputXmlImport", "Supply date is missing or invalid");
        return false;
    }
    if (invoice.goods.isEmpty()) {
        error = QCoreApplication::translate("StoreInputXmlImport", "Invoice has no goods lines");
        return false;
    }
    return true;
}

QJsonObject syncWebSocketQuery(const QJsonObject &request, QString &error)
{
    if (!AppWebSocket::instance || !AppWebSocket::instance->isConnected()) {
        error = QCoreApplication::translate("StoreInputXmlImport", "No connection to the server");
        return {};
    }

    QEventLoop loop;
    const QString requestId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QJsonObject req = request;
    req.insert(QStringLiteral("requestId"), requestId);
    QJsonObject response;

    const QMetaObject::Connection conn = QObject::connect(
        AppWebSocket::instance,
        &AppWebSocket::bMessageReceived,
        &loop,
        [&](const QJsonObject &jdoc) {
            if (jdoc.value(QStringLiteral("requestId")).toString() != requestId) {
                return;
            }
            response = jdoc;
            loop.quit();
        });

    if (!AppWebSocket::instance->sendMessage(req)) {
        error = QCoreApplication::translate("StoreInputXmlImport", "Failed to send request to server");
        QObject::disconnect(conn);
        return {};
    }

    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(60000);
    loop.exec();
    QObject::disconnect(conn);

    if (response.isEmpty()) {
        error = QCoreApplication::translate("StoreInputXmlImport", "Server response timeout");
        return {};
    }
    if (response.value(QStringLiteral("errorCode")).toInt() != 0) {
        error = response.value(QStringLiteral("errorMessage")).toString();
        if (error.isEmpty()) {
            error = QCoreApplication::translate("StoreInputXmlImport", "Server error");
        }
        return {};
    }
    return response;
}

} // namespace

QString StoreInputXmlImport::normalizeName(const QString &name)
{
    QString normalized = name.trimmed();
    normalized.replace(QRegularExpression(QStringLiteral("\\s+")), QStringLiteral(" "));
    return normalized;
}

QVector<StoreInputXmlInvoice> StoreInputXmlImport::parseFile(const QString &path, QString &error)
{
    QVector<StoreInputXmlInvoice> invoices;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error = QCoreApplication::translate("StoreInputXmlImport", "Cannot open file: %1").arg(path);
        return invoices;
    }

    QXmlStreamReader xml(&file);
    if (xml.hasError()) {
        error = QCoreApplication::translate("StoreInputXmlImport", "Invalid XML file");
        return invoices;
    }

    while (!xml.atEnd()) {
        xml.readNext();
        if (!xml.isStartElement()) {
            continue;
        }
        if (localName(xml) != QLatin1String("SignableData")) {
            continue;
        }

        StoreInputXmlInvoice invoice;
        if (!parseSignableData(xml, invoice, error)) {
            if (error.isEmpty()) {
                error = QCoreApplication::translate("StoreInputXmlImport", "Invalid invoice data");
            }
            invoices.clear();
            return invoices;
        }
        invoices.append(invoice);
    }

    if (xml.hasError()) {
        error = QCoreApplication::translate("StoreInputXmlImport", "XML parse error: %1").arg(xml.errorString());
        invoices.clear();
        return invoices;
    }
    if (invoices.isEmpty()) {
        error = QCoreApplication::translate("StoreInputXmlImport", "No invoices found in XML file");
    }
    return invoices;
}

bool StoreInputXmlImport::httpQuery(C5User *user,
                                    QObject *context,
                                    const QString &route,
                                    const QJsonObject &params,
                                    QJsonObject &response,
                                    QString &error)
{
    if (!user) {
        error = QCoreApplication::translate("StoreInputXmlImport", "User is not defined");
        return false;
    }
    if (!context) {
        error = QCoreApplication::translate("StoreInputXmlImport", "Context is not defined");
        return false;
    }

    QEventLoop loop;
    bool ok = false;
    NInterface::query(route,
                      user->mSessionKey,
                      context,
                      params,
                      [&](const QJsonObject &jdoc) {
                          response = jdoc;
                          ok = true;
                          loop.quit();
                      },
                      [&](const QJsonObject &jerr) {
                          error = jerr.value(QStringLiteral("message")).toString();
                          if (error.isEmpty()) {
                              error = jerr.value(QStringLiteral("errorMessage")).toString();
                          }
                          loop.quit();
                          return true;
                      },
                      true,
                      120000,
                      true);
    loop.exec();

    if (!ok) {
        if (error.isEmpty()) {
            error = QCoreApplication::translate("StoreInputXmlImport", "Request failed");
        }
        return false;
    }
    return true;
}

bool StoreInputXmlImport::findPartnerByTin(C5User *user,
                                           QObject *context,
                                           const QString &tin,
                                           QJsonObject &partner,
                                           QString &error)
{
    QJsonObject response;
    if (!httpQuery(user,
                   context,
                   QStringLiteral("/engine/v2/common/partners/get-by-tin"),
                   {{QStringLiteral("f_taxcode"), tin}},
                   response,
                   error)) {
        return false;
    }
    partner = response.value(QStringLiteral("partner")).toObject();
    if (partner.value(QStringLiteral("f_id")).toInt() <= 0) {
        error = QCoreApplication::translate("StoreInputXmlImport", "Partner not found");
        return false;
    }
    return true;
}

bool StoreInputXmlImport::createPartner(C5User *user,
                                        QObject *context,
                                        const QString &tin,
                                        const QString &taxName,
                                        const QString &address,
                                        QJsonObject &partner,
                                        QString &error)
{
    QJsonObject response;
    const QJsonObject params{
        {QStringLiteral("f_category"), 1},
        {QStringLiteral("f_state"), 1},
        {QStringLiteral("f_taxcode"), tin},
        {QStringLiteral("f_taxname"), taxName},
        {QStringLiteral("f_name"), taxName},
        {QStringLiteral("f_address"), address},
        {QStringLiteral("f_phone"), QString()},
        {QStringLiteral("f_contact"), QString()}};
    if (!httpQuery(user, context, QStringLiteral("/engine/v2/common/partners/save"), params, response, error)) {
        return false;
    }
    partner = response.value(QStringLiteral("partner")).toObject();
    if (partner.value(QStringLiteral("f_id")).toInt() <= 0) {
        error = QCoreApplication::translate("StoreInputXmlImport", "Failed to create partner");
        return false;
    }
    return true;
}

bool StoreInputXmlImport::updatePartner(C5User *user, QObject *context, const QJsonObject &partner, QString &error)
{
    QJsonObject response;
    if (!httpQuery(user, context, QStringLiteral("/engine/v2/common/partners/save"), partner, response, error)) {
        return false;
    }
    return response.value(QStringLiteral("partner")).toObject().value(QStringLiteral("f_id")).toInt() > 0;
}

bool StoreInputXmlImport::findGoodsByExactName(C5User *user, const QString &description, GoodsItem &goods, QString &error)
{
    Q_UNUSED(user)
    const QString needle = normalizeName(description);
    if (needle.isEmpty()) {
        error = QCoreApplication::translate("StoreInputXmlImport", "Empty goods description");
        return false;
    }

    const QString engine = SelectorName<GoodsItem>::value;
    const QJsonArray cached = C5StructTableView::cachedResults(engine);
    if (!cached.isEmpty()) {
        GoodsItem singleMatch;
        int matchCount = 0;
        for (const QJsonValue &value : cached) {
            const GoodsItem item = JsonParser<GoodsItem>::fromJson(value.toObject());
            if (normalizeName(item.name).compare(needle, Qt::CaseInsensitive) == 0) {
                singleMatch = item;
                ++matchCount;
            }
        }
        if (matchCount == 1) {
            goods = singleMatch;
            return true;
        }
    }

    QJsonObject request;
    request.insert(QStringLiteral("command"), engine);
    request.insert(QStringLiteral("lower_name"), needle.toLower());
    const QJsonObject response = syncWebSocketQuery(request, error);
    if (response.isEmpty()) {
        return false;
    }

    GoodsItem singleMatch;
    int matchCount = 0;
    const QJsonArray arr = response.value(QStringLiteral("result")).toArray();
    for (const QJsonValue &value : arr) {
        const GoodsItem item = JsonParser<GoodsItem>::fromJson(value.toObject());
        if (normalizeName(item.name).compare(needle, Qt::CaseInsensitive) == 0) {
            singleMatch = item;
            ++matchCount;
        }
    }
    if (matchCount == 1) {
        goods = singleMatch;
        return true;
    }
    error.clear();
    return false;
}

bool StoreInputXmlImport::findGoodsById(int goodsId, GoodsItem &goods)
{
    if (goodsId <= 0) {
        return false;
    }

    const QString engine = SelectorName<GoodsItem>::value;
    const QJsonArray cached = C5StructTableView::cachedResults(engine);
    for (const QJsonValue &value : cached) {
        const GoodsItem item = JsonParser<GoodsItem>::fromJson(value.toObject());
        if (item.id == goodsId) {
            goods = item;
            return true;
        }
    }
    return false;
}

bool StoreInputXmlImport::renameGoods(C5User *user, QObject *context, int goodsId, const QString &name, QString &error)
{
    QJsonObject response;
    return httpQuery(user,
                     context,
                     QStringLiteral("/engine/v2/common/goods/rename"),
                     {{QStringLiteral("f_id"), goodsId}, {QStringLiteral("f_name"), name}},
                     response,
                     error);
}
