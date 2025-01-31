#include "c5tablewidget.h"
#include "c5lineedit.h"
#include "c5combobox.h"
#include "c5message.h"
#include "c5utils.h"
#include "c5checkbox.h"
#include "xlsxall.h"
#include <QHeaderView>
#include <QApplication>
#include <C5LineEditWithSelector.h>

C5TableWidget::C5TableWidget(QWidget *parent) :
    C5ClearTableWidget(parent)
{
}

C5ComboBox *C5TableWidget::createComboBox(int row, int column)
{
    C5ComboBox *c = new C5ComboBox(this);
    c->setProperty("row", row);
    c->setProperty("column", column);
    c->setFrame(false);
    setCellWidget(row, column, c);
    connect(c, SIGNAL(currentTextChanged(QString)), this, SLOT(comboTextChanged(QString)));
    return c;
}

C5ComboBox *C5TableWidget::comboBox(int row, int column)
{
    return static_cast<C5ComboBox *>(cellWidget(row, column));
}

void C5TableWidget::comboTextChanged(const QString &text)
{
    C5ComboBox *c = static_cast<C5ComboBox *>(sender());
    int row, col;
    findWidget(c, row, col);
    setString(row, col, text);
}
