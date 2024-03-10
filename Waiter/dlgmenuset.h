#ifndef DLGMENUSET_H
#define DLGMENUSET_H

#include "c5dialog.h"

namespace Ui {
class DlgMenuSet;
}

class DlgMenuSet : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgMenuSet(const QString &bodyId);
    ~DlgMenuSet();

private slots:
    void on_toolButton_clicked();
    void addDish();
    void removeDish();

private:
    Ui::DlgMenuSet *ui;
    double fMax;

};

#endif // DLGMENUSET_H
