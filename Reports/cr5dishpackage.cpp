#include "cr5dishpackage.h"
#include "ce5dishpackage.h"
#include "ce5packagelist.h"
#include "c5tablemodel.h"

CR5DishPackage::CR5DishPackage(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/menu.png";
    fLabel = tr("Dish package");
    fSqlQuery = "select f_id, f_name, f_price, f_enabled from d_package";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_price"] = tr("Price");
    fTranslation["f_enabled"] = tr("Active");
    fEditor = new CE5DishPackage(dbParams);
}

QToolBar *CR5DishPackage::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
        QAction *a = new QAction(QIcon(":/edit.png"), tr("Edit"), this);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(editDishList()));
        fToolBar->insertAction(fToolBar->actions().at(1), a);
    }
    return fToolBar;
}

void CR5DishPackage::editDishList()
{
    int row = 0;
    if (!currentRow(row)) {
        return;
    }
    int id = fModel->data(row, 0, Qt::EditRole).toInt();
    CE5PackageList *pl = new CE5PackageList(fDBParams, id, this);
    pl->exec();
    delete pl;
}
