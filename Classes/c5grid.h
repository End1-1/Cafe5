#ifndef C5GRID_H
#define C5GRID_H

#include <QWidget>
#include "c5database.h"

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

    void buildQuery();

protected:
    bool fSimpleQuery;

    QString fSqlQuery;

    QMap<QString, QString> fTranslation;

private:
    Ui::C5Grid *ui;

    C5Database fDb;

    C5TableModel *fModel;

private slots:
    virtual void refreshData();
};

#endif // C5GRID_H
