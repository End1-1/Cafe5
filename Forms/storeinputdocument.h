#ifndef STOREINPUTDOCUMENT_H
#define STOREINPUTDOCUMENT_H

#include "c5widget.h"

namespace Ui {
class StoreInputDocument;
}

class StoreInputDocument : public C5Widget
{
    Q_OBJECT

public:
    explicit StoreInputDocument(const QStringList &dbParams, QWidget *parent = nullptr);
    ~StoreInputDocument();
    virtual QToolBar *toolBar() override;
    bool openDoc(const QString &id);

private slots:
    void qtyChanged(const QString &arg1);
    void qtyBoxChanged(const QString &arg1);
    void priceChanged(const QString &arg1);
    void totalChanged(const QString &arg1);
    void on_btnDeleteRow_clicked();
    void on_btnAddRow_clicked();
    void on_toolButton_clicked();
    void saveDoc();
    bool draftDoc();
    void on_btnSetPartner_clicked();

private:
    QAction *fSave;
    QAction *fDraft;
    Ui::StoreInputDocument *ui;
    void countTotal();
    int newEmptyRow();
    void connectSlotSignal(int row);
};

#endif // STOREINPUTDOCUMENT_H
