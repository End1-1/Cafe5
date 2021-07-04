#ifndef WORDER_H
#define WORDER_H

#include <QWidget>

namespace Ui {
class WOrder;
}

class C5OrderDriver;
class DlgOrder;

class WOrder : public QWidget
{
    Q_OBJECT

public:
    explicit WOrder(QWidget *parent = nullptr);

    ~WOrder();

    void setDlg(DlgOrder *dlg);

    void addStretch();

    void itemsToTable();

    int addItem(QJsonObject o);

    QPoint updateItem(int index);

    bool currentRow(int &row);

    void setSelected(bool v = true);

    bool isSelected();

    C5OrderDriver *fOrderDriver;

private:
    Ui::WOrder *ui;

    DlgOrder *fDlg;

    bool fSelected;

    void setChanges();

private slots:
    QPoint focused(int index);

    void on_btnActivate_clicked();

signals:
    void activated();
};

#endif // WORDER_H
