#include "c5settingswidget.h"
#include "ui_c5settingswidget.h"

C5SettingsWidget::C5SettingsWidget(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5SettingsWidget)
{
    ui->setupUi(this);
    ui->cbMenu->setDBValues(fDBParams, "select f_id, f_name from d_menu_names order by 2");
}

C5SettingsWidget::~C5SettingsWidget()
{
    delete ui;
}

void C5SettingsWidget::setSettingsId(int id)
{
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
}

void C5SettingsWidget::save()
{
    if (fSettingsId == 0) {
        return;
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
    C5Database db(fDBParams);
    db[":f_settings"] = fSettingsId;
    db.exec("delete from s_settings_values where f_settings=:f_settings");
    for (QMap<int, QString>::const_iterator it = fTags.begin(); it != fTags.end(); it++) {
        db[":f_settings"] = fSettingsId;
        db[":f_key"] = it.key();
        db[":f_value"] = it.value();
        db.insert("s_settings_values", false);
    }

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
    if (!strcmp(w->metaObject()->className(), "C5LineEdit")) {
        static_cast<C5LineEdit*>(w)->setText(value);
    } else if (!strcmp(w->metaObject()->className(), "C5ComboBox")) {
        static_cast<C5ComboBox*>(w)->setIndexForValue(value);
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
        }
        if (o->children().count() > 0) {
            QWidget *w = widget(static_cast<QWidget*>(o), tag);
            if (w) {
                return w;
            }
        }
    }
    return 0;
}
