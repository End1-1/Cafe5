#ifndef C5TABLEWIDGETWITHSELECTOR_H
#define C5TABLEWIDGETWITHSELECTOR_H

#include "c5tablewidget.h"

class C5LineEditWithSelector;

class C5TableWidgetWithSelector : public C5TableWidget
{
    Q_OBJECT
public:
    C5TableWidgetWithSelector(QWidget *parent = nullptr);

    C5LineEditWithSelector *createLineEditWithSelector(int row, int column, QObject *receiver, char *slot);

    C5LineEditWithSelector *lineEditWithSelector(int row, int column);
};

#endif // C5TABLEWIDGETWITHSELECTOR_H
