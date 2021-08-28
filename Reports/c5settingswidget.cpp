#include "c5settingswidget.h"
#include "ui_c5settingswidget.h"

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
}

bool C5SettingsWidget::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
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
    fTags[ui->chRDB->getTag()] = ui->chRDB->isChecked() ? "1" : "0";
    fTags[ui->leRDBHost->getTag()] = ui->leRDBHost->text();
    fTags[ui->leRDBSchema->getTag()] = ui->leRDBSchema->text();
    fTags[ui->leRDBUser->getTag()] = ui->leRDBUser->text();
    fTags[ui->leRDBPassword->getTag()] = ui->leRDBPassword->text();
    fTags[ui->chDenyPriceChange->getTag()] = ui->chDenyPriceChange->isChecked() ? "1" : "0";
    fTags[ui->chDenyReatail->getTag()] = ui->chDenyReatail->isChecked() ? "1" : "0";
    fTags[ui->chDenyWhosale->getTag()] = ui->chDenyWhosale->isChecked() ? "1" : "0";
    fTags[ui->chFixShopStaff->getTag()] = ui->chFixShopStaff->isChecked() ? "1" : "0";
    fTags[ui->chWaiterLoginAfterPayment->getTag()] = ui->chWaiterLoginAfterPayment->isChecked() ? "1" : "0";
    fTags[ui->rbPrintV1->property("tag").toInt()] = ui->rbPrintV1->isChecked() ? "1" : "0";
    fTags[ui->rbPrintV2->property("tag").toInt()] = ui->rbPrintV2->isChecked() ? "1" : "0";
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
    C5Database db(fDBParams);
    db[":f_settings"] = ui->leCode->getInteger();
    db.exec("delete from s_settings_values where f_settings=:f_settings");
    for (QMap<int, QString>::const_iterator it = fTags.constBegin(); it != fTags.constEnd(); it++) {
        db[":f_settings"] = ui->leCode->getInteger();
        db[":f_key"] = it.key();
        db[":f_value"] = it.value();
        db.insert("s_settings_values", false);
    }
    db[":f_counter"] = ui->leInputDocCounter->getInteger();
    db.update("a_type", where_id(DOC_TYPE_STORE_INPUT));
    db[":f_counter"] = ui->leOutDocCounter->getInteger();
    db.update("a_type", where_id(DOC_TYPE_STORE_OUTPUT));
    db[":f_counter"] = ui->leMoveDocCounter->getInteger();
    db.update("a_type", where_id(DOC_TYPE_STORE_MOVE));
    __c5config.initParamsFromDb();
    return true;
}

void C5SettingsWidget::clearWidgetValue(QWidget *w)
{
    if (!strcmp(w->metaObject()->className(), "C5LineEdit")) {
        static_cast<C5LineEdit*>(w)->clear();
    } else if (!strcmp(w->metaObject()->className(), "C5ComboBox")) {
        static_cast<C5ComboBox*>(w)->setCurrentIndex(-1);
    } else if  (!strcmp(w->metaObject()->className(), "QRadioButton")) {
        static_cast<QRadioButton*>(w)->setAutoExclusive(false);
        static_cast<QRadioButton*>(w)->setChecked(false);
        static_cast<QRadioButton*>(w)->setAutoExclusive(true);
    }
}

void C5SettingsWidget::setWidgetValue(QWidget *w, const QString &value)
{
    if (!strcmp(w->metaObject()->className(), "C5LineEdit") || !strcmp(w->metaObject()->className(), "C5LineEditWithSelector")) {
        static_cast<C5LineEdit*>(w)->setText(value);
    } else if (!strcmp(w->metaObject()->className(), "C5ComboBox")) {
        static_cast<C5ComboBox*>(w)->setIndexForValue(value);
    } else if (!strcmp(w->metaObject()->className(), "C5CheckBox")) {
        static_cast<C5CheckBox*>(w)->setChecked(value == "1");
    } else if  (!strcmp(w->metaObject()->className(), "QRadioButton")) {
        static_cast<QRadioButton*>(w)->setChecked(value == "1");
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
            clear(static_cast<QWidget*>(o));
        }
        QVariant p = o->property("Tag");
        if (p.isValid()) {
            clearWidgetValue(static_cast<QWidget*>(o));
        } else {
            p = o->property("tag");
            if (p.isValid()) {
                clearWidgetValue(static_cast<QWidget*>(o));
            }
        }
    }
    ui->leInputDocCounter->clear();
    ui->leOutDocCounter->clear();
    ui->leMoveDocCounter->clear();
}

QWidget *C5SettingsWidget::widget(QWidget *parent, int tag)
{
    Q_ASSERT(tag != 0);
    QObjectList ol = parent->children();
    foreach (QObject *o, ol) {
        if (!strcmp(o->metaObject()->className(), "C5LineEdit")
                || !strcmp(o->metaObject()->className(), "C5LineEditWithSelector")) {
            C5LineEdit *l = static_cast<C5LineEdit*>(o);
            if (l->getTag() == tag) {
                return l;
            }
        } else if (!strcmp(o->metaObject()->className(), "C5ComboBox")) {
            C5ComboBox *l = static_cast<C5ComboBox*>(o);
            if (l->getTag() == tag) {
                return l;
            }
        } else if (!strcmp(o->metaObject()->className(), "C5CheckBox")) {
            C5CheckBox *c = static_cast<C5CheckBox*>(o);
            if (c->getTag() == tag) {
                return c;
            }
        } else if (!strcmp(o->metaObject()->className(), "QRadioButton")) {
            if (o->property("tag").toInt() == tag) {
                return static_cast<QRadioButton*>(o);
            }
        } else if (o->children().count() > 0) {
            QWidget *w = widget(static_cast<QWidget*>(o), tag);
            if (w) {
                return w;
            }
        } else {
            //qDebug() << "Strange widget" << o ;
        }
    }
    return nullptr;
}

void C5SettingsWidget::on_btnTestAsConnection_clicked()
{
    C5Database dbas("QODBC3");
    dbas.setDatabase("", ui->leASConnectionString->text(), "", "");
    if (dbas.open()) {
        C5Message::error(tr("Connection successfull"));
    } else {
        C5Message::error(dbas.fLastError);
    }
}
