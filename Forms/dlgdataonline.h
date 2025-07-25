#ifndef DLGDATAONLINE_H
#define DLGDATAONLINE_H

#include "c5dialog.h"
#include <QItemSelection>

namespace Ui
{
class DlgDataOnline;
}

class C5TableModel;

class DlgDataOnline : public C5Dialog
{
    Q_OBJECT

public:

    class DataResult
    {
    public:
        QHash<QString, int> fColumns;
        QVector<QJsonArray > fRows;
        QVariant value(int row, const QString &column)
        {
            QJsonValue v = fRows[row][fColumns[column.toLower()] + 1];
            return v;
        }
    };

    explicit DlgDataOnline(const QString &table);

    ~DlgDataOnline();

    static bool get(const QString &table, DataResult &result, bool multiselect = false,
                    int searchcolumn = -1);

private slots:
    void on_leFilter_textChanged(const QString &arg1);

    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    virtual bool tblDoubleClicked(int row, int column, const QJsonArray &values);

    void on_btnRefreshCache_clicked();

    void on_btnCheck_clicked();

    void sqlError(const QString &errorMessage);

private:
    Ui::DlgDataOnline *ui;

    C5TableModel *fTableModel;

    int fSearchColumn;

    QString fQuery;

    void refresh();

    QVector<QJsonArray > fValues;

    bool fReset;

    bool fMultipleSelection;

    static QHash<QString, DlgDataOnline *> fInstances;
};

#endif // DLGDATAONLINE_H
