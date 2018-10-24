#include "c5settingswidget.h"
#include "ui_c5settingswidget.h"

C5SettingsWidget::C5SettingsWidget(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5SettingsWidget)
{
    ui->setupUi(this);
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
    QMap<int, QString> fTags;
    fTags[ui->leLocalReceiptPrinter->getTag()] = ui->leLocalReceiptPrinter->text();
    fTags[ui->leServiceFactor->getTag()] = ui->leServiceFactor->text();
    fTags[ui->leIDRAMID->getTag()] = ui->leIDRAMID->text();
    fTags[ui->leIDRAMPhone->getTag()] = ui->leIDRAMPhone->text();
    fTags[ui->leTaxIP->getTag()] = ui->leTaxIP->text();
    fTags[ui->leTaxPort->getTag()] = ui->leTaxPort->text();
    fTags[ui->leTaxPassword->getTag()] = ui->leTaxPassword->text();
    fTags[ui->leTaxDept->getTag()] = ui->leTaxDept->text();
    C5Database db(fDBParams);
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
    }
}

void C5SettingsWidget::setWidgetValue(QWidget *w, const QString &value)
{
    if (!strcmp(w->metaObject()->className(), "C5LineEdit")) {
        static_cast<C5LineEdit*>(w)->setText(value);
    }
}

void C5SettingsWidget::clear(QWidget *parent)
{
    if (!parent) {
        parent = this;
    }
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
