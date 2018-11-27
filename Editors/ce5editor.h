#ifndef CE5EDITOR_H
#define CE5EDITOR_H

#include "c5widget.h"
#include <QSet>

class C5LineEditWithSelector;
class C5CheckBox;
class C5DateEdit;

class CE5Editor : public C5Widget
{
    Q_OBJECT

public:
    CE5Editor(const QStringList &dbParams, QWidget *parent = 0);

    virtual void setId(int id);

    virtual QString title() = 0;

    virtual QString table() = 0;

    virtual bool save(QString &err, QList<QMap<QString, QVariant> > &data);

    virtual bool checkData(QString &err);

    virtual void clear();

    void getLineEdit(QObject *parent);

    void focusFirst();

private:
    C5LineEditWithSelector *findLineEditWithId();

    QSet<C5LineEditWithSelector*> fLines;

    QSet<C5CheckBox*> fChecks;

    QSet<C5DateEdit*> fDates;
};

#endif // CE5EDITOR_H
