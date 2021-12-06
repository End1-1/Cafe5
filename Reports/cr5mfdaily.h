#ifndef CR5MFDAILY_H
#define CR5MFDAILY_H

#include "c5widget.h"

namespace Ui {
class CR5MfDaily;
}

class CR5MfDaily : public C5Widget
{
    Q_OBJECT

public:
    explicit CR5MfDaily(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5MfDaily();

    virtual QToolBar *toolBar() override;

private:
    Ui::CR5MfDaily *ui;

    void loadDoc(const QDate &date);

private slots:
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
};

#endif // CR5MFDAILY_H
