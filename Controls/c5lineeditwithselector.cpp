#include "c5lineeditwithselector.h"
#include "c5selector.h"
#include "c5cache.h"
#include "c5widget.h"

C5LineEditWithSelector::C5LineEditWithSelector(QWidget *parent) :
    C5LineEdit(parent)
{
    fCache = 0;
    fNameLineEdit = nullptr;
    fWidget = nullptr;
    fDialog = nullptr;
    fMultiselection = false;
}

C5LineEditWithSelector::~C5LineEditWithSelector()
{

}

C5LineEditWithSelector &C5LineEditWithSelector::setSelector(const QStringList &dbParams, QLineEdit *selName, int cacheId, int colId, int colName)
{
    fDBParams = dbParams;
    fNameLineEdit = selName;
    fCache = cacheId;
    fColumnId = colId;
    fColumnName = colName;
    return *this;
}

void C5LineEditWithSelector::setMultiselection(bool v)
{
    fMultiselection = v;
}

void C5LineEditWithSelector::setCallbackWidget(C5Widget *w)
{
    fWidget = w;
}

void C5LineEditWithSelector::setCallbackDialog(C5Dialog *d)
{
    fDialog = d;
}

void C5LineEditWithSelector::setValue(const QString &id)
{
    setText(id);
    if (fCache == 0) {
        return;
    }
    C5Cache *c = C5Cache::cache(fDBParams, fCache);
    QString text;
    int row = -1;
    QStringList ids = id.split(",", QString::SkipEmptyParts);
    foreach (const QString &s, ids) {
        row = c->find(s.toInt());
        if (row > -1) {
            if (!text.isEmpty()) {
                text += ",";
            }
            text += c->getString(row, fColumnName - 1);
        }
    }
    if (!text.isEmpty()) {
        fNameLineEdit->setText(text);
    } else {
        clear();
        fNameLineEdit->clear();
    }
    if (!fMultiselection) {
        if (fWidget && row > -1) {
            fWidget->selectorCallback(fCache, c->getRow(row));
        }
        if (fDialog && row > -1) {
            fDialog->selectorCallback(fCache, c->getRow(row));
        }
    }
}

void C5LineEditWithSelector::setValue(int id)
{
    setValue(QString::number(id));
}

QString C5LineEditWithSelector::text()
{
    if (property("allowall").toBool()) {
        return C5LineEdit::text();
    }
    QString allowedChar("-0123456789,");
    QString t = C5LineEdit::text().trimmed();
    if (fCache > 0) {
        for (int i = t.length() - 1; i > -1; i--) {
            if (!allowedChar.contains(t.at(i))) {
                t.remove(i, 1);
            }
        }
    }
    int i = t.length() - 1;
    while (i > 0) {
        if (t.at(i) == "-") {
            t.remove(i, 1);
        }
        if (t.at(i) == ",") {
            if (t.at(i - 1) == ",") {
                t.remove(i, 1);
            }
        }
        i--;
    }
    if (t.length() > 0) {
        if (t.at(0) == ",") {
            t.remove(0, 1);
        }
    }
    if (t.length() > 0) {
        if (t.at(t.length() - 1) == ",") {
            t.remove(t.length() - 1, 1);
        }
    }
    setValue(t);
    return t;
}

int C5LineEditWithSelector::cacheId()
{
    return fCache;
}

void C5LineEditWithSelector::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    if (fCache == 0) {
        emit doubleClicked();
        return;
    }

    if (fMultiselection) {
        QList<QList<QVariant> > values;
        if (!C5Selector::getMultipleValues(fDBParams, fCache, values)) {
            return;
        }
        QString textId, textName;
        foreach (const QList<QVariant> &c, values) {
            if (!textId.isEmpty()) {
                textId += ",";
                textName += ",";
            }
            textId += c.at(fColumnId).toString();
            textName += c.at(fColumnName).toString();
        }
        setText(textId);
        if (fNameLineEdit) {
            fNameLineEdit->setText(textName);
        }
        emit multiDone(values);
    } else {
        QList<QVariant> values;
        if (!C5Selector::getValue(fDBParams, fCache, values)) {
            return;
        }
        if (values.count() == 0) {
            return;
        }
        setText(values.at(fColumnId).toString());
        if (fNameLineEdit) {
            fNameLineEdit->setText(values.at(fColumnName).toString());
        }
        if (fWidget && values.count() > 0) {
            fWidget->selectorCallback(0, values);
        }
        if (fDialog && values.count() > 0) {
            fDialog->selectorCallback(0, values);
        }
        emit done(values);
    }
}

void C5LineEditWithSelector::focusOutEvent(QFocusEvent *e)
{
    if (fCache) {
        setValue(text());
    }
    C5LineEdit::focusOutEvent(e);
}

C5LineEditWithSelector *isLineEditWithSelector(QObject *o)
{
    C5LineEditWithSelector *le = dynamic_cast<C5LineEditWithSelector*>(o);
    return le;
}
