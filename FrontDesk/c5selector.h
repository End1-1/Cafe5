#ifndef C5SELECTOR_H
#define C5SELECTOR_H

#include "c5dialog.h"

namespace Ui {
class C5Selector;
}

class C5Grid;

class C5TableModel;
class C5Selector : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5Selector(const QStringList &dbParams, QWidget *parent = 0);

    ~C5Selector();

    static bool getValue(const QStringList &dbParams, int cache, QList<QVariant> &values);

private slots:
    void tblDoubleClicked(int row, int column, const QList<QVariant> &values);

    void on_btnRefreshCache_clicked();

    void on_leFilter_textChanged(const QString &arg1);

private:
    Ui::C5Selector *ui;

    int fCache;

    QString fQuery;

    void refresh();

    C5Grid *fGrid;

    QList<QVariant> fValues;

    static QMap<QString, QMap<int, C5Selector*> > fSelectorList;
};

#endif // C5SELECTOR_H