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
}

C5LineEditWithSelector::~C5LineEditWithSelector()
{

}

void C5LineEditWithSelector::setSelector(const QStringList &dbParams, QLineEdit *selName, int cacheId, int colId, int colName)
{
    fDBParams = dbParams;
    fNameLineEdit = selName;
    fCache = cacheId;
    fColumnId = colId;
    fColumnName = colName;
}

void C5LineEditWithSelector::setCallbackWidget(C5Widget *w)
{
    fWidget = w;
}

void C5LineEditWithSelector::setValue(const QString &id)
{
    setText(id);
    if (fCache == 0) {
        return;
    }
    C5Cache *c = C5Cache::cache(fDBParams, fCache);
    int row = c->find(id.toInt());
    if (row > -1) {
        QString text = c->getString(row, fColumnName);
        fNameLineEdit->setText(text);
    } else {
        clear();
        fNameLineEdit->clear();
    }
    if (fWidget && row > -1) {
        fWidget->selectorCallback(0, c->getRow(row));
    }
}

void C5LineEditWithSelector::setValue(int id)
{
    setValue(QString::number(id));
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
    QList<QVariant> values;
    if (!C5Selector::getValue(fDBParams, fCache, values)) {
        return;
    }
    setText(values.at(fColumnId).toString());
    if (fNameLineEdit) {
        fNameLineEdit->setText(values.at(fColumnName).toString());
    }
    if (fWidget && values.count() > 0) {
        fWidget->selectorCallback(0, values);
    }
    emit done(values);
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
