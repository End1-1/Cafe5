#include "c5tablewidgetwithselector.h"
#include "c5lineeditwithselector.h"

C5TableWidgetWithSelector::C5TableWidgetWithSelector(QWidget *parent) :
    C5TableWidget(parent)
{

}

C5LineEditWithSelector *C5TableWidgetWithSelector::createLineEditWithSelector(int row, int column, QObject *receiver, char *slot)
{
    C5LineEditWithSelector *l = new C5LineEditWithSelector();
    l->setFrame(false);
    if (receiver != nullptr) {
        connect(l, SIGNAL(doubleClicked()), this, slot);
    }
    setCellWidget(row, column, l);
    return l;
}

C5LineEditWithSelector *C5TableWidgetWithSelector::lineEditWithSelector(int row, int column)
{
    return static_cast<C5LineEditWithSelector*>(cellWidget(row, column));
}
