#include "c5settingswidget.h"
#include "ui_c5settingswidget.h"
#include "jsons.h"
#include "c5utils.h"
#include <QFileDialog>

C5SettingsWidget::C5SettingsWidget(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::C5SettingsWidget)
{
    ui->setupUi(this);
    ui->cbMenu->setDBValues(fDBParams, "select f_id, f_name from d_menu_names order by 2");
    ui->cbDefaultStore->setDBValues(fDBParams, "select f_id, f_name from c_storages order by 2");
    ui->cbTaxUseExtPos->addItem(tr("Yes"), "true");
    ui->cbTaxUseExtPos->addItem(tr("No"), "false");
    ui->cbFronDeskMode->addItem(tr("Waiter"), 0);
    ui->cbFronDeskMode->addItem(tr("Shop"), 1);
    ui->cbDefaultCurrency->setDBValues(dbParams, "select f_id, f_name from e_currency");
}

C5SettingsWidget::~C5SettingsWidget()
{
    delete ui;
}

void C5SettingsWidget::setId(int id)
{
    CE5Editor::setId(id);
    clear(this);
    fSettingsId = id;
    C5Database db(fDBParams);
    db[":f_settings"] = id;
    db.exec("select f_key, f_value from s_settings_values where f_settings=:f_settings");
    while (db.nextRow()) {
        QWidget *w = widget(this, db.getInt(0));
        if (w) {
            setWidgetValue(w, db.getString(1));
        } else {
            qDebug() << "No widget for: " << db.getInt(0) << db.getString(1);
        }
    }
    db.exec("select f_id, f_counter from a_type");
    while (db.nextRow()) {
        switch (db.getInt(0)) {
            case DOC_TYPE_STORE_INPUT:
                ui->leInputDocCounter->setInteger(db.getInt(1));
                break;
            case DOC_TYPE_STORE_OUTPUT:
                ui->leOutDocCounter->setInteger(db.getInt(1));
                break;
            case DOC_TYPE_STORE_MOVE:
                ui->leMoveDocCounter->setInteger(db.getInt(1));
                break;
            default:
                break;
        }
    }
    db[":f_id"] = id;
    db.exec("select * from sys_json_config where f_id=:f_id");
    if (db.nextRow()) {
        QJsonObject jo = __strjson(db.getString("f_config"));
        ui->leChatOperatorUserId->setInteger(jo["chatoperatoruserid"].toInt());
        ui->chDenyLogout->setChecked(jo["denylogout"].toBool());
        ui->leCoinCashdesk->setInteger(jo["coincash_id"].toInt());
        ui->chPrepaidOnlyByGift->setChecked(jo["prepaid_only_by_gift"].toBool());
        ui->chHideRevenue->setChecked(jo["hide_revenue"].toBool());
        ui->chShowWhosalePrice->setChecked(jo["show_whosale_price"].toBool());
        //buttons config
        ui->chmNewOrder->setChecked(jo["chm_neworder"].toBool());
        ui->chmOrders->setChecked(jo["chm_orders"].toBool());
        ui->chmCompletedOrders->setChecked(jo["chm_completedorders"].toBool());
        ui->chmDebts->setChecked(jo["chm_debts"].toBool());
        ui->chmCheckQuantity->setChecked(jo["chm_checkqty"].toBool());
        ui->chmDraftSale->setChecked(jo["chm_draftsale"].toBool());
        ui->chmStoreInputCheck->setChecked(jo["chm_storeinputcheck"].toBool());
        ui->chmSaleOutputConfirmation->setChecked(jo["chm_saleoutconfirmation"].toBool());
        ui->chmReturnGoods->setChecked(jo["chm_returngoods"].toBool());
        ui->chDebugMode->setChecked(jo["debug_mode"].toBool());
        QJsonArray ja = jo["availableoutstore"].toArray();
        ui->chUseWebsocket->setChecked(jo["use_websocket"].toBool());
        QString s;
        for (int i = 0; i < ja.count(); i++) {
            if (!s.isEmpty()) {
                s += ",";
            }
            s += QString::number(ja.at(i).toInt());
        }
        ui->leAvailableStore->setText(s);
    }
}

bool C5SettingsWidget::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    bool isNew = false;
    if (ui->leCode->getInteger() == 0) {
        isNew = true;
    }
    if (!CE5Editor::save(err, data)) {
        return false;
    }
    QMap<int, QString> fTags;
    fTags[ui->leLocalReceiptPrinter->getTag()] = ui->leLocalReceiptPrinter->text();
    fTags[ui->leServiceFactor->getTag()] = ui->leServiceFactor->text();
    fTags[ui->leIDRAMID->getTag()] = ui->leIDRAMID->text();
    fTags[ui->leIDRAMPhone->getTag()] = ui->leIDRAMPhone->text();
    fTags[ui->leTaxIP->getTag()] = ui->leTaxIP->text();
    fTags[ui->leTaxPort->getTag()] = ui->leTaxPort->text();
    fTags[ui->leTaxPassword->getTag()] = ui->leTaxPassword->text();
    fTags[ui->leTaxDept->getTag()] = ui->leTaxDept->text();
    fTags[ui->cbMenu->getTag()] = ui->cbMenu->currentData().toString();
    fTags[param_default_menu_name] = ui->cbMenu->currentText();
    fTags[ui->leOrderPrefix->getTag()] = ui->leOrderPrefix->text();
    fTags[ui->leHall->getTag()] = ui->leHall->text();
    fTags[ui->leTable->getTag()] = ui->leTable->text();
    fTags[ui->leInputDocNumDigits->getTag()] = ui->leInputDocNumDigits->text();
    fTags[ui->leMoveDocNumDigits->getTag()] = ui->leMoveDocNumDigits->text();
    fTags[ui->leOutDocNumDigits->getTag()] = ui->leOutDocNumDigits->text();
    fTags[ui->cbDefaultStore->getTag()] = ui->cbDefaultStore->currentData().toString();
    fTags[ui->cbTaxUseExtPos->getTag()] = ui->cbTaxUseExtPos->currentData().toString();
    fTags[ui->leHotelDatabase->getTag()] = ui->leHotelDatabase->text();
    fTags[ui->leItemCodeForHotel->getTag()] = ui->leItemCodeForHotel->text();
    fTags[ui->chNoCashDoc->getTag()] = ui->chNoCashDoc->isChecked() ? "1" : "0";
    fTags[ui->chNoScanCode->getTag()] = ui->chNoScanCode->isChecked() ? "1" : "0";
    fTags[ui->leChangeWorkingDateTime->getTag()] = ui->leChangeWorkingDateTime->text();
    fTags[ui->chAlwaysOfferTax->getTag()] = ui->chAlwaysOfferTax->isChecked() ? "1" : "0";
    fTags[ui->leFDFontSize->getTag()] = ui->leFDFontSize->text();
    fTags[ui->chCarMode->getTag()] = ui->chCarMode->isChecked() ? "1" : "0";
    fTags[ui->cbFronDeskMode->getTag()] = ui->cbFronDeskMode->currentData().toString();
    fTags[ui->chEnterShopPin->getTag()] = ui->chEnterShopPin->isChecked() ? "1" : "0";
    fTags[ui->chAutoCash->getTag()] = ui->chAutoCash->isChecked() ? "1" : "0";
    fTags[ui->leCashId->getTag()] = ui->leCashId->text();
    fTags[ui->leCardId->getTag()] = ui->leCardId->text();
    fTags[ui->leCashIdPrefix->getTag()] = ui->leCashIdPrefix->text();
    fTags[ui->leCardIdPrefix->getTag()] = ui->leCardIdPrefix->text();
    fTags[ui->leCashierLogin->getTag()] = ui->leCashierLogin->text();
    fTags[ui->leCashierPin->getTag()] = ui->leCashierPin->text();
    fTags[ui->chDenyPriceChange->getTag()] = ui->chDenyPriceChange->isChecked() ? "1" : "0";
    fTags[ui->chDenyReatail->getTag()] = ui->chDenyReatail->isChecked() ? "1" : "0";
    fTags[ui->chDenyWhosale->getTag()] = ui->chDenyWhosale->isChecked() ? "1" : "0";
    fTags[ui->chFixShopStaff->getTag()] = ui->chFixShopStaff->isChecked() ? "1" : "0";
    fTags[ui->chWaiterLoginAfterPayment->getTag()] = ui->chWaiterLoginAfterPayment->isChecked() ? "1" : "0";
    fTags[ui->rbPrintV1->property("tag").toInt()] = ui->rbPrintV1->isChecked() ? "1" : "0";
    fTags[ui->rbPrintV2->property("tag").toInt()] = ui->rbPrintV2->isChecked() ? "1" : "0";
    fTags[ui->rbPrintV3->property("tag").toInt()] = ui->rbPrintV3->isChecked() ? "1" : "0";
    fTags[ui->leAutologinPin1->getTag()] = ui->leAutologinPin1->text();
    fTags[ui->leAutologinPin2->getTag()] = ui->leAutologinPin2->text();
    fTags[ui->chFixAutoenterInputDocPrice->getTag()] = ui->chFixAutoenterInputDocPrice->isChecked() ? "1" : "0";
    fTags[ui->leReceiptFooterText->getTag()] = ui->leReceiptFooterText->text();
    fTags[ui->rbPrintDown->property("tag").toInt()] = ui->rbPrintDown->isChecked() ? "1" : "0";
    fTags[ui->rbPrintLeft->property("tag").toInt()] = ui->rbPrintLeft->isChecked() ? "1" : "0";
    fTags[ui->chPrintScancodeWithName->getTag()] = ui->chPrintScancodeWithName->isChecked() ? "1" : "0";
    fTags[ui->chPrintDishOrder->getTag()] = ui->chPrintDishOrder->isChecked() ? "1" : "0";
    fTags[ui->leReceipPaperWidth->getTag()] = ui->leReceipPaperWidth->text();
    fTags[ui->chAutoDateCash->getTag()] = ui->chAutoDateCash->isChecked() ? "1" : "0";
    fTags[ui->leDateOfClose->getTag()] = ui->leDateOfClose->text();
    fTags[ui->leShift->getTag()] = ui->leShift->text();
    fTags[ui->leHttpIP->getTag()] = ui->leHttpIP->text();
    fTags[ui->leHttpPort->getTag()] = ui->leHttpPort->text();
    fTags[ui->leHttpUsername->getTag()] = ui->leHttpUsername->text();
    fTags[ui->leHttpPassword->getTag()] = ui->leHttpPassword->text();
    fTags[ui->rbPrint50->property("tag").toInt()] = ui->rbPrint50->isChecked() ? "1" : "0";
    fTags[ui->rbPrint80->property("tag").toInt()] = ui->rbPrint80->isChecked() ? "1" : "0";
    fTags[ui->leASConnectionString->getTag()] = ui->leASConnectionString->text();
    fTags[ui->leUserIdForHotelVoucher->getTag()] = ui->leUserIdForHotelVoucher->text();
    fTags[ui->leHallIdForHotel->getTag()] = ui->leHallIdForHotel->text();
    fTags[ui->leStaticQRCode->getTag()] = ui->leStaticQRCode->text();
    fTags[ui->leIdramTipsWallet->getTag()] = ui->leIdramTipsWallet->text();
    fTags[ui->chIdramTips->getTag()] = ui->chIdramTips->isChecked() ? "1" : "0";
    fTags[ui->leIdramName->getTag()] = ui->leIdramName->text();
    fTags[ui->leIdramSessionId->getTag()] = ui->leIdramSessionId->text();
    fTags[ui->chExternalTax->getTag()] = ui->chExternalTax->isChecked() ? "1" : "0";
    fTags[ui->chCloseTablefterServicePrint->getTag()] = ui->chCloseTablefterServicePrint->isChecked() ? "1" : "0";
    fTags[ui->leScalePath->getTag()] = ui->leScalePath->text();
    fTags[ui->chDenyQtychange->getTag()] = ui->chDenyQtychange->isChecked() ? "1" : "0";
    fTags[ui->chDontGroupPrint->getTag()] = ui->chDontGroupPrint->isChecked() ? "1" : "0";
    fTags[ui->chDebugMode->getTag()] = ui->chDebugMode->isChecked() ? "1" : "0";
    fTags[ui->cbIdramServer->property("Tag").toInt()] = ui->cbIdramServer->currentText();
    fTags[ui->chCloseTableAfterPrecheck->property("Tag").toInt()] = ui->chCloseTableAfterPrecheck->isChecked() ? "1" : "0";
    fTags[ui->chDontCheckStoreQty->property("Tag").toInt()] = ui->chDontCheckStoreQty->isChecked() ? "1" : "0";
    fTags[ui->leVAT->getTag()] = ui->leVAT->text();
    //fTags[ui->chReportOfOnlyLogged->getTag()] = ui->chReportOfOnlyLogged->isChecked() ? "1" : "0";
    fTags[ui->chHideShopQuantity->getTag()] = ui->chHideShopQuantity->isChecked() ? "1" : "0";
    fTags[ui->leShopMobileClentConfig->getTag()] = ui->leShopMobileClentConfig->text();
    fTags[ui->chNeverOfferTax->getTag()] = ui->chNeverOfferTax->isChecked() ? "1" : "0";
    fTags[ui->chAutomaticallyStoreOutWaiter->getTag()] = ui->chAutomaticallyStoreOutWaiter->isChecked() ? "1" : "0";
    fTags[ui->rbPrintVNoPrint->property("tag").toInt()] = ui->rbPrintVNoPrint->isChecked() ? "1" : "0";
    fTags[ui->fnApp->property("Tag").toInt()] = ui->fnApp->currentText();
    fTags[ui->chAskForPrecheck->getTag()] = ui->chAskForPrecheck->isChecked() ? "1" : "0";
    fTags[ui->chFinalReceiptQtySelect->getTag()] = ui->chFinalReceiptQtySelect->isChecked() ? "1" : "0";
    fTags[ui->chForceUsePrintAlias->getTag()] = ui->chForceUsePrintAlias->isChecked() ? "1" : "0";
    fTags[ui->chCompactDishesInOrder->getTag()] = ui->chCompactDishesInOrder->isChecked() ? "1" : "0";
    fTags[ui->fbServiceFont->property("Tag").toInt()] = ui->fbServiceFont->currentText();
    fTags[ui->fbReceiptFont->property("Tag").toInt()] = ui->fbReceiptFont->currentText();
    fTags[ui->leServiceFontSize->getTag()] = ui->leServiceFontSize->text();
    fTags[ui->lereceiptFontSize->getTag()] = ui->lereceiptFontSize->text();
    fTags[ui->leSystemThouthandSeparator->getTag()] = ui->leSystemThouthandSeparator->text();
    fTags[ui->leTaxAlwaysPrintIfAmountLess->getTag()] = ui->leTaxAlwaysPrintIfAmountLess->text();
    fTags[ui->chPrintModOnReceipt->getTag()] = ui->chPrintModOnReceipt->isChecked() ? "1" : "0";
    fTags[ui->chDontCloseEmtpyOrder->getTag()] = ui->chDontCloseEmtpyOrder->isChecked() ? "1" : "0";
    fTags[ui->chDontPrintFinalReceipt->getTag()] = ui->chDontPrintFinalReceipt->isChecked() ? "1" : "0";
    fTags[ui->leAutochargeDishAfterSessionClose->getTag()] = ui->leAutochargeDishAfterSessionClose->text();
    fTags[ui->chChoosePos->getTag()] = ui->chChoosePos->isChecked() ? "1" : "0";
    fTags[ui->leMobileAppConStr->getTag()] = ui->leMobileAppConStr->text();
    fTags[ui->cbDefaultCurrency->getTag()] = ui->cbDefaultCurrency->currentData().toString();
    fTags[ui->chUpdateTempStorageForShop->getTag()] = ui->chUpdateTempStorageForShop->isChecked() ? "1" : "0";
    fTags[ui->chAppendQuantityBefore->getTag()] = ui->chAppendQuantityBefore->isChecked() ? "1" : "0";
    fTags[ui->leAutoDiscount->getTag()] = ui->leAutoDiscount->text();
    fTags[ui->leDeliveryCostumerId->getTag()] = ui->leDeliveryCostumerId->text();
    fTags[ui->leDebtholderFilterId->getTag()] = ui->leDebtholderFilterId->text();
    fTags[ui->chSimpleFiscal->getTag()] = ui->chSimpleFiscal->isChecked() ? "1" : "0";
    C5Database db(fDBParams);
    if (isNew) {
        db[":f_id"] = ui->leCode->getInteger();
        db[":f_name"] = ui->leSettingsName->text();
        db.insert("sys_json_config", false);
    }
    db[":f_settings"] = ui->leCode->getInteger();
    db.exec("delete from s_settings_values where f_settings=:f_settings");
    QString sql = "insert into s_settings_values (f_settings, f_key, f_value) values ";
    bool first = true;
    for (QMap<int, QString>::const_iterator it = fTags.constBegin(); it != fTags.constEnd(); it++) {
        if (first) {
            first = false;
        } else {
            sql += ",";
        }
        sql += QString("(%1, %2, '%3')").arg(ui->leCode->text(), QString::number(it.key()), it.value());
    }
    db.exec(sql);
    db[":f_counter"] = ui->leInputDocCounter->getInteger();
    db.update("a_type", where_id(DOC_TYPE_STORE_INPUT));
    db[":f_counter"] = ui->leOutDocCounter->getInteger();
    db.update("a_type", where_id(DOC_TYPE_STORE_OUTPUT));
    db[":f_counter"] = ui->leMoveDocCounter->getInteger();
    db.update("a_type", where_id(DOC_TYPE_STORE_MOVE));
    QStringList halls = ui->leHall->text().split(",", Qt::SkipEmptyParts);
    if (halls.isEmpty()) {
        halls.append("0");
    }
    QJsonObject jc;
    jc["store"] = ui->cbDefaultStore->currentData().toInt();
    jc["store_name"] = ui->cbDefaultStore->currentText();
    jc["first_page_title"] = ui->leSettingsName->text();
    jc["hall"] = halls.first().toInt();
    jc["table"] = ui->leTable->getInteger();
    jc["menu"] = ui->cbMenu->currentData().toInt();
    jc["companydata"] = ui->leCompanyInfo->text();
    jc["companyname"] = ui->leCompanyname->text();
    jc["receiptprinter"] = ui->leLocalReceiptPrinter->text();
    jc["chatoperatoruserid"] = ui->leChatOperatorUserId->text().toInt();
    jc["servicefactor"] = ui->leServiceFactor->getDouble();
    jc["debugmode"] = ui->chDebugMode->isChecked();
    jc["denylogout"] = ui->chDenyLogout->isChecked();
    jc["cashbox_id"] = ui->leCashId->getInteger();
    jc["coincash_id"] = ui->leCoinCashdesk->getInteger();
    jc["storage"] = ui->cbDefaultStore->currentData().toInt();
    jc["cash_cash_id"] = ui->leCashId->getInteger();
    jc["cash_card_id"] = ui->leCardId->getInteger();
    jc["addhall"] = ui->leHall->text();
    jc["prepaid_only_by_gift"] = ui->chPrepaidOnlyByGift->isChecked();
    jc["hide_revenue"] = ui->chHideRevenue->isChecked();
    jc["show_whosale_price"] = ui->chShowWhosalePrice->isChecked();
    jc["chm_neworder"] = ui->chmNewOrder->isChecked();
    jc["chm_orders"] = ui->chmOrders->isChecked();
    jc["chm_completedorders"] = ui->chmCompletedOrders->isChecked();
    jc["chm_debts"] = ui->chmDebts->isChecked();
    jc["chm_checkqty"] = ui->chmCheckQuantity->isChecked();
    jc["chm_draftsale"] = ui->chmDraftSale->isChecked();
    jc["chm_storeinputcheck"] = ui->chmStoreInputCheck->isChecked();
    jc["chm_saleoutconfirmation"] = ui->chmSaleOutputConfirmation->isChecked();
    jc["chm_returngoods"] = ui->chmReturnGoods->isChecked();
    jc["use_websocket"] = ui->chUseWebsocket->isChecked();
    jc["debug_mode"] = ui->chDebugMode->isChecked();
    QJsonArray ja;
    QStringList a = ui->leAvailableStore->text().split(",", Qt::SkipEmptyParts);
    for  (const QString &s : a) {
        ja.append(s.toInt());
    }
    jc["availableoutstore"] = ja;
    db[":f_id"] = ui->leCode->getInteger();
    db[":f_name"] = ui->leSettingsName->text();
    db[":f_config"] = __jsonstr(jc);
    db.update("sys_json_config", "where f_id=:f_id");
    __c5config.initParamsFromDb();
    return true;
}

void C5SettingsWidget::clearWidgetValue(QWidget *w)
{
    if (!strcmp(w->metaObject()->className(), "C5LineEdit")) {
        static_cast<C5LineEdit *>(w)->clear();
    } else if (!strcmp(w->metaObject()->className(), "C5ComboBox")) {
        static_cast<C5ComboBox *>(w)->setCurrentIndex(-1);
    } else if  (!strcmp(w->metaObject()->className(), "QRadioButton")) {
        static_cast<QRadioButton *>(w)->setAutoExclusive(false);
        static_cast<QRadioButton *>(w)->setChecked(false);
        static_cast<QRadioButton *>(w)->setAutoExclusive(true);
    } else if (!strcmp(w->metaObject()->className(), "QComboBox")) {
        static_cast<QComboBox *>(w)->setCurrentIndex(-1);
    } else if (!strcmp(w->metaObject()->className(), "QFontComboBox")) {
        static_cast<QFontComboBox *>(w)->setCurrentIndex(-1);
    }
}

void C5SettingsWidget::setWidgetValue(QWidget *w, const QString &value)
{
    if (!strcmp(w->metaObject()->className(), "C5LineEdit")
            || !strcmp(w->metaObject()->className(), "C5LineEditWithSelector")) {
        static_cast<C5LineEdit *>(w)->setText(value);
    } else if (!strcmp(w->metaObject()->className(), "C5ComboBox")) {
        static_cast<C5ComboBox *>(w)->setIndexForValue(value);
    } else if (!strcmp(w->metaObject()->className(), "C5CheckBox")) {
        static_cast<C5CheckBox *>(w)->setChecked(value == "1");
    } else if  (!strcmp(w->metaObject()->className(), "QRadioButton")) {
        static_cast<QRadioButton *>(w)->setChecked(value == "1");
    } else if (!strcmp(w->metaObject()->className(), "QComboBox")) {
        static_cast<QComboBox *>(w)->setCurrentText(value);
    } else if (!strcmp(w->metaObject()->className(), "QFontComboBox")) {
        static_cast<QFontComboBox *>(w)->setCurrentText(value);
    }
}

void C5SettingsWidget::clear(QWidget *parent)
{
    if (!parent) {
        parent = this;
    }
    fSettingsId = 0;
    QObjectList ol = parent->children();
    foreach (QObject *o, ol) {
        if (o->children().count() > 0) {
            clear(static_cast<QWidget *>(o));
        }
        QVariant p = o->property("Tag");
        if (p.isValid()) {
            clearWidgetValue(static_cast<QWidget *>(o));
        } else {
            p = o->property("tag");
            if (p.isValid()) {
                clearWidgetValue(static_cast<QWidget *>(o));
            }
        }
    }
    ui->leInputDocCounter->clear();
    ui->leOutDocCounter->clear();
    ui->leMoveDocCounter->clear();
    ui->leChatOperatorUserId->clear();
}

bool C5SettingsWidget::canCopy()
{
    return true;
}

void C5SettingsWidget::copyObject()
{
    ui->leCode->clear();
}

QWidget *C5SettingsWidget::widget(QWidget *parent, int tag)
{
    Q_ASSERT(tag != 0);
    QObjectList ol = parent->children();
    foreach (QObject *o, ol) {
        if (!strcmp(o->metaObject()->className(), "C5LineEdit")
                || !strcmp(o->metaObject()->className(), "C5LineEditWithSelector")) {
            C5LineEdit *l = static_cast<C5LineEdit *>(o);
            if (l->getTag() == tag) {
                return l;
            }
        } else if (!strcmp(o->metaObject()->className(), "QComboBox")) {
            QComboBox *c = static_cast<QComboBox *>(o);
            if (c->property("Tag").toInt() == tag) {
                return c;
            }
        } else if (!strcmp(o->metaObject()->className(), "C5ComboBox")) {
            C5ComboBox *l = static_cast<C5ComboBox *>(o);
            if (l->getTag() == tag) {
                return l;
            }
        } else if (!strcmp(o->metaObject()->className(), "C5CheckBox")) {
            C5CheckBox *c = static_cast<C5CheckBox *>(o);
            if (c->getTag() == tag) {
                return c;
            }
        } else if (!strcmp(o->metaObject()->className(), "QRadioButton")) {
            if (o->property("tag").toInt() == tag) {
                return static_cast<QRadioButton *>(o);
            }
        } else if (!strcmp(o->metaObject()->className(), "QFontComboBox")) {
            if (o->property("Tag").toInt() == tag) {
                return static_cast<QFontComboBox *>(o);
            }
        } else if (o->children().count() > 0) {
            QWidget *w = widget(static_cast<QWidget *>(o), tag);
            if (w) {
                return w;
            }
        } else {
            //qDebug() << "Strange widget" << o ;
        }
    }
    return nullptr;
}

void C5SettingsWidget::on_btnScalePath_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "", "");
    if (!path.isEmpty()) {
        ui->leScalePath->setText(path);
    }
}
