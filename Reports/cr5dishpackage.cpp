#include "cr5dishpackage.h"
#include "ce5dishpackage.h"
#include "ce5packagelist.h"
#include "c5tablemodel.h"

CR5DishPackage::CR5DishPackage(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIconName = ":/menu.png";
    fLabel = tr("Dish package");
    fSqlQuery = "select p.f_id, m.f_name as f_menuname, p.f_name, p.f_price, p.f_enabled from d_package p left join d_menu_names m on m.f_id=p.f_menu ";
    fTranslation["f_id"] = tr("Code");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_price"] = tr("Price");
    fTranslation["f_enabled"] = tr("Active");
    fTranslation["f_menuname"] = tr("Menu");
    fEditor = new CE5DishPackage();
}

QToolBar* CR5DishPackage::toolBar()
{
    if(!fToolBar) {
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

    if(!currentRow(row)) {
        return;
    }

    int id = fModel->data(row, 0, Qt::EditRole).toInt();
    CE5PackageList *pl = new CE5PackageList(mUser, id);
    pl->exec();
    delete pl;
}
