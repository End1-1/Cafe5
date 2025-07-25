#ifndef CR5MFDAILY_H
#define CR5MFDAILY_H

#include "c5widget.h"

namespace Ui {
class CR5MfDaily;
}

class QListWidgetItem;

class CR5MfDaily : public C5Widget
{
    Q_OBJECT

public:
    explicit CR5MfDaily(QWidget *parent = nullptr);

    ~CR5MfDaily();

    virtual QToolBar *toolBar() override;

private:
    Ui::CR5MfDaily *ui;

    void loadDoc(const QDate &date, int worker, int teamlead);

    void refreshTasks();

private slots:
    void processTaskDbClick();

    void exportToExcel();

    void addWorker();

    void removeWorker();

    void addProcess();

    void removeProcess();

    void saveWork();

    void selectAllWorkers();

    void processQtyChanged(const QString &arg1);

    void on_btnDatePlus_clicked();

    void on_btnDateMinus_clicked();

    void on_lstWorkers_currentRowChanged(int currentRow);

    void on_leDate_returnPressed();

    void on_lstTeamlead_itemClicked(QListWidgetItem *item);

    void on_leFilterWorker_textChanged(const QString &arg1);

    void on_lstWorkers_itemClicked(QListWidgetItem *item);

    void on_btnRefreshTask_clicked();
};

#endif // CR5MFDAILY_H
