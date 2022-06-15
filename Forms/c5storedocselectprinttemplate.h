#ifndef C5STOREDOCSELECTPRINTTEMPLATE_H
#define C5STOREDOCSELECTPRINTTEMPLATE_H

#include <QDialog>

namespace Ui {
class C5StoreDocSelectPrintTemplate;
}

class C5StoreDocSelectPrintTemplate : public QDialog
{
    Q_OBJECT

public:
    explicit C5StoreDocSelectPrintTemplate(QWidget *parent = nullptr);
    ~C5StoreDocSelectPrintTemplate();

private slots:
    void on_btnCancel_clicked();

    void on_btnPrintOneDoc_clicked();

    void on_btnPrintTwoDoc_clicked();

private:
    Ui::C5StoreDocSelectPrintTemplate *ui;
};

#endif // C5STOREDOCSELECTPRINTTEMPLATE_H
