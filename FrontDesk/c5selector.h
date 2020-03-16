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
    explicit C5Selector(const QStringList &dbParams);

    ~C5Selector();

    static bool getValue(const QStringList &dbParams, int cache, QList<QVariant> &values);

    static bool getMultipleValues(const QStringList &dbParams, int cache, QList<QList<QVariant> > &values);

    static bool getValues(const QStringList &dbParams, const QString &sql, QList<QVariant> &values, const QMap<QString, QString> &translator);

    void keyPressEvent(QKeyEvent *key) override;

    static void resetCache(const QStringList &dbParams, int cacheId);

private slots:
    void tblSingleClick(const QModelIndex &index);

    void tblDoubleClicked(int row, int column, const QList<QVariant> &values);

    void on_btnRefreshCache_clicked();

    void on_leFilter_textChanged(const QString &arg1);

    void on_btnCheck_clicked();

private:
    Ui::C5Selector *ui;

    int fCache;

    QString fQuery;

    void refresh();

    C5Grid *fGrid;

    QList<QVariant> fValues;

    static QMap<QString, QMap<int, C5Selector*> > fSelectorList;

    bool fReset;

    bool fMultipleSelection;
};

#endif // C5SELECTOR_H
