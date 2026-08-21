#ifndef DLGORDERDATAMATRIX_H
#define DLGORDERDATAMATRIX_H

#include "c5dialog.h"
#include <QStringList>

namespace Ui
{
class DlgOrderDatamatrix;
}

class DlgOrderDatamatrix : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgOrderDatamatrix(C5User *user, const QStringList &codes, QWidget *parent = nullptr);

    ~DlgOrderDatamatrix() override;

    QStringList codes() const;

    static bool editCodes(C5User *user, QStringList &codes, QWidget *parent = nullptr);

private slots:
    void on_leCode_returnPressed();

    void on_btnAdd_clicked();

    void on_btnRemove_clicked();

    void on_btnClear_clicked();

    void on_btnOk_clicked();

    void on_btnCancel_clicked();

private:
    Ui::DlgOrderDatamatrix *ui;

    void addCode(const QString &raw);

    void focusCodeInput();
};

#endif // DLGORDERDATAMATRIX_H
