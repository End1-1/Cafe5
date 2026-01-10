#ifndef C5SELECTOR_H
#define C5SELECTOR_H

#include "c5dialog.h"
#include <QTableView>

namespace Ui
{
class C5Selector;
}

class C5Grid;
class C5TableModel;

class C5Selector : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5Selector(C5User *user);

    ~C5Selector();
    
    static bool getValue(C5User *user, int cache, QJsonArray &values);
    
    static bool getValue(C5User *user, const QString &query, QJsonArray &values);
    
    static bool getValueOfColumn(C5User *user, int cache, QJsonArray &values, int column);

    static bool getMultipleValues(C5User *user, int cache, QVector<QJsonArray> &values);

    static bool getValues(C5User *user, const QString &sql, QJsonArray &values,
                          const QHash<QString, QString> &translator);

    void keyPressEvent(QKeyEvent *key) override;

    static void resetCache(int cacheId);

private slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    virtual bool tblDoubleClicked(int row, int column, const QJsonArray &values);

    void on_btnRefreshCache_clicked();

    void on_leFilter_textChanged(const QString &arg1);

    void on_btnCheck_clicked();

private:
    Ui::C5Selector *ui;

    int fCache;

    int fSearchColumn;

    QString fQuery;

    void refresh();

    C5Grid *fGrid;

    QJsonArray fValues;

    static QMap<QString, QMap<int, C5Selector *> > fSelectorList;

    bool fReset;

    bool fMultipleSelection;
};

#endif // C5SELECTOR_H
