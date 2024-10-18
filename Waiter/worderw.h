#ifndef WORDERW_H
#define WORDERW_H

#include <QWidget>

namespace Ui
{
class WOrder;
}

class C5OrderDriver;
class DlgOrder;
class DishItem;

class WOrder : public QWidget
{
    Q_OBJECT

public:
    explicit WOrder(QWidget *parent = nullptr);

    ~WOrder();

    void setDlg(DlgOrder *dlg);

    void updateDishes();

    void checkAllItems(bool v);

    QList<int> checkedItems();

    int addItem(int menuid, const QString &comment, double price, const QString &emark);

    void updateItem(int index);

    bool currentRow(int &row);

    void setCurrentRow(int row);

    void setSelected(bool v = true);

    bool isSelected();

    void setCheckMode(bool v);

    DishItem *dishWidget(int i);

    C5OrderDriver *fOrderDriver;

private:
    Ui::WOrder *ui;

    DlgOrder *fDlg;

    bool fSelected;

    QList<DishItem *> fItems;

    void setChanges();

private slots:
    QPoint focused(int index);

    void on_btnActivate_clicked();

    void on_btnEditGuestName_clicked();

signals:
    void activated();
};

#endif // WORDERW_H
