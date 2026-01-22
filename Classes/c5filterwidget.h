#ifndef C5FILTERWIDGET_H
#define C5FILTERWIDGET_H

#include "c5utils.h"
#include "c5widget.h"

class C5LineEdit;
class C5LineEditWithSelector;

class C5FilterWidget : public C5Widget
{
    Q_OBJECT

public:
    explicit C5FilterWidget(QWidget *parent = nullptr);

    virtual QString condition() = 0;

    virtual QString filterText() {return "";}

    virtual QString conditionText();

    virtual void saveFilter(QWidget *parent);

    virtual void restoreFilter(QWidget *parent);

    virtual void clearFilter(QWidget *parent);

    virtual bool fixDates();

    void setFilterValue(const QString &key, const QVariant &value);

public slots:
    void setFixDate(bool v);

protected:
    QString in(QString &cond, const QString &field, C5LineEditWithSelector *l);

    QString inFilterText(QString &text, C5LineEdit *l);

    QString normalizeIntInList(const QString &text);

private:
    QWidget* getWidget(const QString &key, QWidget *parent);
};

#endif // C5FILTERWIDGET_H
