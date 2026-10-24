#pragma once

#include "c5widget.h"
#include "dlgstoreinputpayment.h"
#include <QDate>
#include <QShowEvent>

class QJsonObject;

namespace Ui {
class C5SalaryPaymentEditor;
}

class C5SalaryPaymentEditor : public C5Widget
{
    Q_OBJECT

public:
    explicit C5SalaryPaymentEditor(QWidget *parent = nullptr);
    ~C5SalaryPaymentEditor() override;

    QToolBar *toolBar() override;

    void open(const QDate &date);

private slots:
    void on_btnAddStaff_clicked();
    void on_btnRemoveStaff_clicked();
    void on_btnChangePosition_clicked();
    void saveDocument();
    void removeDocument();
    void on_btnPaymentConfig_clicked();
    void on_cbPayment_currentIndexChanged(int index);

private:
    enum Columns {
        colDbId = 0,
        colPositionId,
        colStaffId,
        colNum,
        colPosition,
        colName,
        colDebt,
        colAmount
    };

    Ui::C5SalaryPaymentEditor *ui;

    void initTable();
    void recalcTotal();
    void appendStaffRow(int staffId, const QString &staffName, int positionId, const QString &positionName, double debt);
    void offerAutofillFromAccrual(const QDate &date);
    void fillFromAccrual(const QDate &date);
    QString buildPrintHtml() const;
    void initPaymentSource();
    void applyPaymentSource(const QJsonObject &payment);
    void selectPayment(const StorePaymentPreset &preset);
    void rebuildPaymentCombo();
    QVector<QPair<int, QString>> loadCashboxList() const;
    void sendSave(QJsonObject params);

protected slots:
    void print() override;

protected:
    void showEvent(QShowEvent *e) override;

private:
    bool mLoadOnShow = false;
    bool mPaymentNamesLoaded = false;
    QDate mOpenDate;
    StorePaymentOptions mPaymentOptions;
    QVector<StorePaymentPreset> mPaymentPresets;
    StorePaymentPreset mCurrentPayment;
};
