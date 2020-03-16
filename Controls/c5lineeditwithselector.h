#ifndef C5LINEEDITWITHSELECTOR_H
#define C5LINEEDITWITHSELECTOR_H

#include "c5lineedit.h"

class C5Widget;
class C5Dialog;

class C5LineEditWithSelector : public C5LineEdit
{
    Q_OBJECT

public:
    C5LineEditWithSelector(QWidget *parent = nullptr);

    ~C5LineEditWithSelector();

    C5LineEditWithSelector &setSelector(const QStringList &dbParams, QLineEdit *selName, int cacheId, int colId = 1, int colName = 2);

    void setMultiselection(bool v);

    void setCallbackWidget(C5Widget *w);

    void setCallbackDialog(C5Dialog *d);

    void setValue(const QString &id);

    void setValue(int id);

    int cacheId();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *e);

    virtual void focusOutEvent(QFocusEvent *e);

private:
    QStringList fDBParams;

    int fCache;

    int fColumnId;

    int fColumnName;

    QLineEdit *fNameLineEdit;

    C5Widget *fWidget;

    C5Dialog *fDialog;

    bool fMultiselection;

signals:
    void done(const QList<QVariant> &values);

    void multiDone(const QList<QList<QVariant> > &values);

    void doubleClicked();

};

C5LineEditWithSelector *isLineEditWithSelector(QObject *o);

#endif // C5LINEEDITWITHSELECTOR_H
