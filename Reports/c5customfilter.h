#ifndef C5CUSTOMFILTER_H
#define C5CUSTOMFILTER_H

#include "c5dialog.h"

namespace Ui {
class C5CustomFilter;
}

class QListWidgetItem;

class C5CustomFilter : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5CustomFilter();

    ~C5CustomFilter();

    void setQueries(const QStringList &names, const QStringList &sqls);

    QString sql() const;

private slots:
    void on_btnReject_clicked();

    void on_btnOK_clicked();

    void on_lw_itemClicked(QListWidgetItem *item);

private:
    Ui::C5CustomFilter *ui;

    QString fSql;

    QStringList fParams;

    QList<QWidget*> fWidgets;

};

#endif // C5CUSTOMFILTER_H
