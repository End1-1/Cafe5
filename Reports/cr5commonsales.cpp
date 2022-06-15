#include "cr5commonsales.h"
#include "cr5commonsalesfilter.h"
#include "c5tablemodel.h"
#include "c5waiterorder.h"
#include "c5mainwindow.h"
#include "c5salefromstoreorder.h"
#include "c5waiterorderdoc.h"
#include "dlgexportsaletoasoptions.h"
#include <QDir>
#include <QFile>
#include <QDesktopServices>
#include <QUrl>

CR5CommonSales::CR5CommonSales(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/graph.png";
    fLabel = tr("Sales by tickets");
    fSimpleQuery = false;

    fMainTable = "o_header oh";
    fLeftJoinTables << "left join h_halls hl on hl.f_id=oh.f_hall [hl]"
                    << "left join h_tables ht on ht.f_id=oh.f_table [ht]"
                    << "left join o_header_flags ohf on ohf.f_id=oh.f_id [ohf]"
                    << "left join s_user w on w.f_id=oh.f_staff [w]"
                    << "left join o_state os on os.f_id=oh.f_state [os]"
                    << "left join o_tax ot on ot.f_id=oh.f_id [ot]"
                    << "left join c_partners cpb on cpb.f_id=oh.f_partner [cpb]"
                       ;

    fColumnsFields << "concat(oh.f_prefix, oh.f_hallid) as f_prefix" 
                   << "oh.f_id"
                   << "os.f_name as f_statename"
                   << "oh.f_datecash"
                   << "dayofweek(oh.f_datecash) as f_dayofweek"
                   << "hl.f_name as f_hallname"
                   << "ht.f_name as f_tablename"
                   << "concat(w.f_last, ' ', w.f_first) as f_staff"
                   << "oh.f_dateopen"
                   << "oh.f_timeopen"
                   << "oh.f_dateclose"
                   << "oh.f_timeclose"
                   << "cpb.f_taxname as f_buyer"
                   << "cpb.f_taxcode as f_buyertaxcode"
                   << "count(oh.f_id) as f_count"
                   << "ot.f_receiptnumber"
                   << "sum(oh.f_amounttotal) as f_amounttotal"
                   << "sum(oh.f_amountcash) as f_amountcash"
                   << "sum(oh.f_amountcard) as f_amountcard"
                   << "sum(oh.f_amountbank) as f_amountbank"
                   << "sum(oh.f_amountother) as f_amountother"
                   << "sum(oh.f_amountidram) as f_amountidram"
                   << "sum(oh.f_amountpayx) as f_amountpayx"
                   << "oh.f_amountservice"
                   << "oh.f_amountdiscount"
                   << "oh.f_comment"
                      ;

    fColumnsGroup << "concat(oh.f_prefix, oh.f_hallid) as f_prefix"
                   << "oh.f_id"
                   << "oh.f_datecash"
                   << "dayofweek(oh.f_datecash) as f_dayofweek"
                   << "os.f_name as f_statename"
                   << "hl.f_name as f_hallname"
                   << "ht.f_name as f_tablename"
                   << "ot.f_receiptnumber"
                   << "oh.f_dateopen"
                   << "oh.f_timeopen"
                   << "oh.f_dateclose"
                   << "oh.f_timeclose"
                   << "oh.f_amountservice"
                   << "oh.f_amountdiscount"
                   << "cpb.f_taxname as f_buyer"
                   << "cpb.f_taxcode as f_buyertaxcode"
                   << "concat(w.f_last, ' ', w.f_first) as f_staff"
                   << "oh.f_comment"
                      ;

    fColumnsSum << "f_amounttotal"
                << "f_amountcash"
                << "f_amountcard"
                << "f_amountbank"
                << "f_amountother"
                << "f_amountidram"
                << "f_amountpayx"
                << "f_amountservice"
                << "f_amountdiscount"
                << "f_count"
                      ;

    fColumnsOrder << "oh.f_datecash"
                  << "oh.f_timeclose";

    fTranslation["f_prefix"] = tr("Head");
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_staff"] = tr("Staff");
    fTranslation["f_statename"] = tr("State");
    fTranslation["f_dateopen"] = tr("Open date");
    fTranslation["f_dateclose"] = tr("Close date");
    fTranslation["f_timeopen"] = tr("Open time");
    fTranslation["f_timeclose"] = tr("Close time");
    fTranslation["f_datecash"] = tr("Date, cash");
    fTranslation["f_dayofweek"] = tr("Day of week");
    fTranslation["f_hallname"] = tr("Hall");
    fTranslation["f_buyer"] = tr("Buyer");
    fTranslation["f_receiptnumber"] = tr("Tax");
    fTranslation["f_buyertaxcode"] = tr("Buyer taxcode");
    if (__c5config.frontDeskMode() == FRONTDESK_SHOP) {
        fTranslation["f_tablename"] = tr("Cash");
    } else {
        fTranslation["f_tablename"] = tr("Table");
    }
    fTranslation["f_amounttotal"] = tr("Total");
    fTranslation["f_amountcash"] = tr("Cash");
    fTranslation["f_amountcard"] = tr("Card");
    fTranslation["f_amountbank"] = tr("Bank");
    fTranslation["f_amountother"] = tr("Other");
    fTranslation["f_amountidram"] = tr("Idram");
    fTranslation["f_amountpayx"] = tr("PayX");
    fTranslation["f_amountservice"] = tr("Service");
    fTranslation["f_amountdiscount"] = tr("Discount");
    fTranslation["f_comment"] = tr("Comment");
    fTranslation["f_count"] = tr("Count");

    fColumnsVisible["concat(oh.f_prefix, oh.f_hallid) as f_prefix"] = true;
    fColumnsVisible["oh.f_id"] = true;
    fColumnsVisible["os.f_name as f_statename"] = true;
    fColumnsVisible["oh.f_datecash"] = true;
    fColumnsVisible["dayofweek(oh.f_datecash) as f_dayofweek"] = false;
    fColumnsVisible["hl.f_name as f_hallname"] = true;
    fColumnsVisible["ht.f_name as f_tablename"] = true;
    fColumnsVisible["oh.f_dateopen"] = false;
    fColumnsVisible["oh.f_dateclose"] = false;
    fColumnsVisible["oh.f_timeopen"] = false;
    fColumnsVisible["oh.f_timeclose"] = false;
    fColumnsVisible["cpb.f_taxname as f_buyer"] = false;
    fColumnsVisible["cpb.f_taxcode as f_buyertaxcode"] = false;
    fColumnsVisible["concat(w.f_last, ' ', w.f_first) as f_staff"] = false;
    fColumnsVisible["sum(oh.f_amounttotal) as f_amounttotal"] = true;
    fColumnsVisible["sum(oh.f_amountcash) as f_amountcash"] = true;
    fColumnsVisible["sum(oh.f_amountcard) as f_amountcard"] = true;
    fColumnsVisible["sum(oh.f_amountbank) as f_amountbank"] = true;
    fColumnsVisible["sum(oh.f_amountother) as f_amountother"] = true;
    fColumnsVisible["sum(oh.f_amountidram) as f_amountidram"] = true;
    fColumnsVisible["sum(oh.f_amountpayx) as f_amountpayx"] = true;
    fColumnsVisible["oh.f_amountservice"] = false;
    fColumnsVisible["oh.f_amountdiscount"] = false;
    fColumnsVisible["oh.f_comment"] = false;
    fColumnsVisible["count(oh.f_id) as f_count"] = false;
    fColumnsVisible["ot.f_receiptnumber"] = false;

    restoreColumnsVisibility();

    fFilterWidget = new CR5CommonSalesFilter(fDBParams);
    fFilter = static_cast<CR5CommonSalesFilter*>(fFilterWidget);
}

QToolBar *CR5CommonSales::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbEdit
            << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/AS.png"), tr("Export to AS"), this, SLOT(exportToAS()));
        fToolBar->addAction(QIcon(":/AS.png"), tr("Create store output in AS"), this, SLOT(createStoreOutputAS()));
    }
    return fToolBar;
}

void CR5CommonSales::editRow(int columnWidthId)
{
    Q_UNUSED(columnWidthId);
    C5Grid::editRow(1);
}

void CR5CommonSales::restoreColumnsWidths()
{
    C5Grid::restoreColumnsWidths();
    if (fColumnsVisible["oh.f_id"]) {
        fTableView->setColumnWidth(fModel->fColumnNameIndex["f_id"], 0);
    }
}

bool CR5CommonSales::tblDoubleClicked(int row, int column, const QList<QVariant> &values)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (!fColumnsVisible["oh.f_id"]) {
        C5Message::info(tr("Column 'Header' must be checked in filter"));
        return true;
    }
    if (values.count() == 0) {
        C5Message::info(tr("Nothing selected"));
        return true;
    }
    C5Database db(fDBParams);
    db[":f_id"] = values.at(fModel->fColumnNameIndex["f_id"]).toString();
    db.exec("select f_source from o_header where f_id=:f_id");
    if (db.nextRow()) {
        switch (abs(db.getInt(0))) {
        case 1: {
            C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>(fDBParams);
            wo->setOrder(values.at(fModel->fColumnNameIndex["f_id"]).toString());
            break;
        }
        case 2: {
            C5SaleFromStoreOrder::openOrder(fDBParams, values.at(fModel->fColumnNameIndex["f_id"]).toString());
            break;
        }
        default: {
            C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>(fDBParams);
            wo->setOrder(values.at(fModel->fColumnNameIndex["f_id"]).toString());
            break;
        }
        }
    } else {
        C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>(fDBParams);
        wo->setOrder(values.at(fModel->fColumnNameIndex["f_id"]).toString());
    }
    return true;
}

void CR5CommonSales::transferToRoom()
{
    if (!fColumnsVisible["oh.f_id"]) {
        C5Message::info(tr("Column 'Header' must be checked in filter"));
        return;
    }
    QModelIndexList ml = this->fTableView->selectionModel()->selectedIndexes();
    QSet<int> rows;
    foreach (QModelIndex m, ml) {
        rows << m.row();
    }
    C5Database db1(fDBParams);;
    foreach (int r, rows) {
        QString err;
        C5WaiterOrderDoc w(fModel->data(r, fModel->fColumnNameIndex["f_id"], Qt::EditRole).toString(), db1);
        w.transferToHotel(db1, err);
        if (!err.isEmpty()) {
            C5Message::error(err + "<br>" + fModel->data(r, 0, Qt::EditRole).toString());
            return;
        }
    }
    C5Message::info(tr("Done"));
}

void CR5CommonSales::exportToAS()
{
    if (!fColumnsVisible["oh.f_id"]) {
        C5Message::info(tr("Column 'Header' must be checked in filter"));
        return;
    }
    QStringList idlist;
    for (int i = 0; i < fModel->rowCount(); i++) {
        idlist.append(fModel->data(i, fModel->fColumnNameIndex["f_id"], Qt::EditRole).toString());
    }
    QString partnercode, storecode, servicecode, srvinacc, srvoutacc, iteminacc, itemoutacc, bankacc, vat, simpleItem;
    bool simple;
    int option = DlgExportSaleToAsOptions::getOption(fDBParams, partnercode, storecode, servicecode, srvinacc, srvoutacc,
                                                     iteminacc, itemoutacc, bankacc, vat, simple, simpleItem);
    if (option == 0) {
        return;
    }
    switch (option) {
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    }

    C5Database dbas("QODBC3");
    dbas.setDatabase("", __c5config.getValue(param_as_connection_string), "", "");
    if (!dbas.open()) {
        C5Message::error(dbas.fLastError);
    }
    QMap<QString, QMap<QString, QVariant> > goodsMap;
    QMap<QString, QString> unitMap;
    QMap<QString, QVariant> partnerMap;
    QMap<QString, QVariant> serviceMap;
    QMap<QString, QVariant> simpleMap;
    dbas.exec("select fMTCODE, fMTID, fCAPTION, fUNIT from MATERIALS");
    while (dbas.nextRow()) {
        QMap<QString, QVariant> temp;
        dbas.rowToMap(temp);
        goodsMap[dbas.getString(0)] = temp;
    }
    dbas.exec("select fCODE, fBRIEF from QNTUNIT");
    while (dbas.nextRow()) {
        unitMap[dbas.getString(0)] = dbas.getString(1);
    }
    dbas[":fPARTCODE"] = partnercode;
    dbas.exec("select * from PARTNERS where fPARTCODE=:fPARTCODE");
    if (dbas.nextRow()) {
        dbas.rowToMap(partnerMap);
    } else {
        C5Message::error(tr("Invalid partner code"));
        return;
    }
    dbas[":fSERVCODE"] = servicecode;
    dbas.exec("select * FROm SERVICES where fSERVCODE=:fSERVCODE");
    if (dbas.nextRow()) {
        dbas.rowToMap(serviceMap);
    } else {
        C5Message::error(tr("Invalid service code"));
        return;
    }
    if (simple) {
        dbas[":fSIMPLEITEM"] = simpleItem;
        dbas.exec("select * FROM SERVICES where fSERVCODE=:fSIMPLEITEM");
        if (dbas.nextRow()) {
            dbas.rowToMap(simpleMap);
        } else {
            C5Message::error(tr("Invalid simple item code"));
            return;
        }
    }

    QStringList codeErrors;
    C5Database db(fDBParams);
    if (simple) {
        //TODO: check for single date, date range in simple mode is not allowed
        double total = 0, card = 0, service = 0;
        QDate date;
        for (const QString id: idlist) {
            db[":f_id"] = id;
            db.exec("select f_receiptnumber from o_tax where f_id=:f_id");
            int rn = 0;
            if (db.nextRow()) {
                rn = db.getInt(0);
            }
            switch (option) {
            case 1:
                if (rn == 0) {
                    continue;
                }
                break;
            case 2:
                if (rn > 0) {
                    continue;
                }
                break;
            }
            db[":f_id"] = id;
            db.exec("select * from o_header where f_id=:f_id");
            db.nextRow();
            total += db.getDouble("f_amountcash");
            card += db.getDouble("f_amountcard") + db.getDouble("f_amountidram");
            service += db.getDouble("f_amountservice");
            date= db.getDate("f_datecash");
        }
        QString docid = db.uuid();
        dbas.startTransaction();
        dbas[":fISN"] = docid;
        dbas[":fDOCTYPE"] = 20;
        dbas[":fDOCSTATE"] = 1;
        dbas[":fORDERNUM"] = "";
        dbas[":fDOCNUM"] = "";
        dbas[":fCUR"] = "AMD";
        dbas[":fSUMM"] = total + card;
        dbas[":fCOMMENT"] = QString("%1 %2").arg(tr("Revenue"), date.toString(FORMAT_DATE_TO_STR));
        dbas[":fBODY"] = QString("\r\nPREPAYMENTACC:5231\r\nVATACC:5243\r\nSUMMVAT:%2\r\nBUYERACC:2211\r\nBUYCHACCPOST:Գլխավոր հաշվապահ \r\nMAXROWID:%1\r\n")
                .arg(2)
                .arg(vat.toDouble() > 0.001 ? (vat.toDouble() / 100) * (total + card) : 0);
        dbas[":fPARTNAME"] = partnerMap["ffullcaption"]; // set to kamar
        dbas[":fUSERID"] = 0;
        dbas[":fPARTID"] = partnerMap["fpartid"];
        dbas[":fCRPARTID"] = -1;
        dbas[":fMTID"] = -1;
        dbas[":fINVN"] = "";
        dbas[":fENTRYSTATE"] = 0;
        dbas[":fEMPLIDRESPIN"] = -1;
        dbas[":fEMPLIDRESPOUT"] = -1;
        dbas[":fVATTYPE"] = "5";
        dbas[":fSPEC"] = "                    00"; // <--- Tax receipt id
        if (card > 0.001) {
            dbas[":fBODY"] = dbas[":fBODY"].toString() +
                    QString("BANKACQUIRINGACCOUNT:%1\r\nSUMMPLCARD:%2\r\n")
                    .arg(bankacc)
                    .arg(float_str(card, 2).replace(",", "").replace(" ", ""));
        }
        if (!dbas.exec("insert into DOCUMENTS ("
                  "fISN, fDOCTYPE, fDOCSTATE, fDATE, fORDERNUM, fDOCNUM, fCUR, fSUMM, fCOMMENT, fBODY, fPARTNAME, "
                  "fUSERID, fPARTID, fCRPARTID, fMTID, fEXPTYPE, fINVN, fENTRYSTATE, "
                  "fEMPLIDRESPIN, fEMPLIDRESPOUT, fVATTYPE, fSPEC, fCHANGEDATE,fEXTBODY,fETLSTATE) VALUES ("
                  ":fISN, :fDOCTYPE, :fDOCSTATE, '" + date.toString("yyyy-MM-dd") + "', "
                  ":fORDERNUM, :fDOCNUM, :fCUR, :fSUMM, :fCOMMENT, :fBODY, :fPARTNAME, "
                  ":fUSERID, :fPARTID, :fCRPARTID, :fMTID, '', :fINVN, :fENTRYSTATE, "
                       ":fEMPLIDRESPIN, :fEMPLIDRESPOUT, :fVATTYPE, :fSPEC, current_timestamp,'', '')")) {
            dbas.rollback();
            C5Message::error(dbas.fLastError);
            return;
        }
        dbas[":fISN"] = docid;
        dbas[":fROWNUM"] = 0;
        dbas[":fITEMTYPE"] = 2;
        dbas[":fITEMID"] = simpleMap["fservid"];
        dbas[":fITEMNAME"] = simpleMap["fcaption"];
        dbas[":fUNITBRIEF"] = unitMap[simpleMap["funit"].toString()];
        dbas[":fSTORAGE"] = storecode;
        dbas[":fQUANTITY"] = 1;
        dbas[":fINITPRICE"] = (total + card) - service;
        dbas[":fDISCOUNT"] = 0;
        dbas[":fPRICE"] = (total + card) - service;
        dbas[":fSUMMA"] = (total + card) - service;
        dbas[":fSUMMA1"] = 0;
        dbas[":fENVFEEPERCENT"] = 0;
        dbas[":fENVFEESUMMA"] = 0;
        dbas[":fVAT"] = 1;
        dbas[":fEXPMETHOD"] = 1;
        dbas[":fACCEXPENSE"] = iteminacc;
        dbas[":fACCINCOME"] = itemoutacc;
        dbas[":fPARTYMETHOD"] = 0;
        dbas[":fROWID"] = 0;
        if (!dbas.insert("MTINVOICELIST", false)) {
            dbas.rollback();
            C5Message::error(dbas.fLastError);
            return;
        }
        if (service > 0.001) {
            dbas[":fISN"] = docid;
            dbas[":fROWNUM"] = 1;
            dbas[":fITEMTYPE"] = 2;
            dbas[":fITEMID"] = serviceMap["fservid"];
            dbas[":fITEMNAME"] = serviceMap["fcaption"];
            dbas[":fUNITBRIEF"] = unitMap[serviceMap["funit"].toString()];
            dbas[":fSTORAGE"] = storecode;
            dbas[":fQUANTITY"] = 1;
            dbas[":fINITPRICE"] = service;
            dbas[":fDISCOUNT"] = 0;
            dbas[":fPRICE"] = service;
            dbas[":fSUMMA"] = service;
            dbas[":fSUMMA1"] = 0;
            dbas[":fENVFEEPERCENT"] = 0;
            dbas[":fENVFEESUMMA"] = 0;
            dbas[":fVAT"] = 1;
            dbas[":fEXPMETHOD"] = 1;
            dbas[":fACCEXPENSE"] = srvinacc;
            dbas[":fACCINCOME"] = srvoutacc;
            dbas[":fPARTYMETHOD"] = 0;
            dbas[":fROWID"] = 1;
            if (!dbas.insert("MTINVOICELIST", false)) {
                dbas.rollback();
                C5Message::error(dbas.fLastError);
                return;
            }
        }
        dbas.commit();
        C5Message::info(tr("Done"));
        return;
    }

    for (const QString id: idlist) {
        QMap<QString, QVariant> oh;
        QMap<QString, QVariant> otax;
        QList<QMap<QString, QVariant> > ob;
        db[":f_id"] = id;
        db.exec("select f_receiptnumber from o_tax where f_id=:f_id");
        int rn = 0;
        if (db.nextRow()) {
            rn = db.getInt(0);
        }
        switch (option) {
        case 1:
            if (rn == 0) {
                continue;
            }
            break;
        case 2:
            if (rn > 0) {
                continue;
            }
            break;
        }

        db[":f_id"] = id;
        db.exec("select * from o_header where f_id=:f_id");
        db.nextRow();
        db.rowToMap(oh);
        db[":f_id"] = id;
        db.exec("select * from o_tax where f_id=:f_id");
        if (db.nextRow()) {
            db.rowToMap(otax);
        }
        db[":f_header"] = id;
        db[":f_state"] = DISH_STATE_OK;
        db.exec("select * from o_body where f_header=:f_header and f_state=:f_state");
        while (db.nextRow()) {
            QMap<QString, QVariant> temp;
            db.rowToMap(temp);
            ob.append(temp);
        }
        QString docid = db.uuid();
        dbas.startTransaction();
        dbas[":fISN"] = docid;
        dbas[":fDOCTYPE"] = 20;
        dbas[":fDOCSTATE"] = 1;
        dbas[":fORDERNUM"] = "";
        dbas[":fDOCNUM"] = oh["f_prefix"].toString() + oh["f_hallid"].toString();
        dbas[":fCUR"] = "AMD";
        dbas[":fSUMM"] = oh["f_amounttotal"];
        dbas[":fCOMMENT"] = "";
        dbas[":fBODY"] = QString("\r\nPREPAYMENTACC:5231\r\nVATACC:5243\r\nSUMMVAT:%2\r\nBUYERACC:2211\r\nBUYCHACCPOST:Գլխավոր հաշվապահ \r\nMAXROWID:%1\r\n")
                .arg(ob.count()) //<---- Errror
                .arg(vat.toDouble() > 0.001 ? (vat.toDouble() / 100) * oh["f_amounttotal"].toDouble() : 0);
        dbas[":fPARTNAME"] = partnerMap["ffullcaption"]; // set to kamar
        dbas[":fUSERID"] = 0;
        dbas[":fPARTID"] = partnerMap["fpartid"];
        dbas[":fCRPARTID"] = -1;
        dbas[":fMTID"] = -1;
        dbas[":fINVN"] = "";
        dbas[":fENTRYSTATE"] = 0;
        dbas[":fEMPLIDRESPIN"] = -1;
        dbas[":fEMPLIDRESPOUT"] = -1;
        dbas[":fVATTYPE"] = "5";
        dbas[":fSPEC"] = otax["f_receiptnumber"].toString().isEmpty() ? "                    00" : otax["f_receiptnumber"];
        if (oh["f_amountcard"].toDouble() > 0.001) {
            dbas[":fBODY"] = dbas[":fBODY"].toString() +
                    QString("BANKACQUIRINGACCOUNT:%1\r\nSUMMPLCARD:%2\r\n")
                    .arg(bankacc)
                    .arg(float_str(oh["f_amountcard"].toDouble(), 2).replace(",", "").replace(" ", ""));
        }
        if (!dbas.exec("insert into DOCUMENTS ("
                  "fISN, fDOCTYPE, fDOCSTATE, fDATE, fORDERNUM, fDOCNUM, fCUR, fSUMM, fCOMMENT, fBODY, fPARTNAME, "
                  "fUSERID, fPARTID, fCRPARTID, fMTID, fEXPTYPE, fINVN, fENTRYSTATE, "
                  "fEMPLIDRESPIN, fEMPLIDRESPOUT, fVATTYPE, fSPEC, fCHANGEDATE,fEXTBODY,fETLSTATE) VALUES ("
                  ":fISN, :fDOCTYPE, :fDOCSTATE, '" + oh["f_datecash"].toDate().toString("yyyy-MM-dd") + "', "
                  ":fORDERNUM, :fDOCNUM, :fCUR, :fSUMM, :fCOMMENT, :fBODY, :fPARTNAME, "
                  ":fUSERID, :fPARTID, :fCRPARTID, :fMTID, '', :fINVN, :fENTRYSTATE, "
                       ":fEMPLIDRESPIN, :fEMPLIDRESPOUT, :fVATTYPE, :fSPEC, current_timestamp,'', '')")) {
            dbas.rollback();
            C5Message::error(dbas.fLastError);
            return;
        }

        int rownum = 0;
        for (int ib = 0; ib < ob.count(); ib++) {
            QMap<QString, QVariant> &m = ob[ib];
            if (!goodsMap.contains(m["f_dish"].toString())) {
                if (!codeErrors.contains(m["f_dish"].toString())) {
                    codeErrors.append(QString("%1 - %2").arg(oh["f_prefix"].toString() + oh["f_hallid"].toString()).arg(m["f_dish"].toString()));
                }
                continue;
            }
            dbas[":fISN"] = docid;
            dbas[":fROWNUM"] = rownum;
            dbas[":fITEMTYPE"] = 1;
            dbas[":fITEMID"] = goodsMap[m["f_dish"].toString()]["fmtid"];
            dbas[":fITEMNAME"] = goodsMap[m["f_dish"].toString()]["fcaption"];
            dbas[":fUNITBRIEF"] = unitMap[goodsMap[m["f_dish"].toString()]["funit"].toString()];
            dbas[":fSTORAGE"] = storecode;
            dbas[":fQUANTITY"] = m["f_qty1"];
            dbas[":fINITPRICE"] = m["f_price"];
            dbas[":fDISCOUNT"] = 0;
            dbas[":fPRICE"] = m["f_price"];
            dbas[":fSUMMA"] = m["f_total"];
            dbas[":fSUMMA1"] = 0;
            dbas[":fENVFEEPERCENT"] = 0;
            dbas[":fENVFEESUMMA"] = 0;
            dbas[":fVAT"] = 1;
            dbas[":fEXPMETHOD"] = 1;
            dbas[":fACCEXPENSE"] = iteminacc;
            dbas[":fACCINCOME"] = itemoutacc;
            dbas[":fPARTYMETHOD"] = 0;
            dbas[":fROWID"] = rownum;
            if (!dbas.insert("MTINVOICELIST", false)) {
                dbas.rollback();
                C5Message::error(dbas.fLastError);
                return;
            }
            rownum++;
        }
        if (oh["f_amountservice"].toDouble() > 0.001) {
            dbas[":fISN"] = docid;
            dbas[":fROWNUM"] = rownum;
            dbas[":fITEMTYPE"] = 2;
            dbas[":fITEMID"] = serviceMap["fservid"];
            dbas[":fITEMNAME"] = serviceMap["fcaption"];
            dbas[":fUNITBRIEF"] = unitMap[serviceMap["funit"].toString()];
            dbas[":fSTORAGE"] = storecode;
            dbas[":fQUANTITY"] = 1;
            dbas[":fINITPRICE"] = oh["f_amountservice"];
            dbas[":fDISCOUNT"] = 0;
            dbas[":fPRICE"] = oh["f_amountservice"];
            dbas[":fSUMMA"] = oh["f_amountservice"];
            dbas[":fSUMMA1"] = 0;
            dbas[":fENVFEEPERCENT"] = 0;
            dbas[":fENVFEESUMMA"] = 0;
            dbas[":fVAT"] = 1;
            dbas[":fEXPMETHOD"] = 1;
            dbas[":fACCEXPENSE"] = srvinacc;
            dbas[":fACCINCOME"] = srvoutacc;
            dbas[":fPARTYMETHOD"] = 0;
            dbas[":fROWID"] = rownum;
            if (!dbas.insert("MTINVOICELIST", false)) {
                dbas.rollback();
                C5Message::error(dbas.fLastError);
                return;
            }
            rownum++;
        }
        dbas.commit();
    }

    if (!codeErrors.isEmpty()) {
        QDir d;
        QFile f(d.tempPath() + "/err.txt");
        if (f.open(QIODevice::WriteOnly)) {
            QString ba = codeErrors.join("\r\n");
            f.write(ba.toUtf8());
            f.close();
            QDesktopServices::openUrl(QUrl(d.tempPath() + "/err.txt"));
        }
    }


    C5Message::info(tr("Done"));
}

void CR5CommonSales::createStoreOutputAS()
{
    C5Database db(fDBParams);
    db[":date1"] = fFilter->date1();
    db[":date2"] = fFilter->date2();
    db[":ostate"] = ORDER_STATE_CLOSE;
    QString sql = "select ob.f_dish ,sum(ob.f_qty1) as f_qty "
            "from o_body ob "
            "left join o_header oh on oh.f_id=ob.f_header  "
            "where oh.f_datecash between :date1 and :date2 and oh.f_state=:ostate and ob.f_state in (1, 3) %1 "
            "group by ob.f_dish ";
    if (fFilter->complimentary()) {
        sql.replace("%1", " and oh.f_amounttotal=0 ");
    } else if (fFilter->notComplimentary()) {
        sql.replace("%1", " and oh.f_amounttotal<>0 ");
    } else {
        sql.replace("%1", "");
    }
    db.exec(sql);
    QMap<QString, double> dishesList;
    while (db.nextRow()) {
        dishesList[db.getString("f_dish")] = db.getDouble("f_qty");
    }

    C5Database dbas("QODBC3");
    dbas.setDatabase("", __c5config.getValue(param_as_connection_string), "", "");
    if (!dbas.open()) {
        C5Message::error(dbas.fLastError);
    }
    dbas.exec("select fMTCODE, fMTID, fCAPTION, fUNIT from MATERIALS");
    QMap<QString, QMap<QString, QVariant> > goodsMap;
    while (dbas.nextRow()) {
        QMap<QString, QVariant> temp;
        dbas.rowToMap(temp);
        goodsMap[dbas.getString(0)] = temp;
    }

    QStringList nocalc;
    QMap<QString, QMap<QString, double> > calc;
    for (const QString &s: dishesList.keys()) {
        dbas[":fMTDESTID"] = goodsMap[s]["fmtid"].toString();
        dbas[":fSALED"] = dishesList[s];
        dbas.exec("select fMTSOURCEID,fQTY*:fSALED from MTCOMPLECTS where fMTDESTID=:fMTDESTID");
        bool hascalc = false;
        while (dbas.nextRow()) {
            hascalc = true;
            calc[s][dbas.getString(0)] = dbas.getDouble(1);
        }
        if (!hascalc) {
            nocalc.append(s);
        }
    }

    int maxrowid = 0;
    for (const QString &s: calc.keys()) {
        maxrowid += calc[s].count();
    }
    dbas.startTransaction();
    QString docid = db.uuid();
    dbas.startTransaction();
    dbas[":fISN"] = docid;
    dbas[":fDOCTYPE"] = 7;
    dbas[":fDOCSTATE"] = 1;
    dbas[":fORDERNUM"] = "";
    dbas[":fDOCNUM"] = "";
    dbas[":fCUR"] = "AMD";
    dbas[":fSUMM"] = 0;
    dbas[":fCOMMENT"] = QString("%1")
            .arg(QString("%1 %2 - %3")
                 .arg(tr("Output"))
                 .arg(fFilter->date1().toString(FORMAT_DATE_TO_STR))
                 .arg(fFilter->date2().toString(FORMAT_DATE_TO_STR)));
    dbas[":fBODY"] = QString("\r\nPREPAYMENTACC:5231\r\nVATACC:5243\r\nSUMMVAT:0\r\nBUYERACC:2211\r\nBUYCHACCPOST:Գլխավոր հաշվապահ \r\nMAXROWID:%1\r\n")
            .arg(maxrowid);
    dbas[":fPARTNAME"] = ""; // set to kamar
    dbas[":fUSERID"] = 0;
    dbas[":fPARTID"] = -1;
    dbas[":fCRPARTID"] = -1;
    dbas[":fMTID"] = -1;
    dbas[":fINVN"] = "";
    dbas[":fENTRYSTATE"] = 0;
    dbas[":fEMPLIDRESPIN"] = -1;
    dbas[":fEMPLIDRESPOUT"] = -1;
    dbas[":fVATTYPE"] = "5";
    dbas[":fSPEC"] = "                    00"; // <--- Tax receipt id
    if (!dbas.exec("insert into DOCUMENTS ("
              "fISN, fDOCTYPE, fDOCSTATE, fDATE, fORDERNUM, fDOCNUM, fCUR, fSUMM, fCOMMENT, fBODY, fPARTNAME, "
              "fUSERID, fPARTID, fCRPARTID, fMTID, fEXPTYPE, fINVN, fENTRYSTATE, "
              "fEMPLIDRESPIN, fEMPLIDRESPOUT, fVATTYPE, fSPEC, fCHANGEDATE,fEXTBODY,fETLSTATE) VALUES ("
              ":fISN, :fDOCTYPE, :fDOCSTATE, '" + fFilter->date2().toString("yyyy-MM-dd") + "', "
              ":fORDERNUM, :fDOCNUM, :fCUR, :fSUMM, :fCOMMENT, :fBODY, :fPARTNAME, "
              ":fUSERID, :fPARTID, :fCRPARTID, :fMTID, '', :fINVN, :fENTRYSTATE, "
                   ":fEMPLIDRESPIN, :fEMPLIDRESPOUT, :fVATTYPE, :fSPEC, current_timestamp,'', '')")) {
        dbas.rollback();
        C5Message::error(dbas.fLastError);
        return;
    }

    int rownum = 0;
    for (const QString &s: calc.keys()) {
        for (const QString &m: calc[s].keys()) {
            dbas[":fISN"] = docid;
            dbas[":fROWNUM"] = rownum;
            dbas[":fMTID"] = m;
            dbas[":fQUANTITY"] = calc[s][m];
            dbas[":fSUMMA"] = 0;
            dbas[":fEXPMETHOD"] = 1;
            dbas[":fPARTYMETHOD"] = 0;
            dbas[":fROWID"] = rownum++;
            if (!dbas.insert("MTEXPENSELIST", false)) {
                dbas.rollback();
                C5Message::error(dbas.fLastError);
                return;
            }
        }
    }

    if (!nocalc.isEmpty()) {
        QDir d;
        QFile f(d.tempPath() + "/err.txt");
        if (f.open(QIODevice::WriteOnly)) {
            f.write("No calculation\r\n");
            QString ba = nocalc.join("\r\n");
            f.write(ba.toUtf8());
            f.close();
            QDesktopServices::openUrl(QUrl(d.tempPath() + "/err.txt"));
        }
    }

    if (calc.count() == 0) {
        C5Message::info(tr("Nothing to create"));
        return;
    }

    C5Message::info(tr("Done"));
}
