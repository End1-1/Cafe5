#ifndef C5SELECTOR_H
#define C5SELECTOR_H

#include "c5dialog.h"
#include <QTableView>

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

    static bool getValue(const QStringList &dbParams, int cache, QVector<QJsonValue> &values);

    static bool getValue(const QStringList &dbParams, const QString &query, QVector<QJsonValue> &values);

    static bool getValueOfColumn(const QStringList &dbParams, int cache, QVector<QJsonValue> &values, int column);

    static bool getMultipleValues(const QStringList &dbParams, int cache, QList<QVector<QJsonValue> > &values);

    static bool getValues(const QStringList &dbParams, const QString &sql, QVector<QJsonValue> &values, const QHash<QString, QString> &translator);

    void keyPressEvent(QKeyEvent *key) override;

    static void resetCache(const QStringList &dbParams, int cacheId);

private slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    virtual bool tblDoubleClicked(int row, int column, const QVector<QJsonValue> &values);

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

    QVector<QJsonValue> fValues;

    static QMap<QString, QMap<int, C5Selector*> > fSelectorList;

    bool fReset;

    bool fMultipleSelection;
};

#endif // C5SELECTOR_H
