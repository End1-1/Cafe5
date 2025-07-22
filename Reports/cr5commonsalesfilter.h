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
    explicit CR5CommonSalesFilter(QWidget *parent = nullptr);

    ~CR5CommonSalesFilter();

    virtual QString condition();

    QDate date1() const;

    QDate date2() const;

    bool complimentary();

    bool notComplimentary();

private slots:
    void on_btnFlags_clicked();

    void on_chUseClosingDateTime_clicked(bool checked);

    void on_checkBoxNotComplimentary_clicked(bool checked);

    void on_chptComplimentary_clicked(bool checked);

private:
    Ui::CR5CommonSalesFilter *ui;

    QString fFlags;
};

#endif // CR5COMMONSALESFILTER_H
