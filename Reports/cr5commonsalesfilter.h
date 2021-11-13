#ifndef CR5COMMONSALESFILTER_H
#define CR5COMMONSALESFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5CommonSalesFilter;
}

class CR5CommonSalesFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5CommonSalesFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5CommonSalesFilter();

    virtual QString condition();

    QDate date1() const;

    QDate date2() const;

private slots:
    void on_btnFlags_clicked();

private:
    Ui::CR5CommonSalesFilter *ui;

    QString fFlags;
};

#endif // CR5COMMONSALESFILTER_H
