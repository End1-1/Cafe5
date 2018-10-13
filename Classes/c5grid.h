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

class C5Grid : public QWidget
{
    Q_OBJECT

public:
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

    QMap<QString, QString> fTranslation;

    int newRow();

private:
    Ui::C5Grid *ui;

    C5TableModel *fModel;

protected slots:
    virtual void saveDataChanges();

private slots:
    virtual void refreshData();
};

#endif // C5GRID_H
