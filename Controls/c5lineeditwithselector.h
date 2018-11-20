#ifndef C5LINEEDITWITHSELECTOR_H
#define C5LINEEDITWITHSELECTOR_H

#include "c5lineedit.h"

class C5LineEditWithSelector : public C5LineEdit
{
    Q_OBJECT

public:
    C5LineEditWithSelector(QWidget *parent = 0);

    ~C5LineEditWithSelector();

    void setSelector(const QStringList &dbParams, QLineEdit *selName, int cacheId, int colId = 0, int colName = 1);

    void setValue(const QString &id);

    void setValue(int id);

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *e);

    virtual void focusOutEvent(QFocusEvent *e);

private:
    QStringList fDBParams;

    int fCache;

    int fColumnId;

    int fColumnName;

    QLineEdit *fNameLineEdit;

signals:
    void done(const QList<QVariant> &values);
};

#endif // C5LINEEDITWITHSELECTOR_H
