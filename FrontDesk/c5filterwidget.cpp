#include "c5filterwidget.h"
#include "c5lineeditwithselector.h"
#include "c5dateedit.h"
#include "c5checkbox.h"
#include <QSettings>

C5FilterWidget::C5FilterWidget(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent)
{
    fFocusNextChild = false;
}

void C5FilterWidget::saveFilter(QWidget *parent)
{
    QSettings s(_ORGANIZATION_, QString("%1\\%2\\reportfilter\\%3")
                .arg(_APPLICATION_)
                .arg(_MODULE_)
                .arg(metaObject()->className()));
    QObjectList ol = parent->children();
    QVariant value;
    C5LineEditWithSelector *le;
    C5DateEdit *de;
    C5CheckBox *ce;
    foreach (QObject *o, ol) {
        if (o->children().count() > 0) {
            saveFilter(static_cast<QWidget*>(o));
        }
        if ((le = isLineEditWithSelector(o))) {
            value = le->property("FilterName");
            if (value.isValid()) {
                s.setValue(value.toString(), le->text());
            }
            continue;
        }
        if ((de = isDateEdit(o))) {
            value = de->property("FilterName");
            if (value.isValid()) {
                s.setValue(value.toString(), de->date());
            }
            continue;
        }
        if ((ce = isCheckBox(o))) {
            value = ce->property("FilterName");
            if (value.isValid()) {
                s.setValue(value.toString(), ce->isChecked());
            }
            continue;
        }
    }
}

void C5FilterWidget::restoreFilter(QWidget *parent)
{
    QSettings s(_ORGANIZATION_, QString("%1\\%2\\reportfilter\\%3")
                .arg(_APPLICATION_)
                .arg(_MODULE_)
                .arg(metaObject()->className()));
    QObjectList ol = parent->children();
    QString filterName;
    QVariant value;
    C5LineEditWithSelector *le;
    C5DateEdit *de;
    C5CheckBox *ce;
    foreach (QObject *o, ol) {
        if (o->children().count() > 0) {
            saveFilter(static_cast<QWidget*>(o));
        }
        if ((le = isLineEditWithSelector(o))) {
            filterName = le->property("FilterName").toString();
            if (filterName.isEmpty()) {
                continue;
            }
            value = s.value(filterName, "");
            le->setValue(value.toString());
            continue;
        }
        if ((de = isDateEdit(o))) {
            filterName = de->property("FilterName").toString();
            if (filterName.isEmpty()) {
                continue;
            }
            value = s.value(filterName, QDate::currentDate());
            de->setDate(value.toDate());
            continue;
        }
        if ((ce = isCheckBox(o))) {
            filterName = ce->property("FilterName").toString();
            if (filterName.isEmpty()) {
                continue;
            }
            value = s.value(filterName, 0);
            ce->setChecked(value.toBool());
            continue;
        }
    }
}

void C5FilterWidget::clearFilter(QWidget *parent)
{
    QObjectList ol = parent->children();
    C5LineEditWithSelector *le;
    C5DateEdit *de;
    C5CheckBox *ce;
    foreach (QObject *o, ol) {
        if (o->children().count() > 0) {
            clearFilter(static_cast<QWidget*>(o));
        }
        if ((le = isLineEditWithSelector(o))) {
            le->setValue("");
            continue;
        }
        if ((de = isDateEdit(o))) {
            de->setDate(QDate::currentDate());
            continue;
        }
        if ((ce = isCheckBox(o))) {
            ce->setChecked(false);
            continue;
        }
    }
}
