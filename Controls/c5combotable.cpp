#include "c5combotable.h"
#include "combosearchview.h"
#include <QTableView>
#include <QCompleter>

C5ComboTable::C5ComboTable(QWidget *parent) :
    QComboBox(parent)
{
    setEditable(true);
    fModel.appendRow({new QStandardItem("R1"), new QStandardItem("R2")});
    setModel(&fModel);

    QCompleter *c = new QCompleter();
    c->setCaseSensitivity(Qt::CaseInsensitive);
    c->setModel(&fModel);
    c->setPopup(new QTableView());
    c->setCompletionColumn(1);
    c->popup()->setMinimumHeight(500);
    c->popup()->setMinimumWidth(600);
    setCompleter(c);
}
