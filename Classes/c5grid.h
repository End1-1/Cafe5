#ifndef C5GRID_H
#define C5GRID_H

#include "c5database.h"
#include "c5textdelegate.h"
#include "c5datedelegate.h"
#include "c5combodelegate.h"
#include "c5tableview.h"
#include "c5config.h"
#include "c5message.h"
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
class CE5Editor;
class C5TableWidget;

class C5Grid : public C5Widget
{
    Q_OBJECT

public:
    explicit C5Grid(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5Grid();

    C5TableView *fTableView;

    C5TableWidget *fTableTotal;

    C5TableModel *fModel;

    QHash<QString, QString> fTranslation;

    void setTableForUpdate(const QString &table, const QList<int> &columns);

    virtual void postProcess();

    virtual void buildQuery();

    virtual void buildQuery(const QString &query);

    void setFilter(int column, const QString &filter);

    virtual void hotKey(const QString &key);

    virtual void changeDatabase(const QStringList &dbParams);

    virtual void setCheckboxes(bool v);

public slots:
    virtual bool on_tblView_doubleClicked(const QModelIndex &index);

protected:
    C5Database fDb;

    bool fSimpleQuery;

    QString fSqlQuery;

    QString fMainTable;

    QStringList fLeftJoinTables;

    QStringList fColumnsFields;

    QHash<QString, bool> fColumnsVisible;

    QStringList fColumnsGroup;

    QStringList fColumnsSum;

    QStringList fColumnsOrder;

    QString fWhereCondition;

    QString fHavindCondition;

    QString fGroupCondition;

    QString fOrderCondition;

    C5FilterWidget *fFilterWidget;

    CE5Editor *fEditor;

    QWidget *widget();

    QHBoxLayout *hl();

    int rowId();

    int rowId(int column);

    int rowId(int &row, int column);

    virtual void cellClicked(const QModelIndex &index);

    virtual void callEditor(const QString &id);

    virtual void removeWithId(int id, int row);

    void sumColumnsData();

    virtual void restoreColumnsVisibility();

    virtual void restoreColumnsWidths();

    QStringList dbParams();

    virtual QString reportAdditionalTitle();

    virtual QMenu *buildTableViewContextMenu(const QPoint &point);

    virtual bool tblDoubleClicked(int row, int column, const QList<QVariant> &values);

private:
    Ui::C5Grid *ui;

    QModelIndex fFilterIndex;

    QString fFilterString;

    bool fCheckboxes;

    int fFilterColumn;

    void insertJoinTable(QStringList &joins, QMap<QString, QString> &joinsMap, const QString &table, const QString &mainTable);

    int sumOfColumnsWidghtBefore(int column);

    QString columnName(const QString &s) const;

protected slots:
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    virtual void copySelection();

    virtual void copyAll();

    virtual void saveDataChanges();

    virtual int newRow();

    bool currentRow(int &row);

    virtual void editRow(int columnWidthId = 0);

    virtual void removeRow(int columnWithId = 0);

    virtual void print();

    virtual void exportToExcel();

    virtual void clearFilter();

    virtual void refreshData();

    virtual void setSearchParameters();

    virtual void tableViewContextMenuRequested(const QPoint &point);

    virtual void tableViewHeaderContextMenuRequested(const QPoint &point);

    virtual void tableViewHeaderClicked(int index);

    virtual void tableViewHeaderResized(int column, int oldSize, int newSize);

private slots:    
    void ctrlEnter();

    void autofitColumns();

    void filterByColumn();

    void filterByStringAndIndex();

    void removeFilterForColumn();

    void tblValueChanged(int pos);

    void on_tblView_clicked(const QModelIndex &index);

signals:
    void tblSingleClick(const QModelIndex &);

    void tblDoubleClick(int row, int column, const QList<QVariant> &values);

    void refreshed();


};

#endif // C5GRID_H
