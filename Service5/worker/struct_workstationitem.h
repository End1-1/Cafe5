#pragma once

#include <QStringList>
#include "c5jsonparser.h"
#include "struct_fiscal_machine.h"
#include "struct_parent.h"

struct WorkstationItem : public ParentItem {
    int id = 0;
    int type = 0;
    QString stationAccount;
    QString name;
    QString nameLower;

    virtual void validate() override
    {
        // Intentionally soft: missing keys mean misconfigured workstation (store/cashbox=0).
    }

    int cashboxId() const
    {
        return data.value("f_cashbox_id").toInt();
    }
    int defaultHallId() const
    {
        return data.value("f_default_hall_id").toInt();
    }
    const QString defaultHallName() const { return data.value("f_default_hall_name").toString(); }
    int defaultTableId() const
    {
        int id = data.value("f_default_table_id").toInt();
        return id > 0 ? id : 1;
    }
    int defaultStoreId() const { return data.value("f_default_store_id").toInt(); }
    const QString defaultStoreName() const { return data.value("f_default_store_name").toString(); }
    const QString defaultPrinter() const
    {
        QString printerName = receiptPrinter();
        if(printerName.isEmpty()) {
            printerName = data.value(QStringLiteral("f_default_printer")).toString().trimmed();
        }
        return printerName;
    }
    /** Shop bill printer from RWorkstationConfigShop::leReceiptPrinter (exact Windows name). */
    QString receiptPrinter() const
    {
        return data.value(QStringLiteral("receipt_printer")).toString().trimmed();
    }
    /** Waiter precheck: Windows printer name or print-server URL (RWorkstationConfigWaiter::lePrecheckPrinter). */
    QString precheckPrinter() const
    {
        return data.value(QStringLiteral("precheck_printer")).toString().trimmed();
    }
    static bool isPrintServerTarget(const QString &target)
    {
        return target.startsWith(QStringLiteral("http://"), Qt::CaseInsensitive)
               || target.startsWith(QStringLiteral("https://"), Qt::CaseInsensitive);
    }
    bool hasReceiptPrinter() const { return !receiptPrinter().isEmpty(); }
    /** Non-empty print_server → print via HTTP. */
    bool usePrintServer() const
    {
        return !printServer().isEmpty();
    }
    /** Print if print_server and/or receipt_printer is set (both empty → no print). */
    bool isReceiptPrintingConfigured() const
    {
        return usePrintServer() || hasReceiptPrinter();
    }
    const QString scalePattern() const { return data.value("f_scale_pattern").toString(); }
    const QString presentCardPattern() const
    {
        return data.value("f_present_card_pattern").toString();
    }
    const QString accumulateCardPatter() const
    {
        return data.value("f_accumulate_card_pattern").toString();
    }
    const QString discountCardPattern() const
    {
        return data.value("f_discount_card_pattern").toString();
    }
    const bool shopShowAll() const
    {
        if (!data.contains("f_showall")) {
            return true;
        }
        return data.value("f_showall").toBool();
    }
    const int fiscalMachineId() const { return data.value("f_fiscal_machine_id").toInt(); }
    const int quickDebtPartnerId() const { return data.value("f_quick_debt_partner_id").toInt(); }
    /** Common config: when true, reject sale if stock would go negative. Default false = allow minus. */
    bool dontAllowNegativeRemains() const
    {
        return data.value(QStringLiteral("dont_allow_negative_remains")).toBool(false);
    }
    /** Shop: ask which associate to assign the sale to before payment. Default false = current cashier. */
    bool assignSaleToAssociate() const
    {
        return data.value(QStringLiteral("assign_sale_to_associate")).toBool(false);
    }
    /** Waiter: write kitchen queue (o_goods_process) when customer notifications are enabled. */
    bool customerNotification() const
    {
        return data.value(QStringLiteral("customer_notification")).toBool(false);
    }
    const QString printServer() const
    {
        QString url = data.value(QStringLiteral("print_server")).toString().trimmed();
        return url;
    }
    int printPaperWidthMm() const { return data.value(QStringLiteral("print_paper_width")).toInt(); }

    bool isArcusConfigured() const
    {
        return arcusPort() > 0 && !arcusAddress().trimmed().isEmpty() && !arcusKey().trimmed().isEmpty();
    }

    int arcusPort() const { return data.value(QStringLiteral("arcus_port")).toInt(); }

    QString arcusAddress() const { return data.value(QStringLiteral("arcus_address")).toString(); }

    QString arcusKey() const { return data.value(QStringLiteral("arcus_key")).toString(); }
};

template<>
struct JsonParser<WorkstationItem> {
    static WorkstationItem fromJson(const QJsonObject &jo)
    {
        WorkstationItem hi;
        hi.id = jo["f_id"].toInt();
        hi.type = jo["f_type"].toInt();
        hi.stationAccount = jo["f_station_account"].toString();
        hi.name = jo["f_name"].toString();
        hi.parseData(jo, "f_config");
        hi.validate();

        QJsonArray jfm = jo.value("fiscal").toArray();
        for (int i = 0; i < jfm.size(); i++) {
            const QJsonObject &jf = jfm.at(i).toObject();
            FiscalMachine fm = JsonParser<FiscalMachine>::fromJson(jf);
            fiscalMachines.append(fm);
        }
        return hi;
    }
};

extern WorkstationItem mWorkStation;
