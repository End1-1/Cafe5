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
    ui->cbServiceMode->addItem(tr("Increase price of dish"), SERVICE_AMOUNT_MODE_INCREASE_PRICE);
    ui->cbServiceMode->addItem(tr("Print on receipt like a dish"), SERVICE_AMOUNT_MODE_SEPARATE);
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
    fTags[ui->leOrderPrefix->getTag()] = ui->leOrderPrefix->text();
    fTags[ui->leHall->getTag()] = ui->leHall->text();
    fTags[ui->leTable->getTag()] = ui->leTable->text();
    fTags[ui->leInputDocNumDigits->getTag()] = ui->leInputDocNumDigits->text();
    fTags[ui->leMoveDocNumDigits->getTag()] = ui->leMoveDocNumDigits->text();
    fTags[ui->leOutDocNumDigits->getTag()] = ui->leOutDocNumDigits->text();
    fTags[ui->cbDefaultStore->getTag()] = ui->cbDefaultStore->currentData().toString();
    fTags[ui->cbTaxUseExtPos->getTag()] = ui->cbTaxUseExtPos->currentData().toString();
    fTags[ui->leHotelDatabase->getTag()] = ui->leHotelDatabase->text();
    fTags[ui->cbServiceMode->getTag()] = ui->cbServiceMode->currentData().toString();
    fTags[ui->leItemCodeForHotel->getTag()] = ui->leItemCodeForHotel->text();
    fTags[ui->chNoCashDoc->getTag()] = ui->chNoCashDoc->isChecked() ? "1" : "0";
    fTags[ui->chNoScanCode->getTag()] = ui->chNoScanCode->isChecked() ? "1" : "0";
    fTags[ui->leChangeWorkingDateTime->getTag()] = ui->leChangeWorkingDateTime->text();
    fTags[ui->chAlwaysOfferTax->getTag()] = ui->chAlwaysOfferTax->isChecked() ? "1" : "0";
    fTags[ui->leFDFontSize->getTag()] = ui->leFDFontSize->text();
    fTags[ui->chCarMode->getTag()] = ui->chCarMode->isChecked() ? "1" : "0";
    C5Database db(fDBParams);
    db[":f_settings"] = ui->leCode->getInteger();
    db.exec("delete from s_settings_values where f_settings=:f_settings");
    for (QMap<int, QString>::const_iterator it = fTags.begin(); it != fTags.end(); it++) {
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
    }
}

void C5SettingsWidget::setWidgetValue(QWidget *w, const QString &value)
{
    if (!strcmp(w->metaObject()->className(), "C5LineEdit") ||
            !strcmp(w->metaObject()->className(), "C5LineEditWithSelector")) {
        static_cast<C5LineEdit*>(w)->setText(value);
    } else if (!strcmp(w->metaObject()->className(), "C5ComboBox")) {
        static_cast<C5ComboBox*>(w)->setIndexForValue(value);
    } else if (!strcmp(w->metaObject()->className(), "C5CheckBox")) {
        static_cast<C5CheckBox*>(w)->setChecked(value == "1");
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
        }
    }
    ui->leInputDocCounter->clear();
    ui->leOutDocCounter->clear();
    ui->leMoveDocCounter->clear();
}

QWidget *C5SettingsWidget::widget(QWidget *parent, int tag)
{
    QObjectList ol = parent->children();
    foreach (QObject *o, ol) {
        if (!strcmp(o->metaObject()->className(), "C5LineEdit")) {
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
        }
        if (o->children().count() > 0) {
            QWidget *w = widget(static_cast<QWidget*>(o), tag);
            if (w) {
                return w;
            }
        }
    }
    return nullptr;
}
