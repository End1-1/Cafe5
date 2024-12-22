#include "widgetcontainer.h"
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QDebug>
#include <QSettings>

static QSettings _s(_ORGANIZATION_, _APPLICATION_);

static const int type_lineedit = 1;
static const int type_textedit = 2;
static const int type_combobox = 3;
static QLineEdit *lineEdit = nullptr;
static QTextEdit *textEdit = nullptr;
static QComboBox *comboBox = nullptr;

WidgetContainer::WidgetContainer(QWidget *parent) :
    QWidget(parent)
{
}

void WidgetContainer::getWatchList(QWidget *c)
{
    QObjectList ol = c->children();
    QWidget *w = nullptr;
    QLineEdit *le = nullptr;
    QTextEdit *te = nullptr;
    QComboBox *cb = nullptr;
    QVariant s;
    for (QObject *o : ol) {
        w = dynamic_cast<QWidget *>(o);
        if (w) {
            getWatchList(w);
        }
        le = dynamic_cast<QLineEdit *>(o);
        if (le) {
            s = checkWidget(le, type_lineedit);
            le->setText(_s.value(s.toString()).toString());
            continue;
        }
        te = dynamic_cast<QTextEdit *>(o);
        if (te) {
            s = checkWidget(te, type_textedit);
            te->setPlainText(_s.value(s.toString()).toString());
            continue;
        }
        cb = dynamic_cast<QComboBox *>(o);
        if (cb) {
            s = checkWidget(cb, type_combobox);
            cb->setCurrentIndex(cb->findText(_s.value(s.toString()).toString()));
            continue;
        }
    }
}

bool WidgetContainer::hasChanges()
{
    QLineEdit *le = nullptr;
    QTextEdit *te = nullptr;
    QComboBox *cb = nullptr;
    for (QWidget *w : fWatchList) {
        le = dynamic_cast<QLineEdit *>(w);
        if (le) {
            if (le->property("old").toString() != le->text()) {
                return true;
            }
            continue;
        }
        te = dynamic_cast<QTextEdit *>(w);
        if (te) {
            if (te->property("old").toString() != te->toPlainText()) {
                return true;
            }
            continue;
        }
        cb = dynamic_cast<QComboBox *>(w);
        if (cb) {
            if (cb->property("old").toString() != cb->currentText()) {
                return true;
            }
            continue;
        }
    }
    return false;
}

void WidgetContainer::saveChanges()
{
    QLineEdit *le = nullptr;
    QTextEdit *te = nullptr;
    QComboBox *cb = nullptr;
    for (QWidget *w : fWatchList) {
        le = dynamic_cast<QLineEdit *>(w);
        if (le) {
            _s.setValue(le->property("settings").toString(), le->text());
            continue;
        }
        te = dynamic_cast<QTextEdit *>(w);
        if (te) {
            _s.setValue(te->property("settings").toString(), te->toPlainText());
            continue;
        }
        cb = dynamic_cast<QComboBox *>(w);
        if (cb) {
            _s.setValue(cb->property("settings").toString(), cb->currentText());
            continue;
        }
    }
}

int WidgetContainer::getInt(const QString &name) const
{
    switch (fWidgetsTypes[name]) {
        case type_lineedit:
            lineEdit = static_cast<QLineEdit *>(fWidgetsList[name]);
            return QLocale().toInt(lineEdit->text());
        default:
            return -1;
    }
}

QVariant WidgetContainer::checkWidget(QWidget *w, int wtype)
{
    QVariant s = w->property("settings");
    if (s.isValid()) {
        w->setProperty("old", _s.value(s.toString()));
    }
    fWatchList.append(w);
    fWidgetsList[s.toString()] = w;
    fWidgetsTypes[s.toString()] = wtype;
    return s;
}
