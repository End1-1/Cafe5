#ifndef DLGGOODSLIST_H
#define DLGGOODSLIST_H

#include "c5shopdialog.h"
#include <QJsonObject>

class QTimer;
class QKeyEvent;

namespace Ui
{
class DlgGoodsList;
}

class DlgGoodsList : public C5ShopDialog
{
    Q_OBJECT

public:
    explicit DlgGoodsList(C5User *user);

    ~DlgGoodsList();

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

    virtual void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_leSearch_textChanged(const QString &arg1);

    void on_btnExit_clicked();

    void on_btnMinimize_clicked();

    void onSearchCheckboxChanged();

    void runSearch();

    void loadAllStock();

private:
    Ui::DlgGoodsList* ui;

    int fGoodsId = 0;

    QTimer *mSearchTimer = nullptr;

    int mSearchGen = 0;

    void clearResults();

    void fillResults(const QJsonObject &jo);

    bool handleNavigationKey(int key);

    void acceptCurrentRow();

signals:
    void getGoods(int id, const QString &scancode, double stockQty = -1);
};

#endif // DLGGOODSLIST_H
