#ifndef C5GRID_H
#define C5GRID_H

#include "c5database.h"
#include "c5textdelegate.h"
#include "c5combodelegate.h"
#include "c5tableview.h"
#include <QWidget>
#include <QDebug>
#include <QTableView>

class C5TableModel;

namespace Ui {
class C5Grid;
}

class C5FilterWidget;

class C5Grid : public QWidget
{
    Q_OBJECT

public:
    enum ToolBarButtons {tbNone = 0, tbNew, tbEdit, tbSave, tbRefresh, tbFilter, tbClearFilter, tbPrint, tbExcel };

    explicit C5Grid(QWidget *parent = 0);

    ~C5Grid();

    void setDatabase(const QString &host, const QString &db, const QString &username, const QString &password);

    void setTableForUpdate(const QString &table, const QList<int> &columns);

    virtual void buildQuery();

protected:
    C5Database fDb;

    bool fSimpleQuery;

    QString fSqlQuery;

    C5TableView *fTableView;

    C5TableModel *fModel;

    QMap<QString, QString> fTranslation;

    QString fMainTable;

    QStringList fLeftJoinTables;

    QStringList fColumnsFields;

    QMap<QString, bool> fColumnsVisible;

    QStringList fColumnsGroup;

    QStringList fColumnsSum;

    QString fWhereCondition;

    C5FilterWidget *fFilterWidget;

    QWidget *widget();

    void callEditor(int id);

    void sumColumnsData();

private:
    Ui::C5Grid *ui;

    int fFilterColumn;

protected slots:
    virtual void saveDataChanges();

    virtual int newRow();

    virtual void editRow(int columnWidthId = 0);

    virtual void print();

    virtual void exportToExcel();

    virtual void clearFilter();

    virtual void refreshData();

    virtual void setSearchParameters();

    virtual void tableViewContextMenuRequested(const QPoint &point);
    \
    virtual void tableViewHeaderClicked(const QModelIndex &index);

private slots:    
    void filterByColumn();

    void removeFilterForColumn();
};

#endif // C5GRID_H
