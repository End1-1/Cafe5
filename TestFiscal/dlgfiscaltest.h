#ifndef DLGFISCALTEST_H
#define DLGFISCALTEST_H

#include <QDialog>

class QCloseEvent;
class QShowEvent;
class QTimer;

namespace Ui {
class DlgFiscalTest;
}

class DlgFiscalTest : public QDialog
{
    Q_OBJECT

public:
    explicit DlgFiscalTest(QWidget *parent = nullptr);
    ~DlgFiscalTest() override;

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void on_btnAddRow_clicked();
    void on_btnRemoveRow_clicked();
    void on_btnPrint_clicked();
    void scheduleSaveState();
    void saveState();

private:
    enum class PrintMode { Detailed = 2, Simple = 1 };

    void loadState();
    void setupAutoSave();
    void updatePrintModeUi();
    void addGoodsRow(const QString &name = QString(),
                     double price = 100.0,
                     double qty = 1.0,
                     int taxDept = 1,
                     const QString &adg = QString(),
                     double discount = 0.0,
                     const QString &code = QString());
    PrintMode currentPrintMode() const;

    Ui::DlgFiscalTest *ui;
    QTimer *m_saveTimer = nullptr;
    bool m_loadingState = false;
    bool m_stateLoaded = false;
};

#endif // DLGFISCALTEST_H
