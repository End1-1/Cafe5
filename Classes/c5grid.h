#ifndef C5GRID_H
#define C5GRID_H

#include "c5database.h"
#include "c5textdelegate.h"
#include "c5combodelegate.h"
#include "c5tableview.h"
#include "c5config.h"
#include "c5widget.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QTableView>
#include <QSettings>

class C5TableModel;

namespace Ui {
class C5Grid;
}

class C5FilterWidget;

class C5Grid : public C5Widget
{
    Q_OBJECT

public:
    explicit C5Grid(const QStringList &dbParams, QWidget *parent = 0);

    ~C5Grid();

    C5TableView *fTableView;

    C5TableModel *fModel;

    void setTableForUpdate(const QString &table, const QList<int> &columns);

    virtual void postProcess();

    virtual void buildQuery();

    virtual void buildQuery(const QString &query);

    void setFilter(int column, const QString &filter);

public slots:
    void on_tblView_doubleClicked(const QModelIndex &index);

protected:
    C5Database fDb;

    bool fSimpleQuery;

    QString fSqlQuery;

    QMap<QString, QString> fTranslation;

    QString fMainTable;

    QStringList fLeftJoinTables;

    QStringList fColumnsFields;

    QMap<QString, bool> fColumnsVisible;

    QStringList fColumnsGroup;

    QStringList fColumnsSum;

    QString fWhereCondition;

    QString fHavindCondition;

    C5FilterWidget *fFilterWidget;

    QWidget *widget();

    QHBoxLayout *hl();

    int rowId();

    int rowId(int column);

    int rowId(int &row, int column);

    virtual void cellClicked(const QModelIndex &index);

    virtual void callEditor(int id);

    virtual void removeWithId(int id, int row);

    void sumColumnsData();

    void restoreColumnsVisibility();

    void restoreColumnsWidths();

    QStringList dbParams();

    virtual QString reportAdditionalTitle();

private:
    Ui::C5Grid *ui;

    int fFilterColumn;

    void insertJoinTable(QStringList &joins, QMap<QString, QString> &joinsMap, const QString &table, const QString &mainTable);

    int sumOfColumnsWidghtBefore(int column);

protected slots:
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    virtual void saveDataChanges();

    virtual int newRow();

    virtual void editRow(int columnWidthId = 0);

    virtual void removeRow(int columnWithId = 0);

    virtual void print();

    virtual void exportToExcel();

    virtual void clearFilter();

    virtual void refreshData();

    virtual void setSearchParameters();

    virtual void tableViewContextMenuRequested(const QPoint &point);
    \
    virtual void tableViewHeaderClicked(const QModelIndex &index);

    virtual void tableViewHeaderResized(int column, int oldSize, int newSize);

private slots:    
    void filterByColumn();

    void removeFilterForColumn();

    void on_tblView_clicked(const QModelIndex &index);

signals:
    void tblDoubleClicked(int row, int column, const QList<QVariant> &values);

};

#endif // C5GRID_H
