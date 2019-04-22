#ifndef C5DBUSERACCESS_H
#define C5DBUSERACCESS_H

#include "c5widget.h"

namespace Ui {
class C5DbUserAccess;
}

class C5DbUserAccess : public C5Widget
{
    Q_OBJECT

public:
    explicit C5DbUserAccess(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5DbUserAccess();

    QToolBar *toolBar();

private:
    Ui::C5DbUserAccess *ui;

    QMap<int, int> fUserMap;

    QMap<int, int> fDbMap;

private slots:
    void saveDataChanges();

    void refreshData();
    void on_leUsername_textChanged(const QString &arg1);
};

#endif // C5DBUSERACCESS_H
