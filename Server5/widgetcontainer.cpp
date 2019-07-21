#include "widgetcontainer.h"
#include <QLineEdit>
#include <QDebug>
#include <QSettings>

static QSettings _s(_ORGANIZATION_, _APPLICATION_);

static const int type_lineedit = 1;
static QLineEdit *lineEdit = nullptr;

WidgetContainer::WidgetContainer(QWidget *parent) :
    QWidget(parent)
{

}

void WidgetContainer::getWatchList(QWidget *c)
{
    QObjectList ol = c->children();
    QWidget *w = nullptr;
    QLineEdit *le = nullptr;
    QVariant s;
    for (QObject *o: ol) {
        w = dynamic_cast<QWidget*>(o);
        if (w) {
            getWatchList(w);
        }
        le = dynamic_cast<QLineEdit*>(o);
        if (le) {
            s = le->property("settings");
            if (s.isValid()) {
                le->setProperty("old", _s.value(s.toString()));
                le->setText(_s.value(s.toString()).toString());
            }
            fWatchList.append(w);
            fWidgetsList[s.toString()] = w;
            fWidgetsTypes[s.toString()] = type_lineedit;
            continue;
        }
    }
}

bool WidgetContainer::hasChanges()
{
    QLineEdit *le = nullptr;
    for (QWidget *w: fWatchList) {
        le = dynamic_cast<QLineEdit*>(w);
        if (le) {
            if (le->property("old").toString() != le->text()) {
                return true;
            }
        }
    }
    return false;
}

void WidgetContainer::saveChanges()
{
    QLineEdit *le = nullptr;
    for (QWidget *w: fWatchList) {
        le = dynamic_cast<QLineEdit*>(w);
        if (le) {
            _s.setValue(le->property("settings").toString(), le->text());
        }
    }
}

int WidgetContainer::getInt(const QString &name) const
{
    switch (fWidgetsTypes[name]) {
    case type_lineedit:
        lineEdit = static_cast<QLineEdit*>(fWidgetsList[name]);
        return QLocale().toInt(lineEdit->text());
    default:
        return -1;
    }
}
