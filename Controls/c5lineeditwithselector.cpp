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

C5LineEditWithSelector& C5LineEditWithSelector::setSelector(QLineEdit *selName,
        int cacheId, int colId, int colName)
{
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
    if(id.isEmpty() || id == QLatin1String("0")) {
        clear();

        if(fNameLineEdit) {
            fNameLineEdit->clear();
        }

        return;
    }

    setText(id);

    if(fCache == 0) {
        return;
    }

    C5Cache *c = C5Cache::cache(fCache);
    // Selector dialog inserts a checkbox column at index 0; cache rows do not.
    // setSelector(colId, colName) uses selector indexes → shift -1 for cache.
    const int cacheColId = fColumnId > 0 ? fColumnId - 1 : 0;
    const int cacheColName = fColumnName > 0 ? fColumnName - 1 : 0;
    QString text;
    int row = -1;
    bool found = false;
    QStringList ids = id.split(",", Qt::SkipEmptyParts);

    foreach(const QString &s, ids) {
        const int idVal = s.toInt();

        if(idVal == 0) {
            continue;
        }

        row = c->find(idVal);

        // Cache may be stale (e.g. rows inserted by SQL while app was running).
        if(row < 0) {
            c->ensureId(idVal);
            row = c->find(idVal);
        }

        if(row > -1) {
            found = true;

            if(!text.isEmpty()) {
                text += ",";
            }

            text += c->getString(row, cacheColName);
        }
    }

    if(found) {
        // Keep the id even when the name column is empty.
        if(fNameLineEdit) {
            fNameLineEdit->setText(text);
        }
    } else {
        clear();

        if(fNameLineEdit) {
            fNameLineEdit->clear();
        }

        return;
    }

    if(!fMultiselection) {
        if(row > -1) {
            const QJsonArray &j = c->getRow(row);
            emit singleSelect(j.at(cacheColId).toVariant().toInt(), j.at(cacheColName).toString());
        }

        if(fWidget && row > -1) {
            fWidget->selectorCallback(fCache, c->getRow(row));
        }

        if(fDialog && row > -1) {
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
    if(!property("checkallowed").toBool()) {
        return C5LineEdit::text();
    }

    QString allowedChar("-0123456789,");
    QString t = C5LineEdit::text().trimmed();

    if(fCache > 0) {
        for(int i = t.length() - 1; i > -1; i--) {
            if(!allowedChar.contains(t.at(i))) {
                t.remove(i, 1);
            }
        }
    }

    int i = t.length() - 1;

    while(i > 0) {
        if(t.at(i) == "-") {
            t.remove(i, 1);
        }

        if(t.at(i) == ",") {
            if(t.at(i - 1) == ",") {
                t.remove(i, 1);
            }
        }

        i--;
    }

    if(t.length() > 0) {
        if(t.at(0) == ",") {
            t.remove(0, 1);
        }
    }

    if(t.length() > 0) {
        if(t.at(t.length() - 1) == ",") {
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

bool C5LineEditWithSelector::hasSelector()
{
    return fCache > 0;
}

void C5LineEditWithSelector::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e);

    if(fCache == 0) {
        emit doubleClicked();
        return;
    }

    if(fMultiselection) {
        QVector<QJsonArray > values;

        if(!C5Selector::getMultipleValues(nullptr, fCache, values)) {
            return;
        }

        QString textId, textName;

        foreach(const QJsonArray &c, values) {
            if(!textId.isEmpty()) {
                textId += ",";
                textName += ",";
            }

            textId += QString::number(c.at(fColumnId).toInt());
            textName += c.at(fColumnName).toString();
        }

        setText(textId);

        if(fNameLineEdit) {
            fNameLineEdit->setText(textName);
        }
    } else {
        QJsonArray values;

        if(!C5Selector::getValue(nullptr, fCache, values)) {
            return;
        }

        if(values.count() == 0) {
            return;
        }

        setText(QString::number(values.at(fColumnId).toInt()));

        if(fNameLineEdit) {
            fNameLineEdit->setText(values.at(fColumnName).toString());
        }

        if(fWidget && values.count() > 0) {
            fWidget->selectorCallback(0, values);
        }

        if(fDialog && values.count() > 0) {
            fDialog->selectorCallback(0, values);
        }
    }
}

void C5LineEditWithSelector::focusOutEvent(QFocusEvent *e)
{
    if(fCache) {
        setValue(text());
    }

    C5LineEdit::focusOutEvent(e);
}

C5LineEditWithSelector* isLineEditWithSelector(QObject *o)
{
    C5LineEditWithSelector *le = dynamic_cast<C5LineEditWithSelector*>(o);
    return le;
}
