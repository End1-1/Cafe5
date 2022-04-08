#include "c5filterwidget.h"
#include "c5lineeditwithselector.h"
#include "c5dateedit.h"
#include "c5checkbox.h"
#include "c5guicontrols.h"
#include <QSettings>
#include <QToolButton>

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
    C5LineEditWithSelector *le = nullptr;
    C5DateEdit *de = nullptr;
    C5CheckBox *ce = nullptr;
    QToolButton *tb = nullptr;
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
            if (fixDates()) {
                de->setDate(value.toDate());
            }
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
        if (__guic.isToolButton(tb, o)) {
            if (tb->property("FixDate").toBool()) {
                tb->setCheckable(true);
                connect(tb, SIGNAL(clicked(bool)), this, SLOT(setFixDate(bool)));
                tb->setChecked(fixDates());
            }
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

bool C5FilterWidget::fixDates()
{
    QSettings s(_ORGANIZATION_, QString("%1\\%2\\reportfilter\\%3")
                .arg(_APPLICATION_)
                .arg(_MODULE_)
                .arg(metaObject()->className()));
    return s.value("fixdate").toBool();
}

void C5FilterWidget::setDatabase(const QStringList &dbParams)
{
    fDBParams = dbParams;
    restoreFilter(this);
}

void C5FilterWidget::setFilterValue(const QString &key, const QVariant &value)
{
    QWidget *w = getWidget(key, this);
    if (dynamic_cast<C5DateEdit*>(w)) {
        static_cast<C5DateEdit*>(w)->setDate(value.toDate());
    } else if (dynamic_cast<C5LineEditWithSelector*>(w)) {
        static_cast<C5LineEditWithSelector*>(w)->setValue(value.toString());
    }
}

void C5FilterWidget::setFixDate(bool v)
{
    QSettings s(_ORGANIZATION_, QString("%1\\%2\\reportfilter\\%3")
                .arg(_APPLICATION_, _MODULE_, metaObject()->className()));
    s.setValue("fixdate", v);
}

QString C5FilterWidget::in(QString &cond, const QString &field, C5LineEditWithSelector *l)
{
    if (l->isEmpty()) {
        return cond;
    }
    cond += QString (" and %1 in (%2)").arg(field, l->text());
    return cond;
}

QWidget *C5FilterWidget::getWidget(const QString &key, QWidget *parent)
{
    QObjectList ol = parent->children();
    for (QObject *o: ol) {
        QWidget *w = dynamic_cast<QWidget*>(o);
        if (!w) {
            continue;
        }
        if (w->property("FilterName") == key) {
            return w;
        }
        if (w->children().count() > 0) {
            w = getWidget(key, w);
            if (w) {
                return w;
            }
        }
    }
    return nullptr;
}
